#include "MapParser.h"
#include <iostream>

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
    } else if (this->_wildCardParser != nullptr) {
        // Parse using a wildcard parsing rule.
        return this->_wildCardParser->parse(tokens, position);
    } else {
        throw std::runtime_error(
            "Unexpected token '" + token.value + "' found at " +
            token.startLocation.toString()
        );
    }
}

std::shared_ptr<Expression> MapParser::parseWith(std::vector<DToken>& tokens, std::string rule, int position) {
    auto parser = this->_parsers.at(rule);
    return parser->parse(tokens, position);
}

bool MapParser::canParseAt(std::vector<DToken>& tokens, int position)
{
    auto sequence = TokenSequence{tokens};
    sequence.setPosition(position);
    auto token = sequence.peek();

    if (this->_parsers.contains(token.type) || this->_parsers.contains(token.value)) {
        return true;
    } else if (this->_wildCardParser != nullptr) {
        return this->_wildCardParser->canParseAt(tokens, position);
    } else {
        return false;
    }
}

std::map<std::string, IParseable*>& MapParser::parsers() {
    return this->_parsers;
}

void MapParser::setParsers(std::map<std::string, IParseable*> parsers) {
    this->_parsers = parsers;
}

void MapParser::setWildCardParser(IParseable* wildCardParser)
{
    this->_wildCardParser = wildCardParser;
}