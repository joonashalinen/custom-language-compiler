#include "TypeChecker.h"

namespace MyLanguage {
    namespace TypeCheckers {
        /**
         * Throws a type error that was encountered during type checking.
         */
        void throwTypeError(
            std::shared_ptr<Expression> expression,
            std::string message
        ) {
            throw std::runtime_error(
                std::string("Error encountered at position ") + 
                std::to_string(expression->startPos()) + ": " + message + "."
            );
        }

        /**
         * Type checking code for binary operators other than '='.
         */
        TypeChecker::DTypeCheckContext* postCheckNormalBinaryOperator(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto valueTypes = context->typeStack.pop(2);
            auto operatorName = expression->subTypes().at("name");

            if (valueTypes.at(0) != valueTypes.at(1)) {
                auto errorMessage = (
                    std::string("The left value to binary operator '") + operatorName + 
                    "' was of type '" + valueTypes.at(0) +
                    "' but the right value was of type '" + valueTypes.at(1) + "'"
                );
                throwTypeError(expression, errorMessage);

            } else {
                auto acceptedTypes = std::map<std::string, std::set<std::string>>{
                    {"and", {"Bool"}},
                    {"or", {"Bool"}},
                    {"<", {"Int"}},
                    {">", {"Int"}},
                    {"<=", {"Int"}},
                    {">=", {"Int"}},
                    {"==", {"Int", "Bool"}},
                    {"!=", {"Int", "Bool"}}
                };
                if (!(acceptedTypes.contains(operatorName))) {
                    throwTypeError(expression, std::string("'") + operatorName + "' is not a recognized binary operator");
                }
                auto acceptedTypesForOperator = acceptedTypes.at(operatorName);
                if (!(acceptedTypesForOperator.contains(valueTypes.at(0)))) {
                    throwTypeError(
                        expression,
                        std::string("The left-hand type '") + valueTypes.at(0) + "' to binary operator '" + 
                        operatorName + "' is an invalid type"
                    );
                } else if (!(acceptedTypesForOperator.contains(valueTypes.at(1)))) {
                    throwTypeError(
                        expression,
                        std::string("The right-hand type '") + valueTypes.at(1) + "' to binary operator '" + 
                        operatorName + "' is an invalid type"
                    );
                }
                context->typeStack.stack().push(valueTypes.at(0));
            }
            return context;
        }

        /**
         * Type checking code for variable declarations.
         */
        TypeChecker::DTypeCheckContext* postCheckVariableDeclaration(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto variableName = expression->subTypes().at("name");
            auto variableType = expression->subTypes().at("value-type");
            auto rightHandType = context->typeStack.pop();
            // If the variable already exists in this scope.
            if (context->typeSymbolTable.front().contains(variableName)) {
                throwTypeError(
                    expression, 
                    std::string("The variable '") + variableName + 
                    "' has already been declared in this scope."
                );
            }
            if (variableType != "Any" && variableType != rightHandType) {
                throwTypeError(
                    expression,
                    std::string("Assigning a value of type '") + rightHandType + 
                    "' to a variable of type '" + variableType + "'"
                );
            }
            context->typeStack.stack().push("Unit");
            context->typeSymbolTable.insert(variableName, rightHandType);
            return context;
        }

        /**
         * Type checking code for variable assignments.
         */
        TypeChecker::DTypeCheckContext* postCheckAssignment(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Name of the variable being assigned to.
            auto variableName = expression->children().at(0)->subTypes().at("literal-value");
            // If the variable does not exist.
            if (!(context->typeSymbolTable.contains(variableName))) {
                throwTypeError(
                    expression, 
                    std::string("Trying assign to a variable '") + variableName + 
                    "' that has not been defined"
                );
            }

            auto variableType = context->typeSymbolTable.at(variableName);
            // Type of the value being assigned.
            auto valueType = context->typeStack.pop();
            // If the types do not match.
            if (variableType != valueType) {
                auto errorMessage = (
                    std::string("The type of the variable '") + variableName + 
                    "' being assigned to was of type '" + variableType +
                    "' but the value being assigned was of type '" + valueType + "'"
                );
                throwTypeError(expression, errorMessage);
            }
            return context;
        }

        /**
         * Type checking code for binary operations.
         */
        TypeChecker::DTypeCheckContext* postCheckBinaryOperator(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (expression->subTypes().at("name") == "=") {
                return postCheckAssignment(context, expression);
            } else {
                return postCheckNormalBinaryOperator(context, expression);
            }
        }

