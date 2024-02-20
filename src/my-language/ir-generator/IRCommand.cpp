#include "IRCommand.h"
#include <iostream>

namespace MyLanguage {
    IRCommand::IRCommand(
        std::string type, 
        TParams params
    ): _type(type), _params(params) {
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
        // Form the substring of the list of parameter names within the parentheses.
        auto first = true;
        auto paramsString = std::accumulate(
            this->_params.begin(), 
            this->_params.end(), 
            std::string(""), 
            [&first](std::string paramsString, std::variant<TIRVariable, std::vector<TIRVariable>> param) {
                // Determine whether we need to prefix the next added substring with the separator string.
                auto prefix = first ? "" : ", ";
                first = false;
                // If the parameter is a single variable.
                if (param.index() == 0) {
                    return paramsString + std::string(prefix) + std::get<TIRVariable>(param);
                } else {
                    // The parameter is a list of variables.
                    auto paramsList = std::get<std::vector<TIRVariable>>(param);
                    // Join the variables together with a comma and a space in-between.
                    auto firstSubParam = true;
                    return paramsString + std::string(prefix) + "[" + std::accumulate(
                        paramsList.begin(), 
                        paramsList.end(), 
                        std::string(""),
                        [&firstSubParam](std::string paramsListString, TIRVariable param) {
                            auto newString = firstSubParam ? param : (paramsListString + ", " + param);
                            firstSubParam = false;
                            return newString;
                        }
                    ) + "]";
                }
            }
        );

        return (
            this->_type + "(" + paramsString + ")"
        );
    }
}
