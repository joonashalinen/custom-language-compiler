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

        if (typeExpression->type() == "identifier") {
            auto& typeName = typeExpression->subTypes().at("literal-value");
            typeName = "(" + typeName + ")";
            
        } else if (typeExpression->type() == "function-type") {
            auto& typeName = typeExpression->subTypes().at("name");
            typeName = "(" + typeName + ")";
        }

        typeExpression->setStartPos(position);
        typeExpression->setEndPos(typeExpression->endPos() + 1);
        expression = typeExpression;
    }
    // Parse any potential following pointer asterisks.
    auto sequence = TokenSequence{tokens};
    sequence.setPosition(expression->endPos());
    while (sequence.peek().value == "*") {
        auto& typeName = expression->subTypes().at("literal-value");
        typeName = typeName + "*";
        sequence.consume();
    }
    expression->setEndPos(sequence.position());
    expression->setType("type");
    expression->subTypes().insert({"name", expression->subTypes().at("literal-value")});
    return expression;
}