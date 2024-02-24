#ifndef MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_HH
#define MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_HH

#include <variant>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include "../../components/parsing/Expression.h"

namespace MyLanguage {
    using TIRVariable = std::string;
    using TIRCommand = std::shared_ptr<Expression>;
    using TParams = std::vector<std::variant<TIRVariable, std::vector<TIRVariable>>>;
    /**
     * A command in the IR language.
     */
    class IRCommand {
        public:
            IRCommand(TIRCommand expression);
            std::string type();
            TParams params();
            /**
             * String representation of the IR command.
             */
            std::string toString();
        private:
            std::string _type;
            TParams _params;
            TIRCommand _expression;
    };
};

#endif