#ifndef MY_LANGUAGE_PARSER_FUNCTION_PARSER_HH
#define MY_LANGUAGE_PARSER_FUNCTION_PARSER_HH

#include "../../components/parsing/IParseable.h"
#include "../../components/tokenization/DToken.h"
#include "../../components/parsing/TokenSequence.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "../../components/parsing/ChainParser.h"
#include "../../components/parsing/ListParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/MapParser.h"
#include <string>
#include <functional>
#include <vector>

namespace MyLanguage {
    /**
     * Parser for a function definition.
     */
    class FunctionParser: public IParseable {
        public:
            FunctionParser(
                IParseable* identifierParser,  
                IParseable* baseStatementParser
            );
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            IParseable* _identifierParser;
            std::unique_ptr<Parsing::SkeletonParser> _parameterParser;
            std::unique_ptr<Parsing::ListParser> _parameterListParser;
            std::unique_ptr<Parsing::SkeletonParser> _parser;
            std::unique_ptr<Parsing::SkeletonParser> _returnParser;
            std::unique_ptr<Parsing::ChainParser> _definitionParser;
            std::unique_ptr<MapParser> _statementParser;
    };
}

#endif