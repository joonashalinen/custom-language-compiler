#include "FunctionParser.h"

MyLanguage::FunctionParser::FunctionParser(
    IParseable* identifierParser, 
    IParseable* baseStatementParser,
    IParseable* typeParser
):  _identifierParser(identifierParser)
{
    this->_parameterParser = std::unique_ptr<MyLanguage::FunctionParameterParser>(
        new MyLanguage::FunctionParameterParser{typeParser}
    );

    this->_parameterListParser = std::unique_ptr<Parsing::ListParser>(
        new Parsing::ListParser{"function-parameter-list", "(", ")", ",", *(this->_parameterParser)}
    );

    this->_statementParser = std::unique_ptr<MapParser>(new MapParser{});

    this->_definitionParser = std::unique_ptr<Parsing::ChainParser>(
        new Parsing::ChainParser{
            "function-definition", 
            ";", 
            *(this->_statementParser),
            [](std::vector<DToken>& tokens, int position) {
                return false;
            }
        }
    );

    this->_parser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "function",
            std::vector<std::pair<std::string, std::string>>{
                {"token-value", "fun"},
                {"expression", "ID"},
                {"expression", "L"},
                {"optional", ""},
                {"token-value", ":"},
                {"expression", "T"},
                {"/optional", ""},
                {"token-value", "{"},
                {"expression", "D"},
                {"token-value", "}"}
            },
            std::map<std::string, IParseable*>{
                {"ID", identifierParser},
                {"L", this->_parameterListParser.get()},
                {"D", this->_definitionParser.get()},
                {"T", typeParser}
            }
        }
    );
    
    this->_returnParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "return",
            {
                {"token-value", "return"},
                {"trail", ""},
                {"expression", "E"}
            },
            {
                {"E", baseStatementParser}
            }
        }
    );

    // Set parsing rules for individual statements within the function definition.
    this->_statementParser->setParsers({
        {"return", this->_returnParser.get()}
    });
    this->_statementParser->setWildCardParser(baseStatementParser);
}

std::shared_ptr<Expression> MyLanguage::FunctionParser::parse(std::vector<DToken>& tokens, int position)
{
	auto functionExpression = this->_parser->parse(tokens, position);

    // Switch the function name to be just a sub-type of the function expression 
    // instead of a proper child identifier expression.
    auto identifier = functionExpression->children().at(0);
    Expression::removeChild(functionExpression, identifier);
    functionExpression->subTypes().insert({"name", identifier->rootToken().value});

    // If the function has a return type specified.
    if (functionExpression->children().at(1)->type() == "type") {
        // Switch the return type from an expression into a subtype.
        auto returnType = functionExpression->children().at(1);
        Expression::removeChild(functionExpression, returnType);
        functionExpression->subTypes().insert({"return-type", returnType->subTypes().at("literal-value")});
    } else {
        // The return type is Unit.
        functionExpression->subTypes().insert({"return-type", "Unit"});
    }

    // Determine whether the function contains a return statement.
    auto definitionChildren = functionExpression->children().at(1)->children();
    int returns = std::count_if(definitionChildren.begin(), definitionChildren.end(), [](auto statement) {
        return statement->type() == "return";
    });
    functionExpression->subTypes().insert({"returns-amount", std::to_string(returns)});

    return functionExpression;
}