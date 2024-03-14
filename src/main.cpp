#include <iostream>
#include <fstream>
#include <cassert>
#include <array>
#include "my-language/tokenizer/Tokenizer.h"
#include "my-language/parser/Parser.h"
#include "my-language/ir-generator/ModuleIRGenerator.h"
#include "my-language/assembly-generator/X86AssemblyGenerator.h"
#include "my-language/type-checker/TypeChecker.h"

int main(int argc, char* argv[]) {
    assert(argc == 3);

    // Create compiler parts.
    auto tokenizer = Tokenizer{};
    auto parser = MyLanguage::Parser{};
    auto typeChecker = MyLanguage::TypeChecker{};
    auto irGenerator = MyLanguage::ModuleIRGenerator{};
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
    typeChecker.check(root);
    auto irCommands = irGenerator.generate(root);
    auto assemblyCode = assemblyGenerator.generate(irCommands);

    // Write the compiled assembly into a file.
    std::ofstream outputFile;
    outputFile.open(argv[2]);
    outputFile << assemblyCode;
    outputFile.flush();

    // Generate object file from program assembly code.
    auto assembleProgramCommand = std::string{"as -g -o files/_program.o "} + argv[2];
    system(assembleProgramCommand.c_str());
    // Generate object file from the standard library we want to link to.
    system("as -g -o files/_stdlib.o files/_stdlib.s");

    // Link the object files to produce the final executable.
    #if _WIN32
        system("ld -o program.exe -static files/_program.o files/_stdlib.o");
    #else
        system("ld -o program.out -static files/_program.o files/_stdlib.o");
    #endif
}