#include "IRGenerator.h"
#include <iostream>

namespace MyLanguage {
    IRGenerator::IRGenerator()
    {
        this->_irGenerators.insert({"number", IRGenerator::generateNumber});
        this->_irGenerators.insert({"chain", IRGenerator::generateChain});
    }

    std::vector<IRCommand> IRGenerator::generate(std::shared_ptr<Expression> root)
    {
        auto foldable = DataStructures::FoldableNode<std::vector<IRCommand>, Expression>{root};
        
        return foldable.fold(
            [this](std::shared_ptr<Expression> node, std::vector<std::vector<IRCommand>> accs) {
                if (this->_irGenerators.contains(node->type())) {
                    // Choose the corresponding IR generator function.
                    auto generate = this->_irGenerators.at(node->type());
                    return std::vector<std::vector<IRCommand>>{generate(this, node, accs)};
                } else {
                    throw std::runtime_error("No generator found for type: '" + node->type() + "'.");
                }
            },
            std::vector<IRCommand>{}
        );
    }

    std::vector<IRCommand> IRGenerator::generateNumber(
        std::shared_ptr<Expression> expression, 
        std::vector<std::vector<IRCommand>> accs
    ) {
        auto number = expression->rootToken().value;
        auto command = this->_commandFactory.createLoadIntConst(number);
        return std::vector<IRCommand>{command};
    }

    std::vector<IRCommand> IRGenerator::generateChain(
        std::shared_ptr<Expression> expression, 
        std::vector<std::vector<IRCommand>> accs
    ) {
        auto result = std::vector<IRCommand>{};
        std::for_each(
            accs.begin(), 
            accs.end(),
            [&result](std::vector<IRCommand> acc) {
                result.insert(result.end(), acc.begin(), acc.end());
            }
        );
        return result;
    }
}