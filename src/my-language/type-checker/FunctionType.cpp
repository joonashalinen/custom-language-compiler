#include "FunctionType.h"

namespace MyLanguage {
    FunctionType::FunctionType(std::vector<std::shared_ptr<IType>> parameterTypes, std::shared_ptr<IType> returnType):
        _parameterTypes(parameterTypes), _returnType(returnType) {
    }

    std::string FunctionType::toString()
    {
        std::string result = "(";
        
        bool first = true;
        result = std::accumulate(
            this->_parameterTypes.begin(), 
            this->_parameterTypes.end(),
            result, 
            [&first](auto result, auto type) {
                if (!first) {
                    result = result + ", ";
                }
                return result + type->toString();
                first = false;
        });

        return result + ")" + " => " + this->_returnType->toString();
    }

    std::vector<std::shared_ptr<IType>> FunctionType::parameterTypes()
    {
        return this->_parameterTypes;
    }

    std::shared_ptr<IType> FunctionType::returnType()
    {
        return this->_returnType;
    }
}