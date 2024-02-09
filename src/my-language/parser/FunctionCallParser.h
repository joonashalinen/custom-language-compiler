#ifndef MY_LANGUAGE_PARSER_FUNCTION_CALL_PARSER_HH
#define MY_LANGUAGE_PARSER_FUNCTION_CALL_PARSER_HH

#include "../../components/parsing/IParseable.h"
#include "../../components/tokenization/DToken.h"
#include "../../components/parsing/TokenSequence.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "../../components/parsing/ListParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include <string>
#include <functional>
#include <vector>

namespace MyLanguage {
    /**
     * Parser for a function call of the form: ID(expr1, ... exprN).
     */
    class FunctionCallParser: public IParseable {
        public:
            FunctionCallParser(IParseable* identifierParser, MapParser& parameterParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            IParseable* _identifierParser;
            MapParser& _parameterParser;
            std::unique_ptr<Parsing::ListParser> _parameterListParser;
            std::unique_ptr<Parsing::SkeletonParser> _parser;
    };
}

#endif