#ifndef PARSING_BINARY_EXPRESSION_HH
#define PARSING_BINARY_EXPRESSION_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <functional>
#include "TExpressions.h"
#include "MapParser.h"

/**
 * An expression that consists of two sub-expressions with a 
 * binary operator between them.
 */
class BinaryParser: public IParseable {
    public:
        BinaryParser(
            std::string operatorType,
            MapParser& mapParser
        );
        std::shared_ptr<DExpression> parse(std::vector<DToken>& tokens, int position);
    private:
        std::string _operatorType;
        MapParser& _mapParser;
};

#endif