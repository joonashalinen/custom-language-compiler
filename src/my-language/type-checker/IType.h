#ifndef MY_LANGUAGE_TYPE_CHECKER_ITYPE_HH
#define MY_LANGUAGE_TYPE_CHECKER_ITYPE_HH

#include <string>

namespace MyLanguage {
    /**
     * Interface for all expression types.
     */
    class IType {
        public:
            virtual std::string toString() = 0;
        private:
    };
};

#endif