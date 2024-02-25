#include "IRGenerator.h"
#include <iostream>
#include <cassert>

namespace MyLanguage {

    /**
     * Functions for generating IR commands for different types of expressions.
     */
    namespace IRGenerators {
    
        /**
         * Generator that does nothing. Useful for passing over abstract syntax tree nodes that 
         * have no meaning for IR generation.
         */
        IRGenerator::DGeneratorContext* nullGenerator(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            return context;
        }

        /**
         * Generates the IR commands for a number literal expression.
         */
        IRGenerator::DGeneratorContext* generateNumber(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto number = expression->rootToken().value;
            auto variable = context->commandFactory->nextVariable();
            auto command = context->commandFactory->createLoadIntConst(number, variable);
            context->variableStack.push({variable});
            context->irCommands.insert(context->irCommands.end(), command);
            return context;
        }

        /**
         * IR generation function for a variable declaration expression.
         */
        IRGenerator::DGeneratorContext* generateVariableDeclaration(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            assert(expression->subTypes().contains("name"));
            auto variableName = expression->subTypes().at("name");
            // The value on the right side of the variable is given by the IR variable
            // at the top of the variable stack.
            auto rightVariable = (context->variableStack.pop(1)).at(0);
            // Create the IR variable for the newly created variable.
            auto irVariable = context->commandFactory->nextVariable();
            // Insert the newly created variable to the symbol table and the variable stack.
            context->symbolTable.insert(variableName, irVariable);
            context->variableStack.push({irVariable});
            // Assign the value on the right side to the newly created variable.
            auto command = context->commandFactory->createCopy(rightVariable, irVariable);
            context->irCommands.insert(context->irCommands.end(), command);
            return context;
        }

        /**
         * IR generation function for an identifier expression.
         */
        IRGenerator::DGeneratorContext* generateIdentifier(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto name = expression->subTypes().at("literal-value");
            auto irVariable = context->symbolTable.at(name);
            context->variableStack.push({irVariable});
            return context;
        }

        /**
         * IR generation function for a block expression.
         */
        IRGenerator::DGeneratorContext* generateBlock(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Leave the block's local variable scope.
            context->symbolTable.popFront();
            return context;
        }

        /**
         * Generates the IR commands for a chain expression.
         */
        IRGenerator::DGeneratorContext* generateChain(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (expression->children().size() > 0) {
                auto irVariables = context->variableStack.pop(expression->children().size());
                std::string lastIRVariable = irVariables.back();
                context->variableStack.push({lastIRVariable});
            }
            return context;
        }

        /**
         * Generates the IR commands for a function call expression. 
         * Unary and binary operations other than '=' are also interpreted as function calls.
         */
        IRGenerator::DGeneratorContext* generateFunctionCall(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            assert(expression->subTypes().contains("name"));

            // Gather the variable names storing the argument values.
            auto argumentVars = context->variableStack.pop(expression->children().size());
            // Reserve the next variable we want to store the function call's result in.
            auto resultVariable = context->commandFactory->nextVariable();
            // Create the IR command for the function call.
            auto command = context->commandFactory->createCall(
                expression->subTypes().at("name"), 
                argumentVars,
                resultVariable
            );

            context->irCommands.insert(context->irCommands.end(), command);
            context->variableStack.push({resultVariable});

            return context;
        }

        /**
         * Generates the IR commands for the '=' binary operation.
         */
        IRGenerator::DGeneratorContext* generateAssignment(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            assert(expression->children().size() == 2);
            assert(expression->children().at(0)->type() == "identifier");
            // Name of the variable being assigned to.
            auto variableName = expression->children().at(0)->subTypes().at("literal-value");
            // Name of the corresponding IR variable.
            auto leftIRVariable = context->symbolTable.at(variableName);
            // The value on the right side of the variable is given by the IR variable
            // at the top of the variable stack.
            auto rightIRVariable = (context->variableStack.pop(1)).at(0);
            // Assign the value on the right side to the newly created variable.
            auto command = context->commandFactory->createCopy(rightIRVariable, leftIRVariable);
            context->irCommands.insert(context->irCommands.end(), command);
            // Push Unit to the variable stack. Assignment operations 
            // always return Unit instead of an actual value.
            context->variableStack.push({"Unit"});
            return context;
        }

        /**
         * Generates IR commands for binary operators.
         */
        IRGenerator::DGeneratorContext* generateBinaryOperator(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (expression->subTypes().at("name") == "=") {
                return generateAssignment(context, expression);
            } else {
                return generateFunctionCall(context, expression);
            }
        }

        /**
         * Top-level generator for generating IR commands for any given expression.
         */
        IRGenerator::DGeneratorContext* generateAny(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->irGenerators->contains(expression->type())) {
                // Choose the corresponding IR generator function.
                auto generate = context->irGenerators->at(expression->type());
                return generate(context, expression);
            } else {
                std::cout << expression->rootToken().value << std::endl;
                throw std::runtime_error("No generator found for type: '" + expression->type() + "'.");
            }
        }

        /**
         * Does any needed modifications to the accumulated context value when 
         * encountering a node in a pre-order traversal.
         */
        IRGenerator::DGeneratorContext* preHandle(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Create new local variable scope when encountering a block expression.
            if (expression->type() == "block") {
                context->symbolTable.pushFront();
            }
            return context;
        }
    }

    IRGenerator::IRGenerator()
    {
        this->_irGenerators.insert({"number", IRGenerators::generateNumber});
        this->_irGenerators.insert({"chain", IRGenerators::generateChain});
        this->_irGenerators.insert({"binary-operator", IRGenerators::generateBinaryOperator});
        this->_irGenerators.insert({"function-call", IRGenerators::generateFunctionCall});
        this->_irGenerators.insert({"variable-declaration", IRGenerators::generateVariableDeclaration});
        this->_irGenerators.insert({"identifier", IRGenerators::generateIdentifier});
        this->_irGenerators.insert({"block", IRGenerators::nullGenerator});
        this->_irGenerators.insert({"parenthetical", IRGenerators::nullGenerator});
    }

    std::vector<TIRCommand> IRGenerator::generate(std::shared_ptr<Expression> root)
    {
        auto context = IRGenerator::DGeneratorContext{&this->_irGenerators, &this->_commandFactory};
        auto foldable = DataStructures::FoldableNode<
            IRGenerator::DGeneratorContext*, 
            IRGenerator::TExpression
        >{root, IRGenerators::preHandle};

        return foldable.fold(IRGenerators::generateAny, &context)->irCommands;
    }

    std::map<std::string, IRGenerator::TIRGenerator>& IRGenerator::irGenerators()
    {
        return this->_irGenerators;
    }
}