        /**
         * Type checking code for unary operations.
         */
        TypeChecker::DTypeCheckContext* postCheckUnaryOperator(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto valueType = context->typeStack.stack().top();
            auto operatorName = expression->subTypes().at("name");
            auto acceptedTypes = std::map<std::string, std::string>{
                {"not", "Bool"},
                {"minus", "Int"}
            };
            if (!(acceptedTypes.contains(operatorName))) {
                throwTypeError(expression, std::string("'") + operatorName + "' is not a recognized unary operator");
            } else if (acceptedTypes.at(operatorName) != valueType) {
                throwTypeError(
                    expression, 
                    std::string("The type '") + valueType + 
                    "' is not a valid operand type for the unary operator '" + operatorName + "'"
                );
            }
            return context;
        }

        /**
         * Type checking code for chain expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckChain(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (expression->children().size() > 0) {
                auto childTypes = context->typeStack.pop(expression->children().size());
                std::string lastType = childTypes.back();
                if (expression->subTypes().at("openness") == "open") {
                    context->typeStack.push({lastType});
                } else {
                    context->typeStack.push({"Unit"});
                }
            }
            return context;
        }

        /**
         * Type checking code for identifier expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckIdentifier(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto variableName = expression->subTypes().at("name");
            if (!(context->typeSymbolTable.contains(variableName))) {
                throwTypeError(
                    expression, 
                    std::string("Trying to reference a variable '") + variableName + 
                    "' that has not been defined"
                );
            }
            auto variableType = context->typeSymbolTable.at(variableName);
            context->typeStack.stack().push(variableType);
            return context;
        }

        /**
         * Type checking code for boolean literals.
         */
        TypeChecker::DTypeCheckContext* postCheckBoolean(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeStack.stack().push("Bool");
            return context;
        }
        
        /**
         * Type checking code for number literals.
         */
        TypeChecker::DTypeCheckContext* postCheckNumber(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeStack.stack().push("Int");
            return context;
        }

        /**
         * Pre-order type checking code for block expressions.
         */
        TypeChecker::DTypeCheckContext* preCheckBlock(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeSymbolTable.pushFront();
            return context;
        }

        /**
         * Post-order type checking code for block expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckBlock(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeSymbolTable.popFront();
            return context;
        }

        /**
         * Post-order type checking code for if-then and if-then-else expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckIf(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Type of the then-portion's expression.
            auto conditionType = context->typeStack.pop(expression->children().size()).at(0);
            if (conditionType != "Bool") {
                throwTypeError(
                    expression, 
                    std::string("Expected the condition of an if-statement to be of type 'Bool' but instead it was of type '") + 
                    conditionType + "'"
                );
            }
            context->typeStack.stack().push("Unit");
            return context;
        }

        /**
         * Pre-order type checking code for while-expressions.
         */
        TypeChecker::DTypeCheckContext* preCheckWhile(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->loopBreakTypeStack.stack().push("Any");
            return context;
        }

        /**
         * Post-order type checking code for while-expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckWhile(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto returnType = context->loopBreakTypeStack.pop();
            context->typeStack.stack().push(returnType == "Any" ? "Unit" : returnType);
            return context;
        }

        /**
         * Post-order type checking code for break-expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckBreak(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto breakResultType = expression->children().size() == 0 ? "Unit" : context->typeStack.pop();
            std::string& expectedResultType = context->loopBreakTypeStack.stack().top();
            if (expectedResultType != "Any" && breakResultType != expectedResultType) {
                throwTypeError(expression, "Breaks in loop have mismatching return types.");
            } else {
                expectedResultType = breakResultType;
            }
            return context;
        }

        /**
         * Type checking code for function calls.
         */
        TypeChecker::DTypeCheckContext* postCheckFunctionCall(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto functionName = expression->subTypes().at("name");
            if (!(context->functionTypeSymbolTable.contains(functionName))) {
                throwTypeError(expression, "No function with name '" + functionName + "' found");
            } else {
                auto functionType = context->functionTypeSymbolTable.at(functionName);
                auto parameterTypes = context->typeStack.pop(expression->children().size());
                if (functionType.parameterTypes.size() != parameterTypes.size()) {
                    throwTypeError(expression, "Mismatching amount of arguments given to function call.");
                } else {
                    auto parameterTypesMatch = (std::accumulate(
                        parameterTypes.begin(),
                        parameterTypes.end(),
                        std::pair<int, bool>{0, true}, // Iteration index and the accumulated boolean.
                        [&functionType](auto acc, auto parameterType) {
                            return std::pair<int, bool>{
                                acc.first + 1,
                                acc.second && (parameterType == functionType.parameterTypes.at(acc.first))
                            };
                        }
                    )).second;
                    
                    if (!parameterTypesMatch) {
                        throwTypeError(
                            expression, 
                            "Types of given arguments do not match the expected parameters in function call."
                        );
                    } else {
                        context->typeStack.stack().push(functionType.returnType);
                    }
                }
            }
            return context;
        }

