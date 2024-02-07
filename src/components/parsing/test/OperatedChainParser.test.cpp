#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "../../../libraries/catch.h"
#include "../OperatedChainParser.h"
#include "../MapParser.h"
#include "../LiteralParser.h"
#include "../UnaryParser.h"
#include "../BinaryParser.h"
#include <vector>
#include <memory>

namespace Test {
    auto mapParser = std::unique_ptr<MapParser>(new MapParser{});
    auto literalParser = std::unique_ptr<IParseable>(new LiteralParser{"identifier"});
    auto binaryParser = std::unique_ptr<IParseable>(new BinaryParser{"AND", *mapParser});
    auto unaryParser = std::unique_ptr<IParseable>(new UnaryParser{"NOT", *mapParser});
    auto operatedChainParser = std::unique_ptr<IParseable>(new OperatedChainParser{*mapParser, std::set<std::string>{"AND"}});
};

void beforeEach() {
    Test::mapParser->setParsers(
        std::map<std::string, IParseable*>{
            {"identifier", Test::literalParser.get()},
            {"AND", Test::binaryParser.get()},
            {"NOT", Test::unaryParser.get()}
        }
    );
}

TEST_CASE("simple binary expression", "[parse]") {
    beforeEach();

    auto tokens = std::vector<DToken>{
        DToken{"identifier", "abc", 0, 3},
        DToken{"AND", "AND", 3, 6},
        DToken{"identifier", "efg", 6, 9},
    };

    auto parseTree = Test::operatedChainParser->parse(tokens, 0);
    
    REQUIRE(parseTree->type() == "AND");
    REQUIRE(parseTree->children().size() == 2);
    REQUIRE(parseTree->children().at(0)->type() == "identifier");
    REQUIRE(parseTree->children().at(1)->type() == "identifier");
}

TEST_CASE("combination of chained unary operators and binary operators", "[parse]") {
    beforeEach();

    auto tokens = std::vector<DToken>{
        DToken{"NOT", "NOT", 0, 1},
        DToken{"NOT", "NOT", 1, 2},
        DToken{"identifier", "abc", 2, 5},
        DToken{"AND", "AND", -1, -1},
        DToken{"NOT", "NOT", -1, -1},
        DToken{"NOT", "NOT", -1, -1},
        DToken{"identifier", "efg", -1, -1},
    };

    auto parseTree = Test::operatedChainParser->parse(tokens, 0);

    REQUIRE(parseTree->type() == "AND");
    REQUIRE(parseTree->children().at(0)->type() == "NOT");
    REQUIRE(parseTree->children().at(1)->type() == "NOT");
    REQUIRE(parseTree->children().at(0)->children().at(0)->type() == "NOT");
    REQUIRE(parseTree->children().at(1)->children().at(0)->type() == "NOT");
    REQUIRE(parseTree->children().at(0)->children().at(0)->children().at(0)->type() == "identifier");
    REQUIRE(parseTree->children().at(1)->children().at(0)->children().at(0)->type() == "identifier");
}