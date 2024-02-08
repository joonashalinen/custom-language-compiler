#ifndef PARSING_CHAIN_PARSER_HH
#define PARSING_CHAIN_PARSER_HH

#include <functional>
#include "IParseable.h"
#include "MapParser.h"
#include "TokenSequence.h"
#include "Expression.h"

namespace Parsing {
    class ChainParser: public IParseable {
        public:
            ChainParser(
                std::string type,
                std::string separator, 
                MapParser& parser,
                std::function<bool(std::vector<DToken>& tokens, int position)> separatorOptionalityRule
            );
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::string _type;
            std::string _separator;
            MapParser& _parser;
            std::function<bool(std::vector<DToken>& tokens, int position)> _separatorOptionalityRule;
    };
};

#endif