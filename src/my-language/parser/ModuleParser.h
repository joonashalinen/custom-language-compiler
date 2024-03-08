#ifndef MY_LANGUAGE_MODULE_PARSER_HH
#define MY_LANGUAGE_MODULE_PARSER_HH

#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/LiteralParser.h"
#include "../../components/parsing/ChainParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "ChainParser.h"
#include "FunctionParser.h"

namespace MyLanguage {
    class ModuleParser: public IParseable {
        public:
            ModuleParser(IParseable* statementParser, IParseable* identifierParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::unique_ptr<Parsing::ChainParser> _moduleParser;
            std::unique_ptr<MapParser> _moduleStatementParser;
            std::unique_ptr<MyLanguage::FunctionParser> _functionParser;
    };
};

#endif