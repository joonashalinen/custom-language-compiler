#ifndef PARSING_TRAILING_PARSER_HH
#define PARSING_TRAILING_PARSER_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <functional>

class TrailingParser: public IParseable {
    public:
        TrailingParser(
            std::vector<std::string> types,
            
        );
        std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
    private:
        std::vector<std::string> _types;
};

#endif