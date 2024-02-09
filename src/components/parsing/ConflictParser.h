#ifndef PARSING_CONFLICT_PARSER_HH
#define PARSING_CONFLICT_PARSER_HH

#include "IParseable.h"
#include "../tokenization/DToken.h"
#include "TokenSequence.h"
#include <string>
#include <algorithm>

namespace Parsing {
    /**
     * Parser that decides which rule to use when an expression 
     * may be one of several expression types when looking at just the 
     * first token. The parser selects the longest rule that parses successfully.
     */
    class ConflictParser: public IParseable {
        public:
            ConflictParser(
                std::vector<IParseable*> parsers
            );
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::vector<IParseable*> _parsers;
    };
}

#endif