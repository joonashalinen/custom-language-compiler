#include "MapParser.h"

MapParser::MapParser(
    ): 
    _expressionConstructors(std::map<std::string, TExpressions::ExpressionConstructor>()) {
    
}

std::shared_ptr<DExpression> MapParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto token = tokenSequence.peek();

    if (this->_expressionConstructors.contains(token.type)) {
        return this->parseWith(tokens, token.type, position);
    } else {
        throw std::runtime_error("Unexpected token '" + token.type + "' found at position " + std::to_string(position));
    }
}

std::shared_ptr<DExpression> MapParser::parseWith(std::vector<DToken>& tokens, std::string tokenType, int position) {
    auto constructor = this->_expressionConstructors.at(tokenType);
    auto expression = constructor();
    return (*expression).parse(tokens, position);
}

std::map<std::string, TExpressions::ExpressionConstructor>& MapParser::expressionConstructors() {
    return this->_expressionConstructors;
}