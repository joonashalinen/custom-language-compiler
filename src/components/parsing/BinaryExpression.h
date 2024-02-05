#ifndef PARSING_BINARY_EXPRESSION_HH
#define PARSING_BINARY_EXPRESSION_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <functional>
#include "TExpressions.h"
#include "ExpressionMap.h"

/**
 * An expression that consists of two sub-expressions with a 
 * binary operator between them.
 */
class BinaryExpression: public IParseable {
    public:
        BinaryExpression(
            std::vector<DToken<>>& tokens,
            std::string operatorType,
            ExpressionMap& expressionMap
        );
        std::shared_ptr<DExpression> parse(int position);
    private:
        std::string _operatorType;
        std::vector<DToken<>>& _tokens;
        TokenSequence _tokenSequence;
        ExpressionMap& _expressionMap;
};

#endif