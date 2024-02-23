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
        IRGenerator::TGeneratorResult generateNumber(
            IRCommandFactory& commandFactory,
            std::shared_ptr<Expression> expression, 
            std::vector<IRGenerator::TGeneratorResult> childResults
        ) {
            auto number = expression->rootToken().value;
            auto variable = commandFactory.nextVariable();
            auto command = commandFactory.createLoadIntConst(number, variable);
            return IRGenerator::TGeneratorResult{variable, std::vector<TIRCommand>{command}};
        }

        /**
         * Generates the IR commands for a chain expression.
         */
        IRGenerator::TGeneratorResult generateChain(
            IRCommandFactory& commandFactory,
            std::shared_ptr<Expression> expression, 
            std::vector<IRGenerator::TGeneratorResult> childResults
        ) {
            auto result = IRGenerator::TGeneratorResult{"None", std::vector<TIRCommand>{}};
            std::for_each(
                childResults.begin(), 
                childResults.end(),
                [&result](IRGenerator::TGeneratorResult childResult) {
                    result.second.insert(result.second.end(), childResult.second.begin(), childResult.second.end());
                }
            );
            return result;
        }

        /**
         * Generates the IR commands for a function call expression. 
         * Unary and binary operations are also interpreted as function calls.
         */
        IRGenerator::TGeneratorResult generateFunctionCall(
            IRCommandFactory& commandFactory,
            std::shared_ptr<Expression> expression, 
            std::vector<IRGenerator::TGeneratorResult> childResults
        ) {
            assert(expression->subTypes().contains("name"));

            // Gather the variable names storing the child results.
            auto argumentVars = std::vector<TIRVariable>(childResults.size());
            std::transform(
                childResults.begin(), 
                childResults.end(), 
                argumentVars.begin(), 
                [](IRGenerator::TGeneratorResult& childResult) {
                    return childResult.first;
                }
            );

            // Generate IR commands for the arguments first.
            auto joinedIRCommands = generateChain(commandFactory, expression, childResults).second;
            // Reserve the next variable we want to store the function call's result in.
            auto resultVariable = commandFactory.nextVariable();
            // Create the IR command for the function call itself.
            auto command = commandFactory.createCall(
                expression->subTypes().at("name"), 
                argumentVars,
                resultVariable
            );
            // Add the function call IR command to the total list of IR commands.
            joinedIRCommands.insert(joinedIRCommands.end(), command);

            return IRGenerator::TGeneratorResult{resultVariable, joinedIRCommands};
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
        auto foldable = DataStructures::FoldableNode<IRGenerator::TGeneratorResult, Expression>{root};

        return foldable.fold(
            [this](std::shared_ptr<Expression> node, std::vector<IRGenerator::TGeneratorResult> childResults) {
                if (this->_irGenerators.contains(node->type())) {
                    // Choose the corresponding IR generator function.
                    auto generate = this->_irGenerators.at(node->type());
                    return std::vector<IRGenerator::TGeneratorResult>{generate(this->_commandFactory, node, childResults)};
                } else {
                    throw std::runtime_error("No generator found for type: '" + node->type() + "'.");
                }
            },
            IRGenerator::TGeneratorResult{"None", std::vector<TIRCommand>{}}
        ).second;
    }
}