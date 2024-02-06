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

std::shared_ptr<MapParser> createMapParser(std::vector<DToken>& tokens) {
    auto mapParser = std::shared_ptr<MapParser>(new MapParser{});

    mapParser->expressionConstructors().insert(
        std::pair<std::string, TExpressions::ExpressionConstructor>(
            "identifier", 
            []() {
                return std::shared_ptr<IParseable>(new LiteralParser{"identifier"});
            }
        )
    );

    mapParser->expressionConstructors().insert(
        std::pair<std::string, TExpressions::ExpressionConstructor>(
            "AND",
            [mapParser]() {
                return std::shared_ptr<IParseable>(new BinaryParser{"AND", *mapParser});
            }
        )
    );
    
    mapParser->expressionConstructors().insert(
        std::pair<std::string, TExpressions::ExpressionConstructor>(
            "NOT",
            [mapParser]() {
                return std::shared_ptr<IParseable>(
                    new UnaryParser{
                            "NOT",
                            [mapParser]() {
                                return mapParser;
                            }
                    }
                );
            }
        )
    );

    return mapParser;
}

TEST_CASE("simple binary expression", "[parse]") {
    auto tokens = std::vector<DToken>{
        DToken{"identifier", "abc", 0, 3},
        DToken{"AND", "AND", 3, 6},
        DToken{"identifier", "efg", 6, 9},
    };

    auto mapParser = createMapParser(tokens);

    auto chain = OperatedChainParser{*mapParser, std::set<std::string>{"AND"}};
    auto parseTree = chain.parse(tokens, 0);
    
    REQUIRE(parseTree->type == "AND");
    REQUIRE(parseTree->children.size() == 2);
    REQUIRE(parseTree->children.at(0)->type == "identifier");
    REQUIRE(parseTree->children.at(1)->type == "identifier");
}

TEST_CASE("combination of chained unary operators and binary operators", "[parse]") {
    auto tokens = std::vector<DToken>{
        DToken{"NOT", "NOT", 0, 1},
        DToken{"NOT", "NOT", 1, 2},
        DToken{"identifier", "abc", 2, 5},
        DToken{"AND", "AND", -1, -1},
        DToken{"NOT", "NOT", -1, -1},
        DToken{"NOT", "NOT", -1, -1},
        DToken{"identifier", "efg", -1, -1},
    };

    auto mapParser = createMapParser(tokens);

    auto chain = OperatedChainParser{*mapParser, std::set<std::string>{"AND"}};
    auto parseTree = chain.parse(tokens, 0);

    REQUIRE(parseTree->type == "AND");
    REQUIRE(parseTree->children.at(0)->type == "NOT");
    REQUIRE(parseTree->children.at(1)->type == "NOT");
    REQUIRE(parseTree->children.at(0)->children.at(0)->type == "NOT");
    REQUIRE(parseTree->children.at(1)->children.at(0)->type == "NOT");
    REQUIRE(parseTree->children.at(0)->children.at(0)->children.at(0)->type == "identifier");
    REQUIRE(parseTree->children.at(1)->children.at(0)->children.at(0)->type == "identifier");
}