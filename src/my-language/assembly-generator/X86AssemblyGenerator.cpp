#include "X86AssemblyGenerator.h"

namespace MyLanguage {
    /**
     * Assembly generating functions for each IR command type.
     */
    namespace X86AssemblyGenerators {
        std::string generateLoadIntConst(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto value = command->children().at(0)->subTypes().at("value");
            auto variable = command->children().at(1)->subTypes().at("name");
            auto variableLocation = variableStack.location(variable) + 8;
            return (
                indent + "movl " + "$" + value + ", " + "-" + std::to_string(variableLocation) + "(%rbp)" + "\n"
            );
        }
    }

    X86AssemblyGenerator::X86AssemblyGenerator()
    {
        std::string indent = std::string("    ");
        this->_assemblyGenerator.setIndent(indent);

        this->_assemblyGenerator.setPrelude(
            indent + ".extern print_int" + "\n" +
            indent + ".extern print_bool" + "\n" +
            indent + ".extern read_int" + "\n" +
            indent + ".global main" + "\n" +
            indent + "\n" +
            indent + ".section .text" + "\n" +
            indent + "\n" + 
            "main:" + "\n" + 
            indent + "pushq %rbp" + "\n" + 
            indent + "movq %rsp, %rbp" + "\n"
        );

        // Configure assembly generating functions for each IR command type.
        this->_assemblyGenerator.generators().insert({"LoadIntConst", X86AssemblyGenerators::generateLoadIntConst});
    }

    std::string X86AssemblyGenerator::generate(std::vector<TIRCommand> irCommands)
    {
        return this->_assemblyGenerator.generate(irCommands);
    }
}
