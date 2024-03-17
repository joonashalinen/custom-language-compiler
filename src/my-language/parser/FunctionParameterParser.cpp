#include "FunctionParameterParser.h"

MyLanguage::FunctionParameterParser::FunctionParameterParser(IParseable* typeParser)
{
    this->_identifierParser = std::unique_ptr<LiteralParser>(new LiteralParser{"identifier"});
    this->_skeletonParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "function-parameter",
            {
                {"expression", "ID"},
                {"trail", ""},
                {"token-value", ":"},
                {"expression", "T"},
            },
            {
                {"ID", this->_identifierParser.get()},
                {"T", typeParser}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::FunctionParameterParser::parse(std::vector<DToken>& tokens, int position)
{
    auto expression = this->_skeletonParser->parse(tokens, position);

    // We want to change the parameter name and type into sub-types of the expression 
    // instead of proper child expressions.

    auto parameter = expression->children().at(0);
    auto parameterName = parameter->subTypes().at("name");
    expression->subTypes().insert({"name", parameterName});
    Expression::removeChild(expression, parameter);
    
    // If the function parameter has a type declaration.
    if (expression->children().size() > 0) {
        auto type = expression->children().at(0);
        auto typeName = type->subTypes().at("name");
        expression->subTypes().insert({"value-type", typeName});
        Expression::removeChild(expression, type);
    } else {
        expression->subTypes().insert({"value-type", "Any"});
    }

    assert(expression->children().size() == 0);

    return expression;
}
