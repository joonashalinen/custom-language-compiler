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

        std::string generateLoadBoolConst(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto booleanValue = command->children().at(0)->subTypes().at("value");
            auto zeroOrOne = booleanValue == "true" ? 1 : 0;
            auto variable = command->children().at(1)->subTypes().at("name");
            auto variableLocation = variableStack.negativeEndLocation(variable);
            return (
                indent + "movq " + "$" + std::to_string(zeroOrOne) + ", " + std::to_string(variableLocation) + "(%rbp)" + "\n"
            );
        }

        std::string generateNumericBinaryOperator(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            std::pair<std::string, std::string> summands,
            std::string outputVar,
            std::string operation
        ) {
            auto firstVarLocation = variableStack.negativeEndLocation(summands.first);
            auto secondVarLocation = variableStack.negativeEndLocation(summands.second);
            auto outputVarLocation = variableStack.negativeEndLocation(outputVar);
            auto result = indent + "movq " + std::to_string(firstVarLocation) + "(%rbp)" + ", %rax" + "\n";
            if (operation == "+" || operation == "-" || operation == "*") {
                std::map<std::string, std::string> operationInstructions{
                    {"+", "addq"},
                    {"-", "subq"},
                    {"*", "imulq"}
                };
                auto operationInstruction = operationInstructions.at(operation);
                result = result + indent + operationInstruction + " " + std::to_string(secondVarLocation) + "(%rbp)" + ", %rax" + "\n";
            } else {
                result = result + (
                    indent + "cqto\n" + 
                    indent + "idivq" + " " + std::to_string(secondVarLocation) + "(%rbp)" + "\n"
                );
                if (operation == "%") {
                    result = result + indent + "movq " + "%rdx, " + "%rax" + "\n";
                }
            }
            result = result + indent + "movq " + "%rax, " + std::to_string(outputVarLocation) + "(%rbp)" + "\n";
            return result;
        }

        std::string generateComparison(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            std::pair<std::string, std::string> comparedVariables,
            std::string outputVar,
            std::string operation
        ) {
            std::map<std::string, std::string> instructions{
                {"<=", "setle"},
                {"<", "setl"},
                {">", "setg"},
                {">=", "setge"},
                {"==", "sete"},
                {"!=", "setne"}
            };
            auto firstVarLocation = variableStack.negativeEndLocation(comparedVariables.first);
            auto secondVarLocation = variableStack.negativeEndLocation(comparedVariables.second);
            auto outputVarLocation = variableStack.negativeEndLocation(outputVar);
            return (
                indent + "xor %rax, %rax" + "\n" +
                indent + "movq " + std::to_string(firstVarLocation) + "(%rbp)" + ", %rdx" + "\n" +
                indent + "cmpq " + std::to_string(secondVarLocation) + "(%rbp)" + ", %rdx" + "\n" +
                indent + instructions.at(operation) + " %al" "\n" +
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

        std::string generateCondJump(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto conditionVariable = command->extractChildSubTypeValues("variable", "name").at(0);
            auto conditionVariableLocation = variableStack.negativeEndLocation(conditionVariable);
            auto labels = command->extractChildSubTypeValues("label", "name");
            return (
                indent + "cmpq $0, " + std::to_string(conditionVariableLocation) + "(%rbp)" + "\n" +
                indent + "jne ." + labels.at(0) + "\n" + 
                indent + "jmp ." + labels.at(1) + "\n"
            );
        }

        std::string generateJump(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto label = command->children().at(0)->subTypes().at("name");
            return (
                indent + "jmp ." + label + "\n"
            );
        }

        std::string generateLabel(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto label = command->children().at(0)->subTypes().at("name");
            return (
                "." + label + ":" + "\n"
            );
        }

        std::string generateFunctionLabel(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            // Make sure we reserve an amount of memory divisible by 16 to 
            // ensure the stack is initially aligned to 16 bytes.
            auto reserveMemoryAmount = (
                variableStack.size() % 16 == 0 ? 
                    variableStack.size() : 
                    variableStack.size() + 8
            );

            auto label = command->children().at(0)->subTypes().at("name");
            return (
                "" + label + ":" + "\n" + 
                indent + "pushq %rbp" + "\n" + 
                indent + "movq %rsp, %rbp" + "\n" +
                indent + "subq $" + std::to_string(reserveMemoryAmount) + ", %rsp" + "\n"
            );
        }

        std::string generateUnaryOperator(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            std::string inputVar,
            std::string outputVar,
            std::string unaryOperator
        ) {
            auto inputVarLocation = variableStack.negativeEndLocation(inputVar);
            auto outputVarLocation = variableStack.negativeEndLocation(outputVar);
            if (unaryOperator != "&") {
                auto operatorAssemblies = std::map<std::string, std::string>{
                    {"-", "negq %rax"},
                    {"not", "xorq $1, %rax"},
                    {"*", "movq (%rax), %rax"}
                };
                return (
                    indent + "movq " + std::to_string(inputVarLocation) + "(%rbp), %rax" + "\n" + 
                    indent + operatorAssemblies.at(unaryOperator) + "\n" +
                    indent + "movq %rax, " + std::to_string(outputVarLocation) + "(%rbp)" + "\n"
                );                
            } else {
               return (
                    indent + "movq %rbp, %rax" + "\n" + 
                    indent + "addq $" + std::to_string(inputVarLocation) + ", %rax" + "\n" + 
                    indent + "movq %rax, " + std::to_string(outputVarLocation) + "(%rbp)" + "\n"
                );
            }
        }

        std::string generateFunctionCall(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            std::string functionName,
            std::vector<std::string> argumentVars,
            std::string outputVar
        ) {
            assert(argumentVars.size() <= 6); // Functions can only accept up to six arguments.

            auto paramRegisters = std::vector<std::string>{"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            auto storeArguments = std::string("");
            for (int i = 0; i < ((int) argumentVars.size()); i++) {
                auto argLocation = variableStack.negativeEndLocation(argumentVars.at(i));
                storeArguments += indent + "movq " + std::to_string(argLocation) + "(%rbp)" + ", " + paramRegisters.at(i) + "\n";
            }

            auto outputLocation = variableStack.negativeEndLocation(outputVar);
            return (
                storeArguments +
                indent + "call " + functionName + "\n" +
                indent + "movq " + "%rax, " + std::to_string(outputLocation) + "(%rbp)" + "\n"
            );
        }

        std::string generateWriteFunctionReturn(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto variableName = command->children().at(0)->subTypes().at("name");
            auto variableLocation = variableStack.negativeEndLocation(variableName);
            return (
                indent + "movq " + std::to_string(variableLocation) + "(%rbp)" + ", %rax" + "\n" +
                indent + "movq %rbp, %rsp" + "\n" + 
                indent + "popq %rbp" + "\n" + 
                indent + "ret" + "\n"
            );
        }
        
        std::string generateLoadFunctionParam(
            StructuredLanguage::VariableStack& variableStack, 
            std::string indent,
            TIRCommand command
        ) {
            auto paramRegisters = std::vector<std::string>{"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            auto variableName = command->children().at(0)->subTypes().at("name");
            auto variableLocation = variableStack.negativeEndLocation(variableName);
            int paramIndex = std::stoi(command->subTypes().at("index"));
            return (
                indent + "movq " + paramRegisters.at(paramIndex) + ", " + std::to_string(variableLocation) + "(%rbp)" + "\n"
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

            if (argumentVars.size() == 1 && (std::set<std::string>{"-", "not", "*", "&"}).contains(functionName)) {
                return generateUnaryOperator(variableStack, indent, argumentVars.at(0), outputVarName, functionName);
            } else if ((std::set<std::string>{"+", "-", "*", "/", "%"}).contains(functionName)) {
                return generateNumericBinaryOperator(
                    variableStack, 
                    indent, 
                    {argumentVars.at(0), argumentVars.at(1)}, 
                    outputVarName, 
                    functionName
                );
            } else if ((std::set<std::string>{"<", "<=", ">", ">=", "!=", "=="}).contains(functionName)) {
                return generateComparison(
                    variableStack, 
                    indent, 
                    {argumentVars.at(0), argumentVars.at(1)}, 
                    outputVarName, 
                    functionName
                );
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
            indent + "\n"
        );

        // Configure assembly generating functions for each IR command type.
        this->_assemblyGenerator.generators().insert({"LoadIntConst", X86AssemblyGenerators::generateLoadIntConst});
        this->_assemblyGenerator.generators().insert({"LoadBoolConst", X86AssemblyGenerators::generateLoadBoolConst});
        this->_assemblyGenerator.generators().insert({"CondJump", X86AssemblyGenerators::generateCondJump});
        this->_assemblyGenerator.generators().insert({"Jump", X86AssemblyGenerators::generateJump});
        this->_assemblyGenerator.generators().insert({"Label", X86AssemblyGenerators::generateLabel});
        this->_assemblyGenerator.generators().insert({"FunctionLabel", X86AssemblyGenerators::generateFunctionLabel});
        this->_assemblyGenerator.generators().insert({"Call", X86AssemblyGenerators::generateCall});
        this->_assemblyGenerator.generators().insert({"Copy", X86AssemblyGenerators::generateCopy});
        this->_assemblyGenerator.generators().insert({"WriteFunctionReturn", X86AssemblyGenerators::generateWriteFunctionReturn});
        this->_assemblyGenerator.generators().insert({"LoadFunctionParam", X86AssemblyGenerators::generateLoadFunctionParam});
    }

    std::string X86AssemblyGenerator::generate(std::map<std::string, std::vector<TIRCommand>> irCommands)
    {
        return this->_assemblyGenerator.generate(irCommands);
    }
}
