#include "X86AssemblyGenerator.h"

namespace MyLanguage {
    /**
     * Assembly generating functions for each IR command type.
     */
    namespace X86AssemblyGenerators {
        std::string generateLoadIntConst(TIRCommand command)
        {
            return (
                std::string("movq A, %rax") + "\n" +
                "addq B, %rax" + "\n"
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
            indent + ".type main, @function" + "\n" +
            indent + "\n" +
            indent + ".section .text" + "\n" +
            indent + "\n" + 
            "main:" + "\n"
        );

        // Configure assembly generating functions for each IR command type.
        this->_assemblyGenerator.generators().insert({"LoadIntConst", X86AssemblyGenerators::generateLoadIntConst});
    }

    std::string X86AssemblyGenerator::generate(std::vector<TIRCommand> irCommands)
    {
        return this->_assemblyGenerator.generate(irCommands);
    }
}
