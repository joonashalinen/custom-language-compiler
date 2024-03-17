#ifndef MY_LANGUAGE_TYPE_CHECKER_FUNCTION_TYPE_HH
#define MY_LANGUAGE_TYPE_CHECKER_FUNCTION_TYPE_HH

#include <vector>
#include <string>
#include <numeric>
#include <memory>
#include "IType.h"

namespace MyLanguage {
    /**
     * A type of a function.
     */
    class FunctionType: public IType {
        public:
            FunctionType(std::vector<std::shared_ptr<IType>> parameterTypes, std::shared_ptr<IType> returnType);
            std::string toString();
            std::vector<std::shared_ptr<IType>>& parameterTypes();
            std::shared_ptr<IType> returnType();
        private:
            std::vector<std::shared_ptr<IType>> _parameterTypes;
            std::shared_ptr<IType> _returnType;
    };
};

#endif