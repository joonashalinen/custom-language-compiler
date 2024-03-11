#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../../../libraries/doctest.h"
#include "../X86AssemblyGenerator.h"
#include "../../ir-generator/ModuleIRGenerator.h"
#include "../../parser/Parser.h"
#include "../../tokenizer/Tokenizer.h"
#include <iostream>

namespace Test {
    auto parser = MyLanguage::Parser{};
    auto tokenizer = Tokenizer{};
    auto irGenerator = MyLanguage::ModuleIRGenerator{};
    auto assemblyGenerator = MyLanguage::X86AssemblyGenerator{};
}

TEST_CASE("generate") {
    auto input = "while true do break;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);
    auto root = Test::parser.parse(tokens, 0);
    auto irCommands = Test::irGenerator.generate(root);
    auto assembly = Test::assemblyGenerator.generate(irCommands);

    std::cout << assembly << std::endl;
}