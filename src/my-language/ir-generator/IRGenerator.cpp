#include "IRGenerator.h"
#include <iostream>
#include <cassert>

namespace MyLanguage {

    /**
     * Functions for generating IR commands for different types of expressions.
     */
    namespace IRGenerators {
    
        /**
         * Generates the IR commands for a literal expression.
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
         * Generates the IR commands for a chain expression.
         */
        IRGenerator::DGeneratorContext* generateChain(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // We currently do nothing.
            return context;
        }

        /**
         * Generates the IR commands for a function call expression. 
         * Unary and binary operations are also interpreted as function calls.
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
                throw std::runtime_error("No generator found for type: '" + expression->type() + "'.");
            }
        }
    }

    IRGenerator::IRGenerator()
    {
        this->_irGenerators.insert({"number", IRGenerators::generateNumber});
        this->_irGenerators.insert({"chain", IRGenerators::generateChain});
        this->_irGenerators.insert({"binary-operator", IRGenerators::generateFunctionCall});
        this->_irGenerators.insert({"function-call", IRGenerators::generateFunctionCall});
    }

    std::vector<TIRCommand> IRGenerator::generate(std::shared_ptr<Expression> root)
    {
        auto context = IRGenerator::DGeneratorContext{&this->_irGenerators, &this->_commandFactory};
        auto foldable = DataStructures::FoldableNode<IRGenerator::DGeneratorContext*, IRGenerator::TExpression>{root};

        return foldable.fold(IRGenerators::generateAny, &context)->irCommands;
    }

    std::map<std::string, IRGenerator::TIRGenerator>& IRGenerator::irGenerators()
    {
        return this->_irGenerators;
    }
}