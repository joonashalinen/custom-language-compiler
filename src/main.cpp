#include <iostream>
#include <fstream>
#include <cassert>
#include <array>
#include "my-language/tokenizer/Tokenizer.h"
#include "my-language/parser/Parser.h"
#include "my-language/ir-generator/IRGenerator.h"
#include "my-language/assembly-generator/X86AssemblyGenerator.h"

int main(int argc, char* argv[]) {
    assert(argc == 3);

    // Create compiler parts.
    auto tokenizer = Tokenizer{};
    auto parser = MyLanguage::Parser{};
    auto irGenerator = MyLanguage::IRGenerator{};
    auto assemblyGenerator = MyLanguage::X86AssemblyGenerator{};

    // Read input code file.
    auto codeFile = std::ifstream{argv[1]};
    auto code = std::string{};
    if (codeFile.is_open()) {
        while (codeFile.good()) {
            code.push_back(codeFile.get());
        }
    } else {
        throw std::runtime_error("Could not open input code file.");
    }

    if (code.size() < 2) {
        std::cout << "The input code file cannot be empty." << std::endl;
    } else {
        // Remove the end of file character.
        code.pop_back();
    }

    // Compile the code into assembly.
    auto tokens = tokenizer.tokenizer.tokenize(code);
    auto root = parser.parse(tokens, 0);
    auto irCommands = irGenerator.generate(root);
    auto assemblyCode = assemblyGenerator.generate(irCommands);

    // Write the compiled assembly into a file.
    std::ofstream outputFile;
    outputFile.open(argv[2]);
    outputFile << assemblyCode;
    outputFile.flush();

    // Run the assembler that will produce the final executable from the assembly code.
    auto runAssemblerCommand = std::string{"py run_assembler.py "} + argv[2] + " myprogram.exe";
    system(runAssemblerCommand.c_str());
}