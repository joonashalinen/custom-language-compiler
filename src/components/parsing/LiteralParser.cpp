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
        return std::shared_ptr<Expression>(
            new Expression{
                this->_type,
                position,
                position + 1
            }
        );
    } else {
        throw std::runtime_error("Expected the next token to have type " + this->_type);
    }
}