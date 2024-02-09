#include "FunctionCallParser.h"

MyLanguage::FunctionCallParser::FunctionCallParser(IParseable* identifierParser, MapParser& parameterParser):
    _identifierParser(identifierParser), _parameterParser(parameterParser)
{
    this->_parameterListParser = std::unique_ptr<Parsing::ListParser>(
        new Parsing::ListParser{
            "parameter-list",
            "(",
            ")",
            ",",
            parameterParser
        }
    );

    this->_parser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "function-call",
            std::vector<std::pair<std::string, std::string>>{
                {"expression", "ID"},
                {"expression", "L"}
            },
            std::map<std::string, IParseable*>{
                {"ID", identifierParser},
                {"L", this->_parameterListParser.get()}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::FunctionCallParser::parse(std::vector<DToken>& tokens, int position)
{
	return this->_parser->parse(tokens, position);
}
