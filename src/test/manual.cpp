#include "../my-language/assembly-generator/X86AssemblyGenerator.h"
#include "../my-language/ir-generator/ModuleIRGenerator.h"
#include "../my-language/type-checker/TypeChecker.h"
#include "../my-language/parser/Parser.h"
#include "../my-language/tokenizer/Tokenizer.h"
#include <iostream>

namespace Test {
    auto parser = MyLanguage::Parser{};
    auto tokenizer = Tokenizer{};
    auto irGenerator = MyLanguage::ModuleIRGenerator{};
    auto typeChecker = MyLanguage::TypeChecker{};
    auto assemblyGenerator = MyLanguage::X86AssemblyGenerator{};
}

int main(int argc, char* argv[]) {
    auto input = "2 * 3;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);
    auto root = Test::parser.parse(tokens, 0);
    Test::typeChecker.check(root);
    auto irCommands = Test::irGenerator.generate(root);
    auto assembly = Test::assemblyGenerator.generate(irCommands);

    std::cout << assembly << std::endl;
}