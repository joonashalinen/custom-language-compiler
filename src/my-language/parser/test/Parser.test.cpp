#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libraries/doctest.h"
#include "../Parser.h"
#include "../../tokenizer/Tokenizer.h"

namespace Test {
    auto parser = MyLanguage::Parser{};
    auto tokenizer = Tokenizer{};
}

TEST_CASE("Boolean expression without inner parentheses") {
    auto input = "a and b or c and not d;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->rootToken().value == "or");
    REQUIRE(parseTree->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(1)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(1)->children().at(0)->rootToken().value == "c");
    REQUIRE(parseTree->children().at(1)->children().at(1)->rootToken().value == "not");
    REQUIRE(parseTree->children().at(1)->children().at(1)->children().at(0)->rootToken().value == "d");
}

TEST_CASE("Boolean expression with inner parentheses") {
    auto input = "a and (b or c) and not d;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

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

TEST_CASE("Nested block statement") {
    auto input = "{{a; b;} c}";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "block");
    REQUIRE(parseTree->children().at(0)->type() == "chain");
    REQUIRE(parseTree->children().at(0)->children().at(0)->type() == "block");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->type() == "chain");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->children().at(1)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "c");
}

TEST_CASE("Variable declaration in block") {
    auto input = "{var a = b and c;}";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "block");
    REQUIRE(parseTree->children().at(0)->type() == "chain");
    REQUIRE(parseTree->children().at(0)->children().at(0)->type() == "variable-declaration");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->children().at(0)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->children().at(1)->rootToken().value == "c");
}

TEST_CASE("If then else") {
    auto input = "if a and b then c else d;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    REQUIRE(parseTree->children().at(0)->type() == "if");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(1)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "c");
    REQUIRE(parseTree->children().at(0)->children().at(2)->rootToken().value == "d");
}

TEST_CASE("If then") {
    auto input = "if a and b then c;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    REQUIRE(parseTree->children().at(0)->type() == "if");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(1)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "c");
}

TEST_CASE("While do") {
    auto input = "while a and b do c;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    REQUIRE(parseTree->children().at(0)->type() == "while");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "and");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(1)->rootToken().value == "b");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "c");
}

TEST_CASE("Function call") {
    auto input = "f(a, b);";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "function-call");
    REQUIRE(parseTree->subTypes().at("name") == "f");
    REQUIRE(parseTree->children().size() == 2);
    REQUIRE(parseTree->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(1)->rootToken().value == "b");
}

TEST_CASE("Function call with inner operated chain expression") {
    auto input = "f(a + b);";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "function-call");
    REQUIRE(parseTree->subTypes().at("name") == "f");
    REQUIRE(parseTree->children().size() == 1);
    REQUIRE(parseTree->children().at(0)->rootToken().value == "+");
    REQUIRE(parseTree->children().at(0)->children().at(0)->rootToken().value == "a");
    REQUIRE(parseTree->children().at(0)->children().at(1)->rootToken().value == "b");
}

TEST_CASE("Unary minus expression") {
    auto input = "-1;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "unary-operator");
    REQUIRE(parseTree->subTypes().at("name") == "-");
    REQUIRE(parseTree->children().size() == 1);
    REQUIRE(parseTree->children().at(0)->rootToken().value == "1");
}

TEST_CASE("Subtraction") {
    auto input = "2 - 1;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "binary-operator");
    REQUIRE(parseTree->subTypes().at("name") == "-");
    REQUIRE(parseTree->children().size() == 2);
    REQUIRE(parseTree->children().at(0)->rootToken().value == "2");
    REQUIRE(parseTree->children().at(1)->rootToken().value == "1");
}

TEST_CASE("Variable declaration") {
    auto input = "var a = 1;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "variable-declaration");
    REQUIRE(parseTree->subTypes().at("name") == "a");
    REQUIRE(parseTree->children().size() == 1);
    REQUIRE(parseTree->children().at(0)->rootToken().value == "1");
}

TEST_CASE("Typed variable declaration") {
    auto input = "var a: Int = 1;";
    auto tokens = Test::tokenizer.tokenizer.tokenize(input);

    auto parseTree = Test::parser.parse(tokens, 0);

    REQUIRE(parseTree->type() == "chain");
    parseTree = parseTree->children().at(0);

    REQUIRE(parseTree->type() == "variable-declaration");
    REQUIRE(parseTree->subTypes().at("value-type") == "Int");
    REQUIRE(parseTree->subTypes().at("name") == "a");
    REQUIRE(parseTree->children().size() == 1);
    REQUIRE(parseTree->children().at(0)->rootToken().value == "1");
}