#include "UnaryParser.h"

UnaryParser::UnaryParser(
        std::string operatorType,
        TExpressions::ExpressionConstructor makeExpression
    ):
    _operatorType(operatorType),
    _makeExpression(makeExpression) {
    
}

std::shared_ptr<Expression> UnaryParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto firstToken = tokenSequence.consume();
    
    // If the first token we encounter is the operator.
    if (firstToken.type == this->_operatorType) {
        // Get the expression after the operator.
        auto followingParser = this->_makeExpression();
        auto followingExpression = followingParser->parse(tokens, tokenSequence.position());

        auto unaryExpression = std::shared_ptr<Expression>(
            new Expression{
                this->_operatorType,
                position,
                followingExpression->endPos()
            }
        );
        Expression::addChild(unaryExpression, followingExpression);
        return unaryExpression;
    } else {
        throw std::runtime_error("Expected the operator: " + this->_operatorType);
    }
}