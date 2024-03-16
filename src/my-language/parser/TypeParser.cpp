#include "TypeParser.h"

MyLanguage::TypeParser::TypeParser()
{
    this->_literalParser = std::unique_ptr<LiteralParser>(new LiteralParser{"identifier"});
    this->_parentheticalParser = std::unique_ptr<Parsing::ParentheticalParser>(
        new Parsing::ParentheticalParser{"parenthetical", "(", ")", *this}
    );
}

std::shared_ptr<Expression> MyLanguage::TypeParser::parse(std::vector<DToken>& tokens, int position)
{
    auto expression = this->_literalParser->parse(tokens, position);
    // If the expression is wrapped in parentheses, we want to strip them.
    if (expression->type() == "parenthetical") {
        auto typeExpression = expression->children().at(0);
        auto& typeName = typeExpression->subTypes().at("literal-value");
        typeName = "(" + typeName + ")";
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