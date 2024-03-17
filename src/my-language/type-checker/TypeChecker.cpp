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
                std::string("Error encountered at ") + 
                expression->startLocation().toString() + ": " + message + "."
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

            if (
                valueTypes.at(0) != "Any" &&
                valueTypes.at(1) != "Any" &&
                valueTypes.at(0) != valueTypes.at(1)
            ) {
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
                    {"!=", {"Int", "Bool"}},
                    {"+", {"Int"}},
                    {"-", {"Int"}},
                    {"*", {"Int"}},
                    {"/", {"Int"}},
                    {"%", {"Int"}}
                };
                auto returnedTypes = std::map<std::string, std::string>{
                    {"and", "Bool"},
                    {"or", "Bool"},
                    {"<", "Bool"},
                    {">", "Bool"},
                    {"<=", "Bool"},
                    {">=", "Bool"},
                    {"==", "Bool"},
                    {"!=", "Bool"},
                    {"+", "Int"},
                    {"-", "Int"},
                    {"*", "Int"},
                    {"/", "Int"},
                    {"%", "Int"}
                };
                if (!(acceptedTypes.contains(operatorName))) {
                    throwTypeError(expression, std::string("'") + operatorName + "' is not a recognized binary operator");
                }
                auto acceptedTypesForOperator = acceptedTypes.at(operatorName);
                if (valueTypes.at(0) != "Any" && !(acceptedTypesForOperator.contains(valueTypes.at(0)))) {
                    throwTypeError(
                        expression,
                        std::string("The left-hand type '") + valueTypes.at(0) + "' to binary operator '" + 
                        operatorName + "' is an invalid type"
                    );
                } else if (valueTypes.at(1) != "Any" && !(acceptedTypesForOperator.contains(valueTypes.at(1)))) {
                    throwTypeError(
                        expression,
                        std::string("The right-hand type '") + valueTypes.at(1) + "' to binary operator '" + 
                        operatorName + "' is an invalid type"
                    );
                }
                context->typeStack.stack().push(returnedTypes.at(operatorName));
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
                    "' has already been declared in this scope"
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
            // If the right hand expression is a function.
            if (rightHandType.find("=>") != std::string::npos && rightHandType.back() != '*') {
                // Add the variable also as a function to the symbol table of functions.
                auto functionType = context->functionTypes.at(rightHandType);
                context->functionTypeSymbolTable.insert(variableName, functionType);
            }
            return context;
        }

        /**
         * Type checking code for variable assignments.
         */
        TypeChecker::DTypeCheckContext* postCheckAssignment(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto leftHand = expression->children().at(0);

            // First, we check that the left hand side of the assignment is a valid type.

            // If the left side of the assignment is not an identifier or a pointer dereference.
            if (
                leftHand->type() != "identifier" && 
                (
                    leftHand->type() != "unary-operator" || 
                    (
                        leftHand->type() == "unary-operator" && 
                        leftHand->subTypes().at("name") != "*"
                    )
                )
            ) {
                throwTypeError(
                    expression,
                    std::string("Left-hand side of assignment is not an identifier or a pointer dereference")
                );
            }

            // Next, we want to find the name of the variable present in the left hand expression.

            // How many pointer dereferences are preceding the left hand variable identifier.
            auto dereferenceLevel = 0;

            // If the left hand expression is a pointer dereference.
            if (leftHand->type() == "unary-operator") {
                // Find the variable identifier expression.
                while (leftHand->type() == "unary-operator") {
                    leftHand = leftHand->children().at(0);
                    dereferenceLevel = dereferenceLevel + 1;
                }
            }

            // Name of the variable present in the left hand expression.
            auto variableName = leftHand->subTypes().at("name");

            // Next, we want to check that the left hand variable exists in the symbol table.

            // If the variable does not exist.
            if (!(context->typeSymbolTable.contains(variableName))) {
                throwTypeError(
                    expression, 
                    std::string("Trying to assign to a variable '") + variableName + 
                    "' that has not been defined"
                );
            }

            // Now, we want to find the type of the left hand expression.

            auto leftHandType = context->typeSymbolTable.at(variableName);

            // If the left hand expression contains pointer dereferences.
            if (dereferenceLevel != 0) {
                // The type of the left hand expression is the type of the associated variable minus 
                // the levels of pointer asterisks accessed.
                for (int i = 0; i < dereferenceLevel; i++) {
                    leftHandType.pop_back();
                }
            }

            // Finally, we can now check that the types of the left hand expression and the 
            // right hand expression match.

            // Type of the value being assigned.
            auto valueType = context->typeStack.pop();
            // If the types do not match.
            if (leftHandType != valueType) {
                auto errorMessage = (
                    std::string("The type of the variable '") + variableName + 
                    "' being assigned to was of type '" + leftHandType +
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
            auto& valueType = context->typeStack.stack().top();
            auto operatorName = expression->subTypes().at("name");
            auto invalidTypeError = (
                std::string("The type '") + valueType + 
                "' is not a valid operand type for the unary operator '" + operatorName + "'"
            );
            if (operatorName == "new") {
                if (valueType == "Unit") {
                    throwTypeError(expression, "Cannot allocate a value of type 'Unit' to the heap.");
                }
                valueType = valueType + "*";
            } else if (operatorName == "delete") {
                if (!(valueType.find("*") != std::string::npos)) {
                    throwTypeError(expression, invalidTypeError);
                }
                valueType = "Unit";
            } else if ((std::set<std::string>{"not", "-"}).contains(operatorName)) {
                auto acceptedTypes = std::map<std::string, std::set<std::string>>{
                    {"not", {"Bool"}},
                    {"-", {"Int"}}
                };
                if (!(acceptedTypes.at(operatorName).contains(valueType))) {
                    throwTypeError(expression, invalidTypeError);
                }
           } else if (operatorName == "&") {
                // If the type is a function type, then we want to surround it in parentheses before 
                // suffixing it with the pointer asterisk.
                valueType = ((valueType.find("=>") != std::string::npos) && valueType.back() != '*') ? "(" + valueType + ")" : valueType;
                valueType = valueType + "*";
            } else if (operatorName == "*") {
                if (valueType.back() != '*') {
                    throwTypeError(expression, invalidTypeError);
                }
                valueType.pop_back();
                // If the type is surrounded in parentheses we want to strip them.
                if (valueType.back() == ')') {
                    valueType = valueType.substr(1, valueType.size() - 2);
                }
            } else {
                throwTypeError(expression, std::string("'") + operatorName + "' is not a recognized unary operator");
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
            if (
                !(context->typeSymbolTable.contains(variableName)) && 
                !(context->functionTypeSymbolTable.contains(variableName))
            ) {
                throwTypeError(
                    expression, 
                    std::string("Trying to reference a variable '") + variableName + 
                    "' that has not been defined"
                );
            }

            std::string variableType = (
                context->typeSymbolTable.contains(variableName) ? 
                context->typeSymbolTable.at(variableName) : 
                context->functionTypeSymbolTable.at(variableName)->toString()
            );
            
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
            // Types of the portions of the if-statement.
            auto portionTypes = context->typeStack.pop(expression->children().size());
            // If the condition is not a boolean.
            if (portionTypes.at(0) != "Bool") {
                throwTypeError(
                    expression, 
                    std::string("Expected the condition of an if-statement to be of type 'Bool' but instead it was of type '") + 
                    portionTypes.at(0) + "'"
                );
            }
            // If the if-statement includes an else-statement.
            if (expression->children().size() == 3) {
                // Check that the then-portion and the else-portion have the same type.
                if (portionTypes.at(1) != portionTypes.at(2)) {
                    throwTypeError(
                        expression,
                        "The then-portion of if-else statement has type '" + portionTypes.at(1) + 
                        "' but the else-portion has type '" + portionTypes.at(2) + "'"
                    );
                }
                // The return type is the type of the then-portion and else-portion.
                context->typeStack.stack().push(portionTypes.at(1));
            } else {
                context->typeStack.stack().push("Unit");
            }
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
                throwTypeError(expression, "Breaks in loop have mismatching return types");
            } else {
                expectedResultType = breakResultType;
                context->typeStack.stack().push(breakResultType);
            }
            return context;
        }

        /**
         * Post-order type checking code for continue-expressions.
         */
        TypeChecker::DTypeCheckContext* postCheckContinue(
            TypeChecker::DTypeCheckContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->typeStack.stack().push("Unit");
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
            // If the called function is a type constructor.
            if (
                (std::set<std::string>{"Int", "Bool"}).contains(functionName) || 
                functionName.find("Int*") != std::string::npos || 
                functionName.find("Bool*") != std::string::npos
            ) {
                if (expression->children().size() != 1) {
                    throwTypeError(expression, "Type constructor calls must have exactly one argument");
                }
                auto argumentType = context->typeStack.pop();
                if (functionName != argumentType) {
                    throwTypeError(
                        expression, 
                        "Type constructor '" + functionName + "' was given an argument of type " + argumentType
                    );
                }
                context->typeStack.stack().push(functionName);
            } else if (!(context->functionTypeSymbolTable.contains(functionName))) {
                throwTypeError(expression, "No function with name '" + functionName + "' found");
            } else {
                auto functionType = context->functionTypeSymbolTable.at(functionName);
                auto parameterTypes = context->typeStack.pop(expression->children().size());
                if (functionType->parameterTypes().size() != parameterTypes.size()) {
                    throwTypeError(expression, "Mismatching amount of arguments given to function call");
                } else {
                    auto parameterTypesMatch = (std::accumulate(
                        parameterTypes.begin(),
                        parameterTypes.end(),
                        std::pair<int, bool>{0, true}, // Iteration index and the accumulated boolean.
                        [&functionType](auto acc, auto parameterType) {
                            auto acceptedParameter = functionType->parameterTypes().at(acc.first);
                            return std::pair<int, bool>{
                                acc.first + 1,
                                acc.second && (
                                    acceptedParameter->toString() == "Any" || 
                                    parameterType == acceptedParameter->toString()
                                )
                            };
                        }
                    )).second;
                    
                    if (!parameterTypesMatch) {
                        throwTypeError(
                            expression, 
                            "Types of given arguments do not match the expected parameters in function call"
                        );
                    } else {
                        context->typeStack.stack().push(functionType->returnType()->toString());
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
            auto expectedReturnType = context->functionTypeStack.stack().top()->returnType()->toString();
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

                    auto parameterTypes = std::vector<std::shared_ptr<IType>>{};
                    std::transform(
                        parameters.begin(),
                        parameters.end(),
                        std::back_inserter(parameterTypes),
                        [](auto parameter) {
                            return std::shared_ptr<IType>(new Type{parameter->subTypes().at("value-type")});
                        }
                    );

                    auto functionType = std::shared_ptr<FunctionType>(
                        new FunctionType{parameterTypes, std::shared_ptr<IType>(new Type{returnType})}
                    );
                    context->functionTypeSymbolTable.insert(functionName, functionType);
                    context->functionTypes.insert({functionType->toString(), functionType});
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
            {"return", TypeCheckers::postCheckReturn},
            {"assignment", TypeCheckers::postCheckAssignment},
            {"module", TypeCheckers::postCheckModule},
            {"break", TypeCheckers::postCheckBreak},
            {"continue", TypeCheckers::postCheckContinue}
        };
    }

    void TypeChecker::check(TExpression root)
    {
        auto context = TypeChecker::DTypeCheckContext{&(this->_preTypeCheckers), &(this->_postTypeCheckers)};

        // Temporary solution for getting a proper function type object from a function type signature string.
        context.functionTypes = std::map<std::string, std::shared_ptr<FunctionType>>{
            {
                "(Int) => Unit",
                std::shared_ptr<FunctionType>(
                new FunctionType{
                        {std::shared_ptr<IType>(new Type{"Int"})},
                        std::shared_ptr<IType>(new Type{"Unit"})
                    }
                )
            },
            {
                "(Bool) => Unit",
                std::shared_ptr<FunctionType>(
                    new FunctionType{
                        {std::shared_ptr<IType>(new Type{"Bool"})}, 
                        std::shared_ptr<IType>(new Type{"Unit"})
                    }
                )
            },
            {
                "() => Int",
                std::shared_ptr<FunctionType>(
                new FunctionType{
                        {},
                        std::shared_ptr<IType>(new Type{"Int"})
                    }
                )
            }
        };

        // Set types of global built-in functions.
        context.functionTypeSymbolTable.insert("print_int", context.functionTypes.at("(Int) => Unit"));
        context.functionTypeSymbolTable.insert("print_bool", context.functionTypes.at("(Bool) => Unit"));
        context.functionTypeSymbolTable.insert("read_int", context.functionTypes.at("() => Int"));

        auto foldable = DataStructures::FoldableNode<TypeChecker::DTypeCheckContext*, TypeChecker::TExpression>{root};
        foldable.setPreFolder(TypeCheckers::preCheckAny);
        foldable.fold(TypeCheckers::postCheckAny, &context);
    }
}