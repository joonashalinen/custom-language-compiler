#ifndef MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_HH
#define MY_LANGUAGE_IR_GENERATOR_IR_COMMAND_HH

#include <variant>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

namespace MyLanguage {
    using TIRVariable = std::string;
    using TParams = std::vector<std::variant<TIRVariable, std::vector<TIRVariable>>>;
    /**
     * A command in the IR language.
     */
    class IRCommand {
        public:
            IRCommand(std::string type, TParams params);
            std::string type();
            TParams params();
            /**
             * String representation of the IR command.
             */
            std::string toString();
        private:
            std::string _type;
            TParams _params;
    };
};

#endif