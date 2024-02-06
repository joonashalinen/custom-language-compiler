#include "LiteralExpression.h"

LiteralExpression::LiteralExpression(
        std::vector<DToken>& tokens,
        std::string type
    ):
    _tokenSequence(TokenSequence{tokens}), 
    _tokens(tokens),
    _type(type) {
    
}

std::shared_ptr<DExpression> LiteralExpression::parse(int position)
{
    return std::shared_ptr<DExpression>(
        new DExpression{
            {},
            this->_type,
            position,
            position + 1
        }
    );
}