#ifndef MY_LANGUAGE_TYPE_CHECKER_HH
#define MY_LANGUAGE_TYPE_CHECKER_HH

#include "../../components/data_structures/FoldableNode.h"
#include "../../components/data_structures/BatchStack.h"
#include "../../components/data_structures/LinkedMap.h"
#include "../../components/parsing/Expression.h"
#include <map>
#include <algorithm>

namespace MyLanguage {
    /**
     * Can be used to perform type checking on a parse tree for our custom 
     * programming language.
     */
    class TypeChecker {
        public:
            using TExpression = std::shared_ptr<Expression>;
            struct DTypeCheckContext;

            using TTypeChecker = std::function<
                DTypeCheckContext*(
                    DTypeCheckContext*,
                    TExpression
                )
            >;

            /**
             * Type of the context object being passed around when type checking 
             *  the abstract syntax tree by folding over it.
             */
            struct DTypeCheckContext {
                std::map<std::string, TTypeChecker>* typeCheckers {nullptr};
                DataStructures::BatchStack<std::string> typeStack {};
                DataStructures::LinkedMap<std::string> symbolTable {};
            };

            TypeChecker();
            /**
             * Perform type checking on the given parse tree.
             */
            void check(TExpression root);
        private:
            std::map<std::string, TTypeChecker> _typeCheckers;
    };
};

#endif