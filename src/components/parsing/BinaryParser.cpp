#include "BinaryParser.h"

BinaryParser::BinaryParser(
        std::string operatorType,
        MapParser& mapParser
    ):
    _operatorType(operatorType),
    _mapParser(mapParser) {
    
}

std::shared_ptr<DExpression> BinaryParser::parse(std::vector<DToken>& tokens, int position) {
    // Get the expression preceding the binary operator.
    auto firstExpression = this->_mapParser.parse(tokens, position);

    // Peek at the next token.
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(firstExpression->endPos);
    auto nextToken = tokenSequence.consume();

    // If the next token is the operator we were expecting.
    if (nextToken.type == this->_operatorType) {
        // Get the expression succeeding the binary operator.
        auto secondExpression = this->_mapParser.parse(tokens, tokenSequence.position());

        // Return the parsed binary expression.
        auto binaryParser = new DExpression{
            {{firstExpression, secondExpression}},
            this->_operatorType,
            position,
            secondExpression->endPos
        };
        return std::shared_ptr<DExpression>(binaryParser);
    } else {
        throw std::runtime_error("Expected the operator: " + this->_operatorType);
    }
}