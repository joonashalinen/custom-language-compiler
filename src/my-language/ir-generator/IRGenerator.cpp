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
         * Generates the IR commands for a binary operator expression.
         */
        IRGenerator::TGeneratorResult generateBinaryOperation(
            IRCommandFactory& commandFactory,
            std::shared_ptr<Expression> expression, 
            std::vector<IRGenerator::TGeneratorResult> childResults
        ) {
            assert(childResults.size() == 2);
            assert(expression->subTypes().contains("operator"));

            auto leftVariable = childResults.at(0).first;
            auto rightVariable = childResults.at(1).first;

            auto joinedIRCommands = generateChain(commandFactory, expression, childResults).second;
            auto resultVariable = commandFactory.nextVariable();
            auto command = commandFactory.createCall(
                expression->subTypes().at("operator"), 
                std::vector<TIRVariable>{leftVariable, rightVariable}, 
                resultVariable
            );
            joinedIRCommands.insert(joinedIRCommands.end(), command);

            return IRGenerator::TGeneratorResult{resultVariable, joinedIRCommands};
        }
    }

    IRGenerator::IRGenerator()
    {
        this->_irGenerators.insert({"number", IRGenerators::generateNumber});
        this->_irGenerators.insert({"chain", IRGenerators::generateChain});
        this->_irGenerators.insert({"binary-operator", IRGenerators::generateBinaryOperation});
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