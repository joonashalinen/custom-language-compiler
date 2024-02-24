#ifndef MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_FACTORY_HH
#define MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_FACTORY_HH

#include "TIRCommand.h"
#include <string>
#include <algorithm>

namespace MyLanguage {
    class IRCommandFactory {
        public:
            IRCommandFactory();
            std::shared_ptr<Expression> createExpression(std::string type, std::string attribute, std::string value);
            std::shared_ptr<Expression> createVariableList(std::vector<std::string> variables);
            TIRCommand createLoadIntConst(std::string value, std::string variable);
            TIRCommand createCall(std::string functionName, std::vector<std::string> argumentVars, std::string outputVar);
            TIRCommand createCopy(std::string fromVar, std::string toVar);
            std::string nextVariable();
        private:
            int _variableId = 1;
    };
};

#endif