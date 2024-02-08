#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libraries/doctest.h"
#include "../Parser.h"
#include "../../tokenizer/Tokenizer.h"

namespace Test {
    auto parser = MyLanguage::Parser{};
    auto tokenizer = Tokenizer{};
}

TEST_CASE("Boolean expression without parentheses") {
    auto input = "a and b or c and not d";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->rootToken().value == "or");
    REQUIRE(parseTree->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(1)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(1)->children().at(0)->rootToken().value == "c");
    REQUIRE(parseTree->children().at(1)->children().at(1)->rootToken().value == "not");
    REQUIRE(parseTree->children().at(1)->children().at(1)->children().at(0)->rootToken().value == "d");
}

TEST_CASE("Boolean expression with parentheses") {
    auto input = "a and (b or c) and not d";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "(");
    REQUIRE(parseTree->children().at(0)->children().at(1)->children().at(0)->rootToken().value == "or");
    REQUIRE(parseTree->children().at(0)->children().at(1)->children().at(0)->children().at(0)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(0)->children().at(1)->children().at(0)->children().at(1)->rootToken().value == "c");
    REQUIRE(parseTree->children().at(1)->rootToken().value == "not");
    REQUIRE(parseTree->children().at(1)->children().at(0)->rootToken().value == "d");
}