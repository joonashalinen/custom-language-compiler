#include "LiteralParser.h"

LiteralParser::LiteralParser(
        std::string type
    ):
    _type(type) {
    
}

std::shared_ptr<Expression> LiteralParser::parse(std::vector<DToken>& tokens, int position)
{
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto nextToken = tokenSequence.consume();
    
    if (nextToken.type == this->_type) {
        auto expression = std::shared_ptr<Expression>(
            new Expression{
                this->_type,
                position,
                position + 1
            }
        );
        expression->tokens().insert(expression->tokens().end(), nextToken);
        expression->subTypes().insert({"literal-value", nextToken.value});
        return expression;
    } else {
        throw std::runtime_error(
            "Expected a literal expression of type '" + this->_type + "' at " + 
            nextToken.startLocation.toString()
        );
    }
}