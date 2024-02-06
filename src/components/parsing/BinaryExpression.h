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
            std::string operatorType,
            ExpressionMap& expressionMap
        );
        std::shared_ptr<DExpression> parse(std::vector<DToken>& tokens, int position);
    private:
        std::string _operatorType;
        ExpressionMap& _expressionMap;
};

#endif