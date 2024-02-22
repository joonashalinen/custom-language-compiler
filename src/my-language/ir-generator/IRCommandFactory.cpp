#include "IRCommandFactory.h"

namespace MyLanguage {
    IRCommandFactory::IRCommandFactory()
    {
    }

    std::shared_ptr<Expression> IRCommandFactory::createExpression(std::string type, std::string attribute, std::string value)
    {
        auto expression = std::shared_ptr<Expression>{new Expression{type, -1, -1}};
        expression->subTypes().insert({attribute, value});
        return expression;
    }

    TIRCommand IRCommandFactory::createVariableList(std::vector<std::string> variables)
    {
        auto root = std::shared_ptr<Expression>{new Expression{"variable-list", -1, -1}};
        std::for_each(variables.begin(), variables.end(), [root, this](std::string variable) {
            auto child = this->createExpression("variable", "name", variable);
            root->children().insert(root->children().end(), child);
        });
        return root;
    }
    
    TIRCommand IRCommandFactory::createLoadIntConst(std::string value, std::string variable)
    {
        TIRCommand command = this->createExpression("irCommand", "command", "LoadIntConst");
        auto valueExpression = this->createExpression("number", "value", value);
        auto variableExpression = this->createExpression("variable", "name", variable);
        
        Expression::addChild(command, valueExpression);
        Expression::addChild(command, variableExpression);
        
        return command;
    }

    TIRCommand IRCommandFactory::createCall(
        std::string functionName, 
        std::vector<std::string> argumentVars, 
        std::string outputVar
    ) {
        auto command = this->createExpression("irCommand", "command", "Call");
        auto functionNameExpression = this->createExpression("function", "name", functionName);
        auto argumentVarsExpression = this->createVariableList(argumentVars);
        auto outputVarExpression = this->createExpression("variable", "name", outputVar);
        
        Expression::addChild(command, functionNameExpression);
        Expression::addChild(command, argumentVarsExpression);
        Expression::addChild(command, outputVarExpression);

        return command;
    }

    std::string IRCommandFactory::nextVariable()
    {
        auto nextVariable = "x" + std::to_string(this->_variableId);
        this->_variableId = this->_variableId + 1;
        return nextVariable;
    }
}