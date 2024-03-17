#include "TypeParser.h"

MyLanguage::TypeParser::TypeParser()
{
    this->_literalParser = std::unique_ptr<LiteralParser>(new LiteralParser{"identifier"});
    this->_parameterListParser = std::unique_ptr<Parsing::ListParser>(
        new Parsing::ListParser{"function-type-parameter-list", "(", ")", ",", *this}
    );
    this->_functionTypeParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "function-type",
            {
                {"expression", "L"},
                {"token-value", "=>"},
                {"expression", "T"}
            },
            {
                {"L", this->_parameterListParser.get()},
                {"T", this}
            }
        }
    );
    this->_parentheticalParser = std::unique_ptr<Parsing::ParentheticalParser>(
        new Parsing::ParentheticalParser{"parenthetical", "(", ")", *this}
    );
    this->_mainParser = std::unique_ptr<Parsing::ConflictParser>(
        new Parsing::ConflictParser{
            {
                this->_literalParser.get(),
                this->_functionTypeParser.get(),
                this->_parentheticalParser.get()
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::TypeParser::parse(std::vector<DToken>& tokens, int position)
{
    auto expression = this->_mainParser->parse(tokens, position);

    // If the expression is wrapped in parentheses, we want to strip them.
    if (expression->type() == "parenthetical") {
        auto typeExpression = expression->children().at(0);

        auto& typeName = typeExpression->subTypes().at("name");
        typeName = "(" + typeName + ")";
        
        typeExpression->setStartPos(position);
        typeExpression->setEndPos(typeExpression->endPos() + 1);
        expression = typeExpression;
    }

    // Parse any potential following pointer asterisks.
    auto sequence = TokenSequence{tokens};
    sequence.setPosition(expression->endPos());
    while (sequence.peek().value == "*") {
        auto& typeName = expression->subTypes().at("name");
        typeName = typeName + "*";
        sequence.consume();
    }

    // For function types we want to generate their type name.
    if (expression->type() == "function-type") {
        bool first = true;
        std::string paramListTypes = std::string("(") + std::accumulate(
            expression->children().at(0)->children().begin(), 
            expression->children().at(0)->children().end(),
            std::string(""), 
            [&first](auto result, auto typeParam) {
                if (!first) {
                    result = result + ", ";
                }
                return result + typeParam->subTypes().at("name");
                first = false;
        }) + ")";
        auto returnType = expression->children().at(1)->subTypes().at("name");
        expression->subTypes().insert({"name", paramListTypes + " => " +  returnType});
    }

    expression->setChildren({});
    expression->setStartPos(position);
    expression->setEndPos(sequence.position());
    expression->setType("type");

    return expression;
}