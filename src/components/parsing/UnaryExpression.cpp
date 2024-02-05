#include "UnaryExpression.h"

UnaryExpression::UnaryExpression(
        std::vector<DToken<>>& tokens,
        std::string operatorType,
        TExpressions::ExpressionConstructor makeExpression
    ):
    _tokenSequence(TokenSequence{tokens}), 
    _tokens(tokens),
    _operatorType(operatorType),
    _makeExpression(makeExpression) {
    
}

std::shared_ptr<DExpression> UnaryExpression::parse(int position) {
    this->_tokenSequence.setPosition(position);
    auto firstToken = this->_tokenSequence.consume();
    
    // If the first token we encounter is the operator.
    if (firstToken.type == this->_operatorType) {
        // Get the expression after the operator.
        auto followingExpression = this->_makeExpression(this->_tokens);
        auto followingExpressionValue = followingExpression->parse(this->_tokenSequence.position());

        auto unaryExpression = new DExpression{
            {{followingExpressionValue}},
            this->_operatorType,
            position,
            followingExpressionValue->endPos
        };
        return std::shared_ptr<DExpression>(unaryExpression);
    } else {
        throw std::runtime_error("Expected the operator: " + this->_operatorType);
    }
}