        /**
         * Post-order type checking code for return-expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckReturn(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto expectedReturnType = context->functionTypeStack.stack().top().returnType;
            auto returnType = context->typeStack.pop();
            if (expectedReturnType != "Any" && returnType != expectedReturnType) {
                throwTypeError(expression, "Type of returned value does not match the expected return type");
            }
            return context;
        }

        /**
         * Pre-order type checking code for function expressions.
         */
        TypeChecker::DTypeCheckContext* preCheckFunction(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto returnType = expression->subTypes().at("return-type");
            auto functionName = expression->subTypes().at("name");
            auto functionType = context->functionTypeSymbolTable.at(functionName);
            if (expression->subTypes().at("returns-amount") == "0" && returnType != "Unit") {
                throwTypeError(expression, "Missing return statement in function that should return a value");
            }
            context->functionTypeStack.stack().push(functionType);
            context->typeSymbolTable.pushFront();
            return context;
        }

        /**
         * Post-order type checking code for function expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckFunction(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {            
            context->typeSymbolTable.popFront();
            context->functionTypeStack.pop();
            return context;
        }

        /**
         * Post-order type checking code for function parameters.
         */
        TypeChecker::DTypeCheckContext* postCheckFunctionParameter(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto parameterName = expression->subTypes().at("name");
            auto parameterType = expression->subTypes().at("value-type");
            context->typeSymbolTable.insert(parameterName, parameterType);
            return context;
        }

        /**
         * Pre-order type checking code for module expressions.
         */
        TypeChecker::DTypeCheckContext* preCheckModule(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->functionTypeSymbolTable.pushFront();
            context->typeSymbolTable.pushFront();

            std::for_each(expression->children().begin(), expression->children().end(), [context](auto child) {
                if (child->type() == "function") {
                    auto functionName = child->subTypes().at("name");
                    auto parameters = child->children().at(0)->children();
                    auto returnType = child->subTypes().at("return-type");

                    auto parameterTypes = std::vector<std::string>{};
                    std::transform(
                        parameters.begin(),
                        parameters.end(),
                        std::back_inserter(parameterTypes),
                        [](auto parameter) {
                            return parameter->subTypes().at("value-type");
                        }
                    );

                    auto functionType = TypeChecker::DFunctionType{parameterTypes, returnType};
                    context->functionTypeSymbolTable.insert(functionName, functionType);
                }
            });

            return context;
        }

        /**
         * Post-order type checking code for module expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckModule(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->functionTypeSymbolTable.popFront();
            context->typeSymbolTable.popFront();
            return context;
        }

        /**
         * Top-level post-order type checker for any expression type.
         */
        TypeChecker::DTypeCheckContext* postCheckAny(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->postTypeCheckers->contains(expression->type())) {
                // Choose the corresponding type checker function.
                auto generate = context->postTypeCheckers->at(expression->type());
                return generate(context, expression);
            } else {
                return context;
            }
        }

        /**
         * Top-level pre-order type checker for any expression type.
         */
        TypeChecker::DTypeCheckContext* preCheckAny(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->preTypeCheckers->contains(expression->type())) {
                // Choose the corresponding type checker function.
                auto generate = context->preTypeCheckers->at(expression->type());
                return generate(context, expression);
            } else {
                return context;
            }
        }
    }
    
    TypeChecker::TypeChecker()
    {
        this->_preTypeCheckers = {
            {"block", TypeCheckers::preCheckBlock},
            {"function", TypeCheckers::preCheckFunction},
            {"module", TypeCheckers::preCheckModule},
            {"while", TypeCheckers::preCheckWhile}
        };

        this->_postTypeCheckers = {
            {"number", TypeCheckers::postCheckNumber},
            {"boolean", TypeCheckers::postCheckBoolean},
            {"chain", TypeCheckers::postCheckChain},
            {"binary-operator", TypeCheckers::postCheckBinaryOperator},
            {"unary-operator", TypeCheckers::postCheckUnaryOperator},
            {"function-call", TypeCheckers::postCheckFunctionCall},
            {"variable-declaration", TypeCheckers::postCheckVariableDeclaration},
            {"identifier", TypeCheckers::postCheckIdentifier},
            {"block", TypeCheckers::postCheckBlock},
            {"if", TypeCheckers::postCheckIf},
            {"while", TypeCheckers::postCheckWhile},
            {"function", TypeCheckers::postCheckFunction},
            {"function-parameter", TypeCheckers::postCheckFunctionParameter},
            {"return", TypeCheckers::postCheckReturn}
        };
    }

    void TypeChecker::check(TExpression root)
    {
        auto context = TypeChecker::DTypeCheckContext{&(this->_preTypeCheckers), &(this->_postTypeCheckers)};
        auto foldable = DataStructures::FoldableNode<TypeChecker::DTypeCheckContext*, TypeChecker::TExpression>{root};
        foldable.setPreFolder(TypeCheckers::preCheckAny);
        foldable.fold(TypeCheckers::postCheckAny, &context);
    }
}
