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
            VariableStack();
            /**
             * Push multiple variables to the stack.
             */
            void push(std::vector<std::string> variables);
            /**
             * The starting memory location of the variable with the given name.
             */
            int location(std::string variable);
            /**
             * The negation of a variable's ending memory location. 
             */
            int negativeEndLocation(std::string variable);
            /**
             * The starting address of the next memory location reserved for the next variable.
             */
            int nextLocation();
            /**
             * Amount of bytes that the variable stack uses in total.
             */
            int size();
            /**
             * Map storing the location values of the stored variables.
             */
            std::map<std::string, int>& locations();
        private:
            std::map<std::string, int> _locations;
            int _currentLocation = 0;
    };
};

#endif