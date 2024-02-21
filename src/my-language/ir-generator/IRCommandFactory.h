#ifndef MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_FACTORY_HH
#define MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_FACTORY_HH

#include "TIRCommand.h"
#include <string>

namespace MyLanguage {
    class IRCommandFactory {
        public:
            IRCommandFactory();
            TIRCommand createLoadIntConst(std::string value, std::string variable);
            TIRCommand createCall(std::string operation, std::vector<std::string> argumentVars, std::string outputVar);
            std::string nextVariable();
        private:
            int _variableId = 1;
    };
};

#endif