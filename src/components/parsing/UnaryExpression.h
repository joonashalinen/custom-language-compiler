#ifndef PARSING_UNARY_EXPRESSION_HH
#define PARSING_UNARY_EXPRESSION_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <functional>
#include "TExpressions.h"

/**
 * An expression that has a unary operator in front of it.
 */
class UnaryExpression: public IParseable {
    public:
        UnaryExpression(
            std::vector<DToken<>>& tokens,
            std::string operatorType,
            TExpressions::ExpressionConstructor makeExpression
        );
        std::shared_ptr<DExpression> parse(int position);
    private:
        std::string _operatorType;
        std::vector<DToken<>>& _tokens;
        TokenSequence _tokenSequence;
        TExpressions::ExpressionConstructor _makeExpression;
};

#endif