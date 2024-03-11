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
    auto moduleExpression =  this->_moduleParser->parse(tokens, position);
    auto moduleChildren = moduleExpression->children();

    // First, we check that there is at most one top-level expression that is not a function definition.
    
    auto isNotAFunction = [](auto child) {
        return child->type() != "function";
    };

    auto topLevelExpressions = std::vector<std::shared_ptr<Expression>>{};
    std::copy_if(moduleChildren.begin(), moduleChildren.end(), std::back_inserter(topLevelExpressions), isNotAFunction);

    // Next, we transform the top-level expressions into a function called 'main'. If no top-level expression 
    // is present, the 'main' function will have an empty definition.

    std::shared_ptr<Expression> mainFunction;
    if (topLevelExpressions.size() > 0) {
        mainFunction = (MyLanguage::ExpressionFactory{}).createFunction("main", "void", {}, topLevelExpressions);
        std::for_each(topLevelExpressions.begin(), topLevelExpressions.end(), [&moduleExpression](auto expression) {
            Expression::removeChild(moduleExpression, expression);
        });
    } else {
        mainFunction = (MyLanguage::ExpressionFactory{}).createFunction("main", "void", {}, {});
    }
    
    Expression::addChild(moduleExpression, mainFunction);

    return moduleExpression;
}