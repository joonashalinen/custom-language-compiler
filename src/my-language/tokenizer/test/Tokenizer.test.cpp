#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "../../../libraries/catch.h"
#include "../Tokenizer.h"
#include "../../../components/tokenization/Tokenization.h"

TEST_CASE( "sdf", "[tokenize]" ) {
    auto tokenizer = Tokenizer{Tokenization::Tokenizer{}};
    auto tokens = tokenizer.tokenizer.tokenize("// test comment\n   if        ");
    REQUIRE( tokens.size() == 2 );
}