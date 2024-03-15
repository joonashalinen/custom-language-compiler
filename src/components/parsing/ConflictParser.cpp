#include "ConflictParser.h"
#include <iostream>

Parsing::ConflictParser::ConflictParser(std::vector<IParseable*> parsers):
    _parsers(parsers) {
}

std::shared_ptr<Expression> Parsing::ConflictParser::parse(std::vector<DToken>& tokens, int position)
{
	auto parseResults = std::vector<std::shared_ptr<Expression>>(this->_parsers.size());
    std::transform(this->_parsers.begin(), this->_parsers.end(), parseResults.begin(), [&tokens, position](auto parser) {
        try {
            return parser->parse(tokens, position);
        } catch (...) {
            return std::shared_ptr<Expression>(new Expression{"none", -1, -1});
        }
    });

    auto longestExpression = *(std::max_element(
        parseResults.begin(), 
        parseResults.end(), 
        [](auto a, auto b) {
            return a->endPos() < b->endPos();
        }
    ));

    if (longestExpression->endPos() == -1) {
        throw std::runtime_error("No parsing rule could be matched at " + tokens.at(position).startLocation.toString());
    }

    return longestExpression;
}