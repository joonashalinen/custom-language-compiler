#include "ListParser.h"

Parsing::ListParser::ListParser(
    std::string type,
    std::string beginCharacter, 
    std::string endCharacter, 
    std::string separator,
    IParseable& elementParser
): _elementParser(elementParser) {
    
    this->_chainParser = std::unique_ptr<Parsing::ChainParser>(
        new Parsing::ChainParser{
            type,
            separator,
            elementParser,
            [endCharacter](std::vector<DToken>& tokens, int position) {
                return tokens.at(position).value == endCharacter;
            }
        }
    );

    this->_parentheticalParser = std::unique_ptr<Parsing::ParentheticalParser>(
        new Parsing::ParentheticalParser{"___PARENTHETICAL___", beginCharacter, endCharacter, *(this->_chainParser)}
    );
}

std::shared_ptr<Expression> Parsing::ListParser::parse(std::vector<DToken>& tokens, int position)
{
	auto expression = this->_parentheticalParser->parse(tokens, position)->children().at(0);
    // Skip past the end character.
    expression->setEndPos(expression->endPos() + 1);
    return expression;
}
