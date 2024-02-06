#include "UnaryParser.h"

UnaryParser::UnaryParser(
        std::string operatorType,
        TExpressions::ExpressionConstructor makeExpression
    ):
    _operatorType(operatorType),
    _makeExpression(makeExpression) {
    
}

std::shared_ptr<DExpression> UnaryParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto firstToken = tokenSequence.consume();
    
    // If the first token we encounter is the operator.
    if (firstToken.type == this->_operatorType) {
        // Get the expression after the operator.
        auto followingExpression = this->_makeExpression();
        auto followingExpressionValue = followingExpression->parse(tokens, tokenSequence.position());

        auto unaryParser = std::shared_ptr<DExpression>(
            new DExpression{
                {{followingExpressionValue}},
                this->_operatorType,
                position,
                followingExpressionValue->endPos
            }
        );
        followingExpressionValue->parent = unaryParser;

        return unaryParser;
    } else {
        throw std::runtime_error("Expected the operator: " + this->_operatorType);
    }
}