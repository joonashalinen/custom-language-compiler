#ifndef PARSING_EXPRESSION_CHAIN_HH
#define PARSING_EXPRESSION_CHAIN_HH

#include "IParseable.h"
#include <vector>
#include <functional>
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include "IParseable.h"
#include "MapParser.h"
#include <set>

/**
 * A parser for an expression that consists of 
 * consecutive (possibly recursive) sub-expressions.
 * These sub-expressions may have unary operators in front of them 
 * or they may be joined via binary operators.
 */
class OperatedChainParser: public IParseable {
    public:
        OperatedChainParser(
            MapParser& expressions,
            std::set<std::string> nonUnaryOperators,
            std::map<std::string, int> precedenceLevels = std::map<std::string, int>()
        );
        std::shared_ptr<DExpression> parse(std::vector<DToken>& tokens, int position);
        int precedenceLevel(std::string expressionType);
    private:
        MapParser& _expressions;
        std::set<std::string> _nonUnaryOperators;
        std::map<std::string, int> _precedenceLevels;
        std::shared_ptr<DExpression> _firstHigherPrecedenceLeftChild(std::shared_ptr<DExpression> expression, int precedence);
};

#endif