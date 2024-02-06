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
class MapParser: public IParseable {

    public:
        MapParser();
        std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        std::shared_ptr<Expression> parseWith(std::vector<DToken>& tokens, std::string tokenType, int position);
        std::map<std::string, TExpressions::ExpressionConstructor>& expressionConstructors();

    private:
        std::map<std::string, TExpressions::ExpressionConstructor> _expressionConstructors;
};

#endif