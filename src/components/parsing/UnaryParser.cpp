#include "UnaryParser.h"

UnaryParser::UnaryParser(
        std::string operatorType,
        std::set<std::string> acceptableOperators,
        IParseable& expressionParser
    ):
    _operatorType(operatorType),
    _expressionParser(expressionParser),
    _acceptableOperators(acceptableOperators) {
    
}

std::shared_ptr<Expression> UnaryParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto firstToken = tokenSequence.consume();
    
    // If the first token we encounter is the operator.
    if (this->_acceptableOperators.contains(firstToken.type)) {
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
        unaryExpression->subTypes().insert({"name", firstToken.value});

        Expression::addChild(unaryExpression, followingExpression);
        return unaryExpression;
    } else {
        throw std::runtime_error("Expected the operator '" + this->_operatorType + "' at " + firstToken.startLocation.toString());
    }
}