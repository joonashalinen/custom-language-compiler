#ifndef MY_LANGAUGE_PARSER_CHAIN_PARSER_HH
#define MY_LANGAUGE_PARSER_CHAIN_PARSER_HH

#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/ChainParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/OperatedChainParser.h"

namespace MyLanguage {
    class ChainParser: public IParseable {
        public:
            ChainParser(OperatedChainParser* operatedChainParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            OperatedChainParser* _operatedChainParser;
            std::unique_ptr<Parsing::ChainParser> _chainParser;
            std::unique_ptr<MapParser> _mapParser;
            std::unique_ptr<Parsing::SkeletonParser> _variableDeclarationParser;
    };
};

#endif