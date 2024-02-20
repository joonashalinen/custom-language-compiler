#include "IRGenerator.h"
#include <iostream>
#include <cassert>

namespace MyLanguage {
    IRGenerator::IRGenerator()
    {
        this->_irGenerators.insert({"number", IRGenerator::generateNumber});
        this->_irGenerators.insert({"chain", IRGenerator::generateChain});
        this->_irGenerators.insert({"binary-operator", IRGenerator::generateBinaryOperation});
    }

    std::vector<IRCommand> IRGenerator::generate(std::shared_ptr<Expression> root)
    {
        auto foldable = DataStructures::FoldableNode<IRGenerator::TGeneratorResult, Expression>{root};

        return foldable.fold(
            [this](std::shared_ptr<Expression> node, std::vector<IRGenerator::TGeneratorResult> childResults) {
                if (this->_irGenerators.contains(node->type())) {
                    // Choose the corresponding IR generator function.
                    auto generate = this->_irGenerators.at(node->type());
                    return std::vector<IRGenerator::TGeneratorResult>{generate(this, node, childResults)};
                } else {
                    throw std::runtime_error("No generator found for type: '" + node->type() + "'.");
                }
            },
            IRGenerator::TGeneratorResult{"None", std::vector<IRCommand>{}}
        ).second;
    }

    IRGenerator::TGeneratorResult IRGenerator::generateNumber(
        std::shared_ptr<Expression> expression, 
        std::vector<IRGenerator::TGeneratorResult> childResults
    ) {
        auto number = expression->rootToken().value;
        auto variable = this->_commandFactory.nextVariable();
        auto command = this->_commandFactory.createLoadIntConst(number, variable);
        return IRGenerator::TGeneratorResult{variable, std::vector<IRCommand>{command}};
    }

    IRGenerator::TGeneratorResult IRGenerator::generateChain(
        std::shared_ptr<Expression> expression, 
        std::vector<IRGenerator::TGeneratorResult> childResults
    ) {
        auto result = IRGenerator::TGeneratorResult{"None", std::vector<IRCommand>{}};
        std::for_each(
            childResults.begin(), 
            childResults.end(),
            [&result](IRGenerator::TGeneratorResult childResult) {
                result.second.insert(result.second.end(), childResult.second.begin(), childResult.second.end());
            }
        );
        return result;
    }

    IRGenerator::TGeneratorResult IRGenerator::generateBinaryOperation(
        std::shared_ptr<Expression> expression, 
        std::vector<IRGenerator::TGeneratorResult> childResults
    ) {
        assert(childResults.size() == 2);
        assert(expression->subTypes().contains("operator"));

        auto leftVariable = childResults.at(0).first;
        auto rightVariable = childResults.at(1).first;

        auto joinedIRCommands = this->generateChain(expression, childResults).second;
        auto resultVariable = this->_commandFactory.nextVariable();
        auto command = this->_commandFactory.createCall(
            expression->subTypes().at("operator"), 
            std::vector<TIRVariable>{leftVariable, rightVariable}, 
            resultVariable
        );
        joinedIRCommands.insert(joinedIRCommands.end(), command);

        return IRGenerator::TGeneratorResult{resultVariable, joinedIRCommands};
    }
}