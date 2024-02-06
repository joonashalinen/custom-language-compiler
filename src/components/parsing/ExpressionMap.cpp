#include "ExpressionMap.h"

ExpressionMap::ExpressionMap(
            std::vector<DToken>& tokens
    ): 
    _expressionConstructors(std::map<std::string, TExpressions::ExpressionConstructor>()),
    _tokens(tokens),
    _tokenSequence(TokenSequence{tokens}) {
    
}

std::shared_ptr<DExpression> ExpressionMap::parse(int position) {
    this->_tokenSequence.setPosition(position);
    auto token = this->_tokenSequence.peek();

    if (this->_expressionConstructors.contains(token.type)) {
        return this->parseWith(token.type, position);
    } else {
        throw std::runtime_error("Unexpected token '" + token.type + "' found at position " + std::to_string(position));
    }
}

std::shared_ptr<DExpression> ExpressionMap::parseWith(std::string tokenType, int position) {
    auto constructor = this->_expressionConstructors.at(tokenType);
    auto expression = constructor(this->_tokens);
    return (*expression).parse(position);
}

std::map<std::string, TExpressions::ExpressionConstructor>& ExpressionMap::expressionConstructors() {
    return this->_expressionConstructors;
}