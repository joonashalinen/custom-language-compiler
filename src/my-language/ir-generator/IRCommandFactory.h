#ifndef MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_FACTORY_HH
#define MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_FACTORY_HH

#include "TIRCommand.h"
#include <string>
#include <algorithm>
#include <iostream>

namespace MyLanguage {
    class IRCommandFactory {
        public:
            IRCommandFactory();
            std::shared_ptr<Expression> createExpression(std::string type, std::string attribute, std::string value);
            std::shared_ptr<Expression> createVariableList(std::vector<std::string> variables);
            TIRCommand createLoadIntConst(std::string value, std::string variable);
            TIRCommand createLoadBoolConst(std::string value, std::string variable);
            TIRCommand createLoadFunctionParam(int index, std::string variable);
            TIRCommand createWriteFunctionReturn(std::string fromVar);
            TIRCommand createLoadFunctionReturn(std::string toVar);
            TIRCommand createCopy(std::string fromVar, std::string toVar);
            TIRCommand createCall(std::string functionName, std::vector<std::string> argumentVars, std::string outputVar);
            TIRCommand createCondJump(std::string conditionVar, std::string onTrueLabel, std::string onFalseLabel);
            TIRCommand createJump(std::string label);
            TIRCommand createLabel(std::string label);
            TIRCommand createFunctionLabel(std::string label);
            TIRCommand createNextLabel();
            std::string nextVariable();
            std::string nextLabel();
        private:
            int _variableId = 1;
            int _labelId = 1;
    };
};

#endif