#include "IRCommandFactory.h"

namespace MyLanguage {
    IRCommandFactory::IRCommandFactory()
    {
    }
    
    IRCommand IRCommandFactory::createLoadIntConst(std::string value, std::string variable)
    {
        return IRCommand{
            "LoadIntConst",
            TParams{{value}, {variable}}
        };
    }

    /* IRCommand IRCommandFactory::createCall(
        std::string operation, 
        std::vector<std::string> argumentVars, 
        std::string outputVar
    ) {
        return IRCommand{
            "LoadIntConst",
            TParams{{3}, {this->nextVariable()}}
        };
    } */

    std::string IRCommandFactory::nextVariable()
    {
        auto nextVariable = "x" + std::to_string(this->_variableId);
        this->_variableId = this->_variableId + 1;
        return nextVariable;
    }
}