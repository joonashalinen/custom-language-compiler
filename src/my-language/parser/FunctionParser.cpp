#include "FunctionParser.h"

MyLanguage::FunctionParser::FunctionParser(
    IParseable* identifierParser, 
    IParseable* parameterParser,
    IParseable* chainParser
):  _identifierParser(identifierParser), _parameterParser(parameterParser)
{
    this->_parameterListParser = std::unique_ptr<Parsing::ListParser>(
        new Parsing::ListParser{"parameter-list", "(", ")", ",", *parameterParser}
    );

    this->_parser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "function",
            std::vector<std::pair<std::string, std::string>>{
                {"token-value", "fun"},
                {"expression", "ID"},
                {"expression", "L"},
                {"token-value", "{"},
                {"expression", "C"},
                {"token-value", "}"}
            },
            std::map<std::string, IParseable*>{
                {"ID", identifierParser},
                {"L", this->_parameterListParser.get()},
                {"C", chainParser}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::FunctionParser::parse(std::vector<DToken>& tokens, int position)
{
	auto functionExpression = this->_parser->parse(tokens, position);

    // Switch the function name to be just a sub-type of the function expression 
    // instead of a proper child identifier expression.
    auto identifier = functionExpression->children().at(0);
    Expression::removeChild(functionExpression, identifier);
    functionExpression->subTypes().insert({"name", identifier->rootToken().value});

    return functionExpression;
}
