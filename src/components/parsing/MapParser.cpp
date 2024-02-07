#include "MapParser.h"

MapParser::MapParser() {
    
}

std::shared_ptr<Expression> MapParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);
    auto token = tokenSequence.peek();

    if (this->_parsers.contains(token.type)) {
        return this->parseWith(tokens, token.type, position);
    } else if (this->_parsers.contains(token.value)) {
        return this->parseWith(tokens, token.value, position);
    } else {
        throw std::runtime_error("Unexpected token '" + token.type + "' found at position " + std::to_string(position));
    }
}

std::shared_ptr<Expression> MapParser::parseWith(std::vector<DToken>& tokens, std::string tokenType, int position) {
    auto parser = this->_parsers.at(tokenType);
    return parser->parse(tokens, position);
}

std::map<std::string, IParseable*>& MapParser::parsers() {
    return this->_parsers;
}

void MapParser::setParsers(std::map<std::string, IParseable*> parsers) {
    this->_parsers = parsers;
}