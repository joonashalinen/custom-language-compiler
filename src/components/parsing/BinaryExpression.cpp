#include "BinaryExpression.h"

BinaryExpression::BinaryExpression(
        std::string operatorType,
        ExpressionMap& expressionMap
    ):
    _operatorType(operatorType),
    _expressionMap(expressionMap) {
    
}

std::shared_ptr<DExpression> BinaryExpression::parse(std::vector<DToken>& tokens, int position) {
    // Get the expression preceding the binary operator.
    auto firstExpression = this->_expressionMap.parse(tokens, position);

    // Peek at the next token.
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(firstExpression->endPos);
    auto nextToken = tokenSequence.consume();

    // If the next token is the operator we were expecting.
    if (nextToken.type == this->_operatorType) {
        // Get the expression succeeding the binary operator.
        auto secondExpression = this->_expressionMap.parse(tokens, tokenSequence.position());

        // Return the parsed binary expression.
        auto binaryExpression = new DExpression{
            {{firstExpression, secondExpression}},
            this->_operatorType,
            position,
            secondExpression->endPos
        };
        return std::shared_ptr<DExpression>(binaryExpression);
    } else {
        throw std::runtime_error("Expected the operator: " + this->_operatorType);
    }
}