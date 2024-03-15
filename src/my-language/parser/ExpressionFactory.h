#ifndef MY_LANGUAGE_EXPRESSION_FACTORY_HH
#define MY_LANGUAGE_EXPRESSION_FACTORY_HH

#include "../../components/parsing/Expression.h"
#include <memory>
#include <algorithm>
#include <iostream>

namespace MyLanguage {
    /**
     * A factory for manually creating instances of different abstract syntax tree 
     * nodes for expressions of our language.
     */
    class ExpressionFactory {
        public:
            ExpressionFactory();
            /**
             * Create a new function parameter expression.
             */
            std::shared_ptr<Expression> createFunctionParameter(
                std::string name,
                std::string type
            );
            /**
             * Create a new function expression.
             */
            std::shared_ptr<Expression> createFunction(
                std::string name,
                std::string returnType,
                std::vector<std::shared_ptr<Expression>> parameterExpressions,
                std::vector<std::shared_ptr<Expression>> definitionExpressions
            );
        private:
            
    };
};

#endif