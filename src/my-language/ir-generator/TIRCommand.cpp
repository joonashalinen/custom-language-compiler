#include "TIRCommand.h"
#include <iostream>

namespace MyLanguage {
    IRCommand::IRCommand(TIRCommand expression): _expression(expression) {
    }

    std::string IRCommand::type()
    {
        return this->_type;
    }

    TParams IRCommand::params()
    {
        return this->_params;
    }

    std::string IRCommand::toString()
    {
        return "";
    }
}
