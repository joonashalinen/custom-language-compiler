#include "VariableDeclarationParser.h"

MyLanguage::VariableDeclarationParser::VariableDeclarationParser(
    OperatedChainParser* operatedChainParser,
    IParseable* typeParser
) {
    this->_skeletonParser = this->_createSkeletonParser(
        {
            {"token-value", "var"},
            {"token-type", "identifier"},
            {"token-value", "="},
            {"expression", "E"}
        },
        operatedChainParser,
        typeParser
    );
    this->_typedSkeletonParser = this->_createSkeletonParser(
        {
            {"token-value", "var"},
            {"token-type", "identifier"},
            {"token-value", ":"},
            {"expression", "T"},
            {"token-value", "="},
            {"expression", "E"}
        },
        operatedChainParser,
        typeParser
    );
    this->_conflictParser = std::shared_ptr<Parsing::ConflictParser>{
        new Parsing::ConflictParser{
            {
                this->_skeletonParser.get(),
                this->_typedSkeletonParser.get()
            }
        }
    };
}

std::shared_ptr<Expression> MyLanguage::VariableDeclarationParser::parse(
    std::vector<DToken>& tokens, 
    int position
) {
    auto expression = this->_conflictParser->parse(tokens, position);
    // If the expression is a typed variable declaration.
    if (expression->children().size() == 3) {
        // Change the type child expression into a subtype of the variable declaration.
        expression->subTypes().insert({"value-type", expression->children().at(1)->subTypes().at("name")});
        Expression::removeChild(expression, expression->children().at(1));
    } else {
        // Set the value type of the variable to 'Any'.
        expression->subTypes().insert({"value-type", "Any"});
    }
    // Set the proper subtype for the name of the declared variable.
    expression->subTypes().insert({"name", expression->tokens().at(1).value});
    // Remove the identifier child expression.
    Expression::removeChild(expression, expression->children().at(0));
    return expression;
}

std::shared_ptr<Parsing::SkeletonParser> MyLanguage::VariableDeclarationParser::_createSkeletonParser(
    std::vector<std::pair<std::string, std::string>> pattern,
    OperatedChainParser* operatedChainParser,
    IParseable* typeParser
) {
    auto skeletonParser = std::shared_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "variable-declaration",
            pattern,
            std::map<std::string, IParseable*>{
                {"E", operatedChainParser},
                {"T", typeParser}
            }
        }
    );
    return skeletonParser;
}