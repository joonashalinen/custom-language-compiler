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
            std::string operatorType,
            TExpressions::ExpressionConstructor makeExpression
        );
        std::shared_ptr<DExpression> parse(std::vector<DToken>& tokens, int position);
    private:
        std::string _operatorType;
        TExpressions::ExpressionConstructor _makeExpression;
};

#endif