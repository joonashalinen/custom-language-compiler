#include "AssemblyGenerator.h"
#include <iostream>

namespace MyLanguage {
    AssemblyGenerator::AssemblyGenerator()
    {
        
    }

    std::string AssemblyGenerator::generateForFunction(std::vector<TIRCommand> irCommands) {
        // Gather the names of all variables from the IR commands.
        auto variableNames = std::set<std::string>{};
        std::for_each(irCommands.begin(), irCommands.end(), [&variableNames](TIRCommand command) {
            std::for_each(command->children().begin(), command->children().end(), [&variableNames](TExpression child) {
                if (child->type() == "variable") {
                    auto variableName = child->subTypes().at("name");
                    // If the variable is not a function.
                    if (variableName.back() != 'F') {
                        variableNames.insert(variableName);                    
                    }
                }
            });
        });

        // Reserve memory locations for each variable.
        auto variableStack = StructuredLanguage::VariableStack{};
        auto variableNamesList = std::vector<std::string>(variableNames.size());
        std::copy(variableNames.begin(), variableNames.end(), variableNamesList.begin());
        variableStack.push(variableNamesList);

        // Generate assembly code for each IR command and join it all together.
        return std::accumulate(
            irCommands.begin(), 
            irCommands.end(), 
            std::string(""), 
            [this, &variableStack](std::string assembly, TIRCommand command) {
                auto commandType = command->subTypes().at("command");
                if (this->_generators.contains(commandType)) {
                    assembly = assembly + this->_indent + "# " + commandType + "\n";
                    auto generator = this->_generators.at(commandType);
                    return assembly + generator(variableStack, this->_indent, command);
                } else {
                    throw std::runtime_error("No assembly generator found for command with type: '" + commandType + "'.");
                }
            }
        );
    }

    std::string AssemblyGenerator::generate(std::map<std::string, std::vector<TIRCommand>> irCommands)
    {
        return this->_prelude + std::accumulate(
            irCommands.begin(), 
            irCommands.end(), 
            std::string(""), 
            [this](std::string assembly, std::pair<std::string, std::vector<TIRCommand>> p) {
                return assembly + this->generateForFunction(p.second);
            }
        );
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
