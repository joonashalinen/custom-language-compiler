#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../../../libraries/doctest.h"
#include "../IRGenerator.h"
#include "../../parser/Parser.h"
#include "../../tokenizer/Tokenizer.h"

namespace Test {
    auto parser = MyLanguage::Parser{};
    auto tokenizer = Tokenizer{};
    auto generator = MyLanguage::IRGenerator{};
}

TEST_CASE("generate") {
    auto input = "1 + 2 * 2;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);
    auto root = Test::parser.parse(tokens, 0);
    auto irCommands = Test::generator.generate(root);

    std::for_each(irCommands.begin(), irCommands.end(), [](MyLanguage::IRCommand command) {
        std::cout << command.toString() << std::endl;
    });
}