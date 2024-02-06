#ifndef PARSING_EXPRESSION_CHAIN_HH
#define PARSING_EXPRESSION_CHAIN_HH

#include "IParseable.h"
#include <vector>
#include <functional>
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include "IParseable.h"
#include "ExpressionMap.h"
#include <set>

/**
 * An expression that consists of consecutive (possibly recursive) sub-expressions.
 * These sub-expressions may have unary operators in front of them 
 * or they may be joined via binary operators.
 */
class ExpressionChain: public IParseable {
    public:
        ExpressionChain(
            std::vector<DToken>& tokens,
            ExpressionMap& expressions,
            std::set<std::string> nonUnaryOperators
        );
        std::shared_ptr<DExpression> parse(int position);
    private:
        std::vector<DToken>& _tokens;
        TokenSequence _tokenSequence;
        ExpressionMap& _expressions;
        std::set<std::string> _nonUnaryOperators;
};

#endif