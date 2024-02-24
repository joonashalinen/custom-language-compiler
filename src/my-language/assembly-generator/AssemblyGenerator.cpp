#include "AssemblyGenerator.h"
#include <iostream>

namespace MyLanguage {
    AssemblyGenerator::AssemblyGenerator()
    {
        
    }

    std::string AssemblyGenerator::generate(std::vector<TIRCommand> irCommands)
    {
        // Gather the names of all variables from the IR commands.
        auto variableNames = std::set<std::string>{};
        std::for_each(irCommands.begin(), irCommands.end(), [&variableNames](TIRCommand command) {
            std::for_each(command->children().begin(), command->children().end(), [&variableNames](TExpression child) {
                if (child->type() == "variable") {
                    variableNames.insert(child->subTypes().at("name"));
                }
            });
        });

        // Reserve memory locations for each variable.
        auto variableNamesList = std::vector<std::string>(variableNames.size());
        std::copy(variableNames.begin(), variableNames.end(), variableNamesList.begin());
        this->_variableStack.push(variableNamesList);

        // Make sure we reserve an amount of memory divisible by 16 to 
        // ensure the stack is initially aligned to 16 bytes.
        auto reserveMemoryAmount = (
            this->_variableStack.size() % 16 == 0 ? 
                this->_variableStack.size() : 
                this->_variableStack.size() + 8
        );

        // Reserve the required amount of memory on the stack to store all variables.
        auto beginPortion = (
            this->_prelude + 
            this->_indent + "subq $" + std::to_string(reserveMemoryAmount) + ", %rsp" + "\n"
        );

        auto endPortion = (
            this->_indent + "movq $0, %rax" + "\n" + 
            this->_indent + "movq %rbp, %rsp" + "\n" + 
            this->_indent + "popq %rbp" + "\n" + 
            this->_indent + "ret" + "\n"
        );

        // Generate assembly code for each IR command and join it all together.
        return beginPortion + std::accumulate(
            irCommands.begin(), 
            irCommands.end(), 
            std::string(""), 
            [this](std::string assembly, TIRCommand command) {
                auto commandType = command->subTypes().at("command");
                if (this->_generators.contains(commandType)) {
                    auto generator = this->_generators.at(commandType);
                    return assembly + generator(this->_variableStack, this->_indent, command);
                } else {
                    throw std::runtime_error("No assembly generator found for command with type: '" + commandType + "'.");
                }
            }
        ) + endPortion;
    }

    std::string AssemblyGenerator::prelude()
    {
        return this->_prelude;
    }

    std::string AssemblyGenerator::indent()
    {
        return this->_indent;
    }

    std::map<std::string, AssemblyGenerator::TGenerator>& AssemblyGenerator::generators()
    {
        return this->_generators;
    }
    
    void AssemblyGenerator::setPrelude(std::string prelude)
    {
        this->_prelude = prelude;
    }

    void AssemblyGenerator::setIndent(std::string indent)
    {
        this->_indent = indent;
    }
}
