#include "ChainParser.h"

MyLanguage::ChainParser::ChainParser(MapParser& mapParser)
{
    this->_chainParser = std::unique_ptr<Parsing::ChainParser>(
        new Parsing::ChainParser{
            "chain", 
            ";", 
            mapParser,
            [](std::vector<DToken>& tokens, int position) {
                if (position > 0 && position < (int) (tokens.size())) {
                    return (
                        tokens.at(position).value == "}" || 
                        tokens.at(position - 1).value == "}"
                    );
                } else if (position < (int) (tokens.size())) {
                    return tokens.at(position).value == "}";
                } else if (position > 0) {
                    return tokens.at(position - 1).value == "}";
                } else {
                    return false;
                }
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::ChainParser::parse(std::vector<DToken>& tokens, int position)
{
    return this->_chainParser->parse(tokens, position);
}