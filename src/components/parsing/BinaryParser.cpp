#include "BinaryParser.h"
#include <iostream>
#include <cassert>

BinaryParser::BinaryParser(
        std::string operatorType,
        std::set<std::string> acceptedOperators,
        MapParser& mapParser
    ):
    _operatorType(operatorType),
    _acceptedOperators(acceptedOperators),
    _mapParser(mapParser) {
    
}

std::shared_ptr<Expression> BinaryParser::parse(std::vector<DToken>& tokens, int position) {
    // Get the expression preceding the binary operator.
    auto firstExpression = this->_mapParser.parse(tokens, position);

    // Peek at the next token.
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(firstExpression->endPos());
    auto nextToken = tokenSequence.consume();

    // If the next token is the operator we were expecting.
    if (this->_acceptedOperators.contains(nextToken.type)) {
        // Get the expression succeeding the binary operator.
        auto secondExpression = this->_mapParser.parse(tokens, tokenSequence.position());

        // Return the parsed binary expression.
        auto binaryExpression = std::shared_ptr<Expression>(
            new Expression{
                this->_operatorType,
                position,
                secondExpression->endPos()
            }
        );
        binaryExpression->tokens().insert(binaryExpression->tokens().end(), nextToken);
        binaryExpression->subTypes().insert({"name", nextToken.value});
        Expression::addChild(binaryExpression, firstExpression);
        Expression::addChild(binaryExpression, secondExpression);
        return binaryExpression;
    } else {
        throw std::runtime_error(
            "Expected the binary operator: " + this->_operatorType + " at " + 
            nextToken.startLocation.toString()
        );
    }
}