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

    TIRCommand IRCommandFactory::createLoadBoolConst(std::string value, std::string variable)
    {
        TIRCommand command = this->createExpression("irCommand", "command", "LoadBoolConst");
        auto valueExpression = this->createExpression("boolean", "value", value);
        auto variableExpression = this->createExpression("variable", "name", variable);
        
        Expression::addChild(command, valueExpression);
        Expression::addChild(command, variableExpression);
        
        return command;
    }

    TIRCommand IRCommandFactory::createLoadFunctionParam(int index, std::string variable)
    {
        TIRCommand command = this->createExpression("irCommand", "command", "LoadFunctionParam");
        auto variableExpression = this->createExpression("variable", "name", variable);
        command->subTypes().insert({"index", std::to_string(index)});
        Expression::addChild(command, variableExpression);
        return command;
    }

    TIRCommand IRCommandFactory::createWriteFunctionReturn(std::string fromVar)
    {
        TIRCommand command = this->createExpression("irCommand", "command", "WriteFunctionReturn");
        auto variableExpression = this->createExpression("variable", "name", fromVar);
        Expression::addChild(command, variableExpression);
        return command;
    }

    TIRCommand IRCommandFactory::createLoadFunctionReturn(std::string toVar)
    {
        TIRCommand command = this->createExpression("irCommand", "command", "LoadFunctionReturn");
        auto variableExpression = this->createExpression("variable", "name", toVar);
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

    TIRCommand IRCommandFactory::createCopy(
        std::string fromVar, 
        std::string toVar
    ) {
        auto command = this->createExpression("irCommand", "command", "Copy");
        auto fromVarExpression = this->createExpression("variable", "name", fromVar);
        auto toVarExpression = this->createExpression("variable", "name", toVar);
        
        Expression::addChild(command, fromVarExpression);
        Expression::addChild(command, toVarExpression);

        return command;
    }

    TIRCommand IRCommandFactory::createLabel(std::string label) {
        auto command = this->createExpression("irCommand", "command", "Label");
        auto labelExpression = this->createExpression("label", "name", label);
        
        Expression::addChild(command, labelExpression);

        return command;
    }

    TIRCommand IRCommandFactory::createFunctionLabel(std::string label) {
        auto command = this->createExpression("irCommand", "command", "FunctionLabel");
        auto labelExpression = this->createExpression("label", "name", label);
        
        Expression::addChild(command, labelExpression);

        return command;
    }

    TIRCommand IRCommandFactory::createNextLabel()
    {
        auto nextLabel = this->nextLabel();
        return this->createLabel(nextLabel);
    }

    TIRCommand IRCommandFactory::createCondJump(
        std::string conditionVar, 
        std::string onTrueLabel,
        std::string onFalseLabel
    ) {
        auto command = this->createExpression("irCommand", "command", "CondJump");
        auto conditionVarExpression = this->createExpression("variable", "name", conditionVar);
        auto onTrueLabelExpression = this->createExpression("label", "name", onTrueLabel);
        auto onFalseLabelExpression = this->createExpression("label", "name", onFalseLabel);
        
        Expression::addChild(command, conditionVarExpression);
        Expression::addChild(command, onTrueLabelExpression);
        Expression::addChild(command, onFalseLabelExpression);

        return command;
    }

    TIRCommand IRCommandFactory::createJump(std::string label) {
        auto command = this->createExpression("irCommand", "command", "Jump");
        auto labelExpression = this->createExpression("label", "name", label);
        
        Expression::addChild(command, labelExpression);

        return command;
    }

    std::string IRCommandFactory::nextVariable()
    {
        auto nextVariable = "x" + std::to_string(this->_variableId);
        this->_variableId = this->_variableId + 1;
        return nextVariable;
    }

    std::string IRCommandFactory::nextLabel()
    {
        auto nextLabel = "L" + std::to_string(this->_labelId);
        this->_labelId = this->_labelId + 1;
        return nextLabel;
    }
}