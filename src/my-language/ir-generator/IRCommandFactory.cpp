#include "IRCommandFactory.h"

namespace MyLanguage {
    IRCommandFactory::IRCommandFactory()
    {
    }
    
    TIRCommand IRCommandFactory::createLoadIntConst(std::string value, std::string variable)
    {
        auto command = TIRCommand{new Expression{"irCommand", -1, -1}};
        auto valueExpression = TIRCommand{new Expression{"number", -1, -1}};
        auto variableExpression = TIRCommand{new Expression{"identifier", -1, -1}};
        
        Expression::addChild(command, valueExpression);
        Expression::addChild(command, variableExpression);

        command->subTypes().insert({"command", "LoadIntConst"});
        valueExpression->subTypes().insert({"value", value});
        variableExpression->subTypes().insert({"name", variable});
        
        return command;
    }

    TIRCommand IRCommandFactory::createCall(
        std::string operation, 
        std::vector<std::string> argumentVars, 
        std::string outputVar
    ) {
        return TIRCommand{new Expression{"None", -1, -1}};
        /* return IRCommand{
            "Call",
            TParams{{operation}, {argumentVars}, {outputVar}}
        }; */
    }

    std::string IRCommandFactory::nextVariable()
    {
        auto nextVariable = "x" + std::to_string(this->_variableId);
        this->_variableId = this->_variableId + 1;
        return nextVariable;
    }
}