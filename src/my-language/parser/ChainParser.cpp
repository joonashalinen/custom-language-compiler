#include "ChainParser.h"

MyLanguage::ChainParser::ChainParser(OperatedChainParser* operatedChainParser)
{
    // First, we construct the sub-parsers.

    this->_mapParser = std::unique_ptr<MapParser>(new MapParser{});
    this->_variableDeclarationParser = std::unique_ptr<VariableDeclarationParser>(
        new VariableDeclarationParser{operatedChainParser}
    );

    // Next, we set the parsing rules.

    this->_mapParser->setParsers(std::map<std::string, IParseable*>{
        {"var", this->_variableDeclarationParser.get()}
    });
    this->_mapParser->setWildCardParser(operatedChainParser);

    // Finally, we construct the final chain parser.

    this->_chainParser = std::unique_ptr<Parsing::ChainParser>(
        new Parsing::ChainParser{
            "chain", 
            ";", 
            *(this->_mapParser),
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

MapParser* MyLanguage::ChainParser::mapParser() {
    return this->_mapParser.get();
}