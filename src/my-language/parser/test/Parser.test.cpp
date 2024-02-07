#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "../../../libraries/catch.h"
#include "../Parser.h"
#include "../../tokenizer/Tokenizer.h"

namespace Test {
    auto parser = MyLanguage::Parser{};
    auto tokenizer = Tokenizer{};
}

TEST_CASE("Recursive boolean expression", "[Parser::parse]") {
    auto input = "aaa and bbb and not ccc";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    std::cout << parseTree->children().at(1)->type() << std::endl;

    REQUIRE(parseTree->type() == "binary-operator");
    REQUIRE(parseTree->children().at(0)->type() == "binary-operator");
    REQUIRE(parseTree->children().at(0)->children().at(0)->type() == "identifier");
    REQUIRE(parseTree->children().at(0)->children().at(1)->type() == "identifier");
    REQUIRE(parseTree->children().at(1)->type() == "unary-operator");
    REQUIRE(parseTree->children().at(1)->children().at(0)->type() == "identifier");
}