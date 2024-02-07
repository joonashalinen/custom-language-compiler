#include "UnaryParser.h"

UnaryParser::UnaryParser(
        std::string operatorType,
        IParseable& expressionParser
    ):
    _operatorType(operatorType),
    _expressionParser(expressionParser){
    
}

std::shared_ptr<Expression> UnaryParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto firstToken = tokenSequence.consume();
    
    // If the first token we encounter is the operator.
    if (firstToken.type == this->_operatorType) {
        // Get the expression after the operator.
        auto followingExpression = this->_expressionParser.parse(tokens, tokenSequence.position());

        auto unaryExpression = std::shared_ptr<Expression>(
            new Expression{
                this->_operatorType,
                position,
                followingExpression->endPos()
            }
        );
        unaryExpression->tokens().insert(unaryExpression->tokens().end(), firstToken);
        
        Expression::addChild(unaryExpression, followingExpression);
        return unaryExpression;
    } else {
        throw std::runtime_error("Expected the operator: " + this->_operatorType);
    }
}