#ifndef MY_LANGAUGE_PARSER_CHAIN_PARSER_HH
#define MY_LANGAUGE_PARSER_CHAIN_PARSER_HH

#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/ChainParser.h"

namespace MyLanguage {
    class ChainParser: public IParseable {
        public:
            ChainParser(MapParser* mapParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::unique_ptr<Parsing::ChainParser> _chainParser;
            std::unique_ptr<MapParser> _mapParser;
    };
};

#endif