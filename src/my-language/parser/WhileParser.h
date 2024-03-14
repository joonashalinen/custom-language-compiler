#ifndef MY_LANGUAGE_WHILE_PARSER_HH
#define MY_LANGUAGE_WHILE_PARSER_HH

#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/LiteralParser.h"

namespace MyLanguage {
    class WhileParser: public IParseable {
        public:
            WhileParser(OperatedChainParser* operatedChainParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
            bool canParseAt(std::vector<DToken>& tokens, int position);
        private:
            OperatedChainParser* _operatedChainParser;
            std::unique_ptr<Parsing::SkeletonParser> _mainParser;
            std::unique_ptr<LiteralParser> _continueParser;
            std::unique_ptr<Parsing::SkeletonParser> _breakParser;
            /**
             * Undo changes made to sub-parsers.
             */
            void _restore();
    };
};

#endif