#ifndef MY_LANGUAGE_TYPE_CHECKER_TYPE_HH
#define MY_LANGUAGE_TYPE_CHECKER_TYPE_HH

#include <string>
#include "IType.h"

namespace MyLanguage {
    /**
     * A type of an expression.
     */
    class Type: public IType {
        public:
            Type(std::string name);
            std::string toString();
        private:
            std::string _name;
    };
};

#endif