#ifndef PARSING_LITERAL_EXPRESSION_HH
#define PARSING_LITERAL_EXPRESSION_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <functional>
#include "TExpressions.h"

/**
 * A literal expression, i.e. an expression that does not consist 
 * of sub-expressions.
 */
class LiteralParser: public IParseable {
    public:
        LiteralParser(
            std::string type
        );
        std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
    private:
        std::string _type;
};

#endif