#include "FunctionCallParser.h"

MyLanguage::FunctionCallParser::FunctionCallParser(IParseable* identifierParser, IParseable& parameterParser):
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
	auto functionExpression = this->_parser->parse(tokens, position);

    // Switch the function name to be just a sub-type of the function expression 
    // instead of a proper child identifier expression.
    auto identifier = functionExpression->children().at(0);
    Expression::removeChild(functionExpression, identifier);
    functionExpression->subTypes().insert({"name", identifier->rootToken().value});

    // Next, we lift the individual argument expressions to become direct children of the function expression.
    
    auto argumentList = functionExpression->children().at(0);
    // Copy the children of the argument list expression.
    std::vector<std::shared_ptr<Expression>> arguments = argumentList->children();
    // Change each child of the argument list expression to be a child of the function expression.
    std::for_each(
        arguments.begin(), 
        arguments.end(), 
        [&argumentList, &functionExpression](std::shared_ptr<Expression> argument) {
            Expression::removeChild(argumentList, argument);
            Expression::addChild(functionExpression, argument);
        }
    );
    // Make the root argument list expression no longer be a child of the function expression.
    // Now the function expression should only have argument expressions as its direct children.
    Expression::removeChild(functionExpression, argumentList);
    functionExpression->setEndPos(argumentList->endPos());

    return functionExpression;
}
