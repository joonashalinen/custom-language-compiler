#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "../../../libraries/catch.h"
#include "../Tokenizer.h"
#include "../../../components/tokenization/Tokenization.h"

TEST_CASE( "parentheses are recognized", "[tokenize]" ) {
    auto tokenizer = Tokenizer{Tokenization::Tokenizer{}};
    auto tokens = tokenizer.tokenizer.tokenize("//\n () ");
    
    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens.at(0).type == "parentheses");
    REQUIRE(tokens.at(0).value == "(");
    REQUIRE(tokens.at(0).startPos == 4);
    REQUIRE(tokens.at(0).endPos == 5);

    REQUIRE(tokens.at(1).type == "parentheses");
    REQUIRE(tokens.at(1).value == ")");
    REQUIRE(tokens.at(1).startPos == 5);
    REQUIRE(tokens.at(1).endPos == 6);

    REQUIRE(tokens.at(2).type == "end");
    REQUIRE(tokens.at(2).value == "");
    REQUIRE(tokens.at(2).startPos == 7);
    REQUIRE(tokens.at(2).endPos == 7);
}

TEST_CASE("identifier names can contain everything they should", "[tokenize]") {
    auto tokenizer = Tokenizer{Tokenization::Tokenizer{}};
    auto tokens = tokenizer.tokenizer.tokenize("//\n _aBzY3D- ");
    
    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens.at(0).type == "identifier");
    REQUIRE(tokens.at(0).value == "_aBzY3D-");
    REQUIRE(tokens.at(0).startPos == 4);
    REQUIRE(tokens.at(0).endPos == 12);
}

TEST_CASE("keywords are prioritized over identifiers", "[tokenize]") {
    auto tokenizer = Tokenizer{Tokenization::Tokenizer{}};
    auto tokens = tokenizer.tokenizer.tokenize("//\n if ");
    
    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens.at(0).type == "if");
    REQUIRE(tokens.at(0).value == "if");
    REQUIRE(tokens.at(0).startPos == 4);
    REQUIRE(tokens.at(0).endPos == 6);
}