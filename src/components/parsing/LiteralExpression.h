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
class LiteralExpression: public IParseable {
    public:
        LiteralExpression(
            std::vector<DToken<>>& tokens,
            std::string type
        );
        std::shared_ptr<DExpression> parse(int position);
    private:
        std::string _type;
        std::vector<DToken<>>& _tokens;
        TokenSequence _tokenSequence;
};

#endif