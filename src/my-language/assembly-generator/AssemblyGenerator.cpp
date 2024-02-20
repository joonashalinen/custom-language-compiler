#include "AssemblyGenerator.h"

namespace MyLanguage {
    AssemblyGenerator::AssemblyGenerator()
    {
        
    }

    std::string AssemblyGenerator::generate(std::vector<IRCommand> irCommands)
    {
        return this->_prelude + std::accumulate(
            irCommands.begin(), 
            irCommands.end(), 
            std::string(""), 
            [this](std::string assembly, IRCommand command) {
                if (this->_generators.contains(command.type())) {
                    auto generator = this->_generators.at(command.type());
                    return assembly + generator(command);
                } else {
                    throw std::runtime_error("No assembly generator found command with type: '" + command.type() + "'.");
                }
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
