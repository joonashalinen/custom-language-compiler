#include "ModuleParser.h"

MyLanguage::ModuleParser::ModuleParser(
    IParseable* statementParser,
    IParseable* identifierParser
    )
{
    // First, we construct the parsers.

    this->_moduleStatementParser = std::unique_ptr<MapParser>(new MapParser{});

    this->_moduleParser = std::unique_ptr<Parsing::ChainParser>(
        new Parsing::ChainParser{
            "module", 
            ";", 
            *(this->_moduleStatementParser),
            [](std::vector<DToken>& tokens, int position) {
                if (position > 0) {
                    return tokens.at(position - 1).value == "}";
                } else {
                    return false;
                }
            }
        }
    );

    this->_functionParser = std::unique_ptr<MyLanguage::FunctionParser>(
        new MyLanguage::FunctionParser{
            identifierParser,
            statementParser
        }
    );

    // Next, we set the parsing rules.

    this->_moduleStatementParser->setParsers(std::map<std::string, IParseable*>{
        {"function-keyword", this->_functionParser.get()}
    });
    this->_moduleStatementParser->setWildCardParser(statementParser);

}

std::shared_ptr<Expression> MyLanguage::ModuleParser::parse(std::vector<DToken>& tokens, int position)
{
    return this->_moduleParser->parse(tokens, position);
}