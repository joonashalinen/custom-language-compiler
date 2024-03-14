#ifndef MY_LANGUAGE_PARSER_HH
#define MY_LANGUAGE_PARSER_HH

#include "../../components/parsing/IParseable.h"
#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/LiteralParser.h"
#include "../../components/parsing/BinaryParser.h"
#include "../../components/parsing/UnaryParser.h"
#include "../../components/parsing/ParentheticalParser.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "../../components/parsing/ConflictParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include "IfParser.h"
#include "WhileParser.h"
#include "ChainParser.h"
#include "FunctionCallParser.h"
#include "FunctionParser.h"
#include "ModuleParser.h"

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
            std::unique_ptr<LiteralParser> _identifierLiteralParser;
            std::unique_ptr<LiteralParser> _numberLiteralParser;
            std::unique_ptr<LiteralParser> _booleanLiteralParser;
            std::unique_ptr<Parsing::ConflictParser> _identifierParser;
            std::unique_ptr<Parsing::ParentheticalParser> _parentheticalParser;
            std::unique_ptr<Parsing::ParentheticalParser> _blockParser;
            std::unique_ptr<MyLanguage::ChainParser> _chainParser;
            std::unique_ptr<BinaryParser> _binaryParser;
            std::unique_ptr<UnaryParser> _unaryParser;
            std::unique_ptr<OperatedChainParser> _operatedChainParser;
            std::unique_ptr<MyLanguage::IfParser> _ifParser;
            std::unique_ptr<MyLanguage::WhileParser> _whileParser;
            std::unique_ptr<MyLanguage::FunctionCallParser> _functionCallParser;
            std::unique_ptr<MyLanguage::ModuleParser> _moduleParser;
    };
};

#endif