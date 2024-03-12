#include "TypeChecker.h"

namespace MyLanguage {
    namespace TypeCheckers {
        
        /**
         * Type checking code for number literals.
         */
        TypeChecker::DTypeCheckContext* checkBoolean(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeStack.stack().push("Bool");
            return context;
        }
        
        /**
         * Type checking code for number literals.
         */
        TypeChecker::DTypeCheckContext* checkNumber(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeStack.stack().push("Int");
            return context;
        }

        /**
         * Top-level type checker for any expression type.
         */
        TypeChecker::DTypeCheckContext* checkAny(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->typeCheckers->contains(expression->type())) {
                // Choose the corresponding type checker function.
                auto generate = context->typeCheckers->at(expression->type());
                return generate(context, expression);
            } else {
                return context;
            }
        }
    }
    
    TypeChecker::TypeChecker()
    {
    }

    void TypeChecker::check(TExpression root)
    {
        auto context = TypeChecker::DTypeCheckContext{&(this->_typeCheckers)};
        auto foldable = DataStructures::FoldableNode<TypeChecker::DTypeCheckContext*, TypeChecker::TExpression>{root};
        foldable.fold(TypeCheckers::checkAny, &context);
    }
}

