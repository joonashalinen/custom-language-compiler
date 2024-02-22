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

       std::string generateCall(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto functionName = command->children().at(0)->subTypes().at("name");
            auto outputVarName = command->children().at(2)->subTypes().at("name");
            
            auto argumentExpressions = command->children().at(1)->children();
            auto argumentVars = std::vector<std::string>(argumentExpressions.size());
            std::transform(
                argumentExpressions.begin(), 
                argumentExpressions.end(), 
                argumentVars.begin(),
                [](std::shared_ptr<Expression> argumentExpression) {
                    return argumentExpression->subTypes().at("name");
                } 
            );

            if (functionName == "+") {
                return generateAdd(variableStack, indent, {argumentVars.at(0), argumentVars.at(1)}, outputVarName);
            } else {
                throw std::runtime_error("Not implemented.");
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
    }

    std::string X86AssemblyGenerator::generate(std::vector<TIRCommand> irCommands)
    {
        return this->_assemblyGenerator.generate(irCommands);
    }
}
