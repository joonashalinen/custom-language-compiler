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
            auto variableLocation = variableStack.negativeEndLocation(variable);
            return (
                indent + "movq " + "$" + value + ", " + std::to_string(variableLocation) + "(%rbp)" + "\n"
            );
        }

        std::string generateAdd(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            std::pair<std::string, std::string> summands,
            std::string outputVar
        ) {
            auto firstSummandLocation = variableStack.negativeEndLocation(summands.first);
            auto secondSummandLocation = variableStack.negativeEndLocation(summands.second);
            auto outputVarLocation = variableStack.negativeEndLocation(outputVar);
            return (
                indent + "movq " + std::to_string(firstSummandLocation) + "(%rbp)" + ", %rax" + "\n" +
                indent + "addq " + std::to_string(secondSummandLocation) + "(%rbp)" + ", %rax" + "\n" +
                indent + "movq " + "%rax, " + std::to_string(outputVarLocation) + "(%rbp)" + "\n"
            );
        }

        std::string generateCopy(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto variables = command->extractChildSubTypeValues("variable", "name");
            assert(variables.size() == 2);
            auto fromVarLocation = variableStack.negativeEndLocation(variables.at(0));
            auto toVarLocation = variableStack.negativeEndLocation(variables.at(1));
            return (
                indent + "movq " + std::to_string(fromVarLocation) + "(%rbp)" + ", %rax" + "\n" +
                indent + "movq " + "%rax, " + std::to_string(toVarLocation) + "(%rbp)" + "\n"
            );
        }

        std::string generateFunctionCall(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            std::string functionName,
            std::vector<std::string> argumentVars,
            std::string outputVar
        ) {
            // Currently we only support function calls that take a single argument.
            auto argLocation = variableStack.negativeEndLocation(argumentVars.at(0));
            auto outputLocation = variableStack.negativeEndLocation(outputVar);
            return (
                indent + "movq " + std::to_string(argLocation) + "(%rbp)" + ", %rdi" + "\n" +
                indent + "call " + functionName + "\n" +
                indent + "movq " + "%rax, " + std::to_string(outputLocation) + "(%rbp)" + "\n"
            );
        }

       std::string generateCall(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto functionName = command->children().at(0)->subTypes().at("name");
            auto argumentVars = command->children().at(1)->extractChildSubTypeValues("variable", "name");
            auto outputVarName = command->children().at(2)->subTypes().at("name");

            if (functionName == "+") {
                return generateAdd(variableStack, indent, {argumentVars.at(0), argumentVars.at(1)}, outputVarName);
            } else {
                return generateFunctionCall(variableStack, indent, functionName, argumentVars, outputVarName);
            }
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
        this->_assemblyGenerator.generators().insert({"Call", X86AssemblyGenerators::generateCall});
        this->_assemblyGenerator.generators().insert({"Copy", X86AssemblyGenerators::generateCopy});
    }

    std::string X86AssemblyGenerator::generate(std::vector<TIRCommand> irCommands)
    {
        return this->_assemblyGenerator.generate(irCommands);
    }
}
