#include "VariableDeclarationParser.h"

MyLanguage::VariableDeclarationParser::VariableDeclarationParser(
    OperatedChainParser* operatedChainParser
) {
    this->_skeletonParser = this->_createSkeletonParser(
        {
            {"token-value", "var"},
            {"token-type", "identifier"},
            {"token-value", "="},
            {"expression", "E"}
        },
        operatedChainParser
    );
    this->_typedSkeletonParser = this->_createSkeletonParser(
        {
            {"token-value", "var"},
            {"token-type", "identifier"},
            {"token-value", ":"},
            {"token-type", "identifier"},
            {"token-value", "="},
            {"expression", "E"}
        },
        operatedChainParser
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
        expression->subTypes().insert({"value-type", expression->tokens().at(3).value});
        Expression::removeChild(expression, expression->children().at(1));
    }
    // Set the proper subtype for the name of the declared variable.
    expression->subTypes().insert({"name", expression->tokens().at(1).value});
    // Remove the identifier child expression.
    Expression::removeChild(expression, expression->children().at(0));
    return expression;
}

std::shared_ptr<Parsing::SkeletonParser> MyLanguage::VariableDeclarationParser::_createSkeletonParser(
    std::vector<std::pair<std::string, std::string>> pattern,
    OperatedChainParser* operatedChainParser
) {
    auto skeletonParser = std::shared_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "variable-declaration",
            pattern,
            std::map<std::string, IParseable*>{
                {"E", operatedChainParser}
            }
        }
    );
    return skeletonParser;
}