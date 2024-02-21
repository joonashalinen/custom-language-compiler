#ifndef STRUCTURED_LANGUAGE_VARIABLE_STACK_HH
#define STRUCTURED_LANGUAGE_VARIABLE_STACK_HH

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

namespace StructuredLanguage {
    /**
     * A stack data structure that maintains variables in a memory stack.
     */
    class VariableStack {
        public:
            VariableStack(std::vector<std::string> variables);
            /**
             * The starting memory location of the variable with the given name.
             */
            int location(std::string variable);
            /**
             * The starting address of the next memory location reserved for the next variable.
             */
            int nextLocation();
        private:
            std::map<std::string, int> _locations;
            int _currentLocation = 0;
    };
};

#endif