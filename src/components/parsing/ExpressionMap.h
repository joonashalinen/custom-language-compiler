#ifndef PARSING_EXPRESSION_MAP_HH
#define PARSING_EXPRESSION_MAP_HH

#include <map>
#include <memory>
#include <string>
#include "IParseable.h"
#include <functional>
#include <vector>
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include "TExpressions.h"

/**
 * An expression that can be one of several expression 
 * types depending on what the next token is.
 */
class ExpressionMap: public IParseable {

    public:
        ExpressionMap(
            std::vector<DToken>& tokens
        );
        std::shared_ptr<DExpression> parse(int position);
        std::shared_ptr<DExpression> parseWith(std::string tokenType, int position);
        std::map<std::string, TExpressions::ExpressionConstructor>& expressionConstructors();

    private:
        TokenSequence _tokenSequence;
        std::map<std::string, TExpressions::ExpressionConstructor> _expressionConstructors;
        std::vector<DToken>& _tokens;
};

#endif