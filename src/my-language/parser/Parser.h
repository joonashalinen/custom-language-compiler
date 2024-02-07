#ifndef MY_LANGUAGE_PARSER_HH
#define MY_LANGUAGE_PARSER_HH

#include "../../components/parsing/IParseable.h"
#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/LiteralParser.h"
#include "../../components/parsing/BinaryParser.h"
#include "../../components/parsing/UnaryParser.h"
#include "../../components/parsing/ParentheticalParser.h"
#include "../../components/parsing/OperatedChainParser.h"

namespace MyLanguage {
    /**
     * The parser for our custom programming language.
     */
    class Parser: public IParseable {
        public:
            Parser();
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::unique_ptr<MapParser> _mapParser;
            std::unique_ptr<LiteralParser> _literalParser;
            std::unique_ptr<Parsing::ParentheticalParser> _parentheticalParser;
            std::unique_ptr<BinaryParser> _binaryParser;
            std::unique_ptr<UnaryParser> _unaryParser;
            std::unique_ptr<OperatedChainParser> _topLevelParser;
    };
};

#endif