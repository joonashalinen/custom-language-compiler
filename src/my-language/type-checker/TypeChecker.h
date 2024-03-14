#ifndef MY_LANGUAGE_TYPE_CHECKER_HH
#define MY_LANGUAGE_TYPE_CHECKER_HH

#include "../../components/data_structures/FoldableNode.h"
#include "../../components/data_structures/BatchStack.h"
#include "../../components/data_structures/LinkedMap.h"
#include "../../components/parsing/Expression.h"
#include <map>
#include <algorithm>
#include <set>
#include <stack>

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

            struct DFunctionType {
                std::vector<std::string> parameterTypes {};
                std::string returnType {};
            };

            /**
             * Type of the context object being passed around when type checking 
             *  the abstract syntax tree by folding over it.
             */
            struct DTypeCheckContext {
                std::map<std::string, TTypeChecker>* preTypeCheckers {nullptr};
                std::map<std::string, TTypeChecker>* postTypeCheckers {nullptr};
                DataStructures::BatchStack<std::string> typeStack {};
                DataStructures::LinkedMap<std::string> typeSymbolTable {};
                DataStructures::LinkedMap<DFunctionType> functionTypeSymbolTable {};
                DataStructures::BatchStack<std::string> loopBreakTypeStack {};
                DataStructures::BatchStack<DFunctionType> functionTypeStack {};
            };

            TypeChecker();
            /**
             * Perform type checking on the given parse tree.
             */
            void check(TExpression root);
        private:
            std::map<std::string, TTypeChecker> _preTypeCheckers;
            std::map<std::string, TTypeChecker> _postTypeCheckers;
    };
};

#endif