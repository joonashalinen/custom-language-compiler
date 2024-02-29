#ifndef PARSING_UNARY_EXPRESSION_HH
#define PARSING_UNARY_EXPRESSION_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <functional>
#include <set>
#include "TExpressions.h"
#include <iostream>

/**
 * An expression that has a unary operator in front of it.
 */
class UnaryParser: public IParseable {
    public:
        UnaryParser(
            std::string operatorType,
            std::set<std::string> acceptableOperators,
            IParseable& expressionParser
        );
        std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
    private:
        std::string _operatorType;
        IParseable& _expressionParser;
        std::set<std::string> _acceptableOperators;
};

#endif