#ifndef PARSING_BLOCK_PARSER_HH
#define PARSING_BLOCK_PARSER_HH

#include "ParentheticalParser.h"
#include "TokenSequence.h"

namespace Parsing {
    class BlockParser: public IParseable {
        public:
            BlockParser(ParentheticalParser& parser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            ParentheticalParser& _parser;
    };
};

#endif