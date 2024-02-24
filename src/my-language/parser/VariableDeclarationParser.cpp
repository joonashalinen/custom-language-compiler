#include "VariableDeclarationParser.h"

MyLanguage::VariableDeclarationParser::VariableDeclarationParser(
    OperatedChainParser* operatedChainParser
) {
    this->_skeletonParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "variable-declaration",
            std::vector<std::pair<std::string, std::string>>{
                {"token-value", "var"},
                {"token-type", "identifier"},
                {"token-value", "="},
                {"expression", "E"}
            },
            std::map<std::string, IParseable*>{
                {"E", operatedChainParser}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::VariableDeclarationParser::parse(
    std::vector<DToken>& tokens, 
    int position
) {
    auto expression = this->_skeletonParser->parse(tokens, position);
    // Set the proper subtype for the name of the declared variable.
    expression->subTypes().insert({"name", expression->tokens().at(1).value});
    // Remove the identifier child expression.
    Expression::removeChild(expression, expression->children().at(0));
    return expression;
}