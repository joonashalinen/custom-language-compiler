#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "../../../libraries/catch.h"
#include "../ExpressionChain.h"
#include "../ExpressionMap.h"
#include "../LiteralExpression.h"
#include "../UnaryExpression.h"
#include "../BinaryExpression.h"
#include <vector>
#include <memory>



std::shared_ptr<ExpressionMap> createExpressionMap(std::vector<DToken>& tokens) {
    auto expressionMap = std::shared_ptr<ExpressionMap>(new ExpressionMap{});

    expressionMap->expressionConstructors().insert(
        std::pair<std::string, TExpressions::ExpressionConstructor>(
            "identifier", 
            []() {
                return std::shared_ptr<IParseable>(new LiteralExpression{"identifier"});
            }
        )
    );

    expressionMap->expressionConstructors().insert(
        std::pair<std::string, TExpressions::ExpressionConstructor>(
            "AND",
            [expressionMap]() {
                return std::shared_ptr<IParseable>(new BinaryExpression{"AND", *expressionMap});
            }
        )
    );
    
    expressionMap->expressionConstructors().insert(
        std::pair<std::string, TExpressions::ExpressionConstructor>(
            "NOT",
            [expressionMap]() {
                return std::shared_ptr<IParseable>(
                    new UnaryExpression{
                            "NOT",
                            [expressionMap]() {
                                return expressionMap;
                            }
                    }
                );
            }
        )
    );

    return expressionMap;
}

TEST_CASE("simple binary expression", "[parse]") {
    auto tokens = std::vector<DToken>{
        DToken{"identifier", "abc", 0, 3},
        DToken{"AND", "AND", 3, 6},
        DToken{"identifier", "efg", 6, 9},
    };

    auto expressionMap = createExpressionMap(tokens);

    auto chain = ExpressionChain{*expressionMap, std::set<std::string>{"AND"}};
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

    auto expressionMap = createExpressionMap(tokens);

    auto chain = ExpressionChain{*expressionMap, std::set<std::string>{"AND"}};
    auto parseTree = chain.parse(tokens, 0);

    REQUIRE(parseTree->type == "AND");
    REQUIRE(parseTree->children.at(0)->type == "NOT");
    REQUIRE(parseTree->children.at(1)->type == "NOT");
    REQUIRE(parseTree->children.at(0)->children.at(0)->type == "NOT");
    REQUIRE(parseTree->children.at(1)->children.at(0)->type == "NOT");
    REQUIRE(parseTree->children.at(0)->children.at(0)->children.at(0)->type == "identifier");
    REQUIRE(parseTree->children.at(1)->children.at(0)->children.at(0)->type == "identifier");
}