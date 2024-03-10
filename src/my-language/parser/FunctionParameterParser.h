#ifndef MY_LANGUAGE_PARSER_FUNCTION_PARAMETER_PARSER_HH
#define MY_LANGUAGE_PARSER_FUNCTION_PARAMETER_PARSER_HH

#include "../../components/parsing/IParseable.h"
#include "../../components/tokenization/DToken.h"
#include "../../components/parsing/TokenSequence.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "../../components/parsing/ChainParser.h"
#include "../../components/parsing/ListParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/MapParser.h"
#include "../../components/parsing/LiteralParser.h"
#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <cassert>

namespace MyLanguage {
    /**
     * Parser for a function parameter.
     */
    class FunctionParameterParser: public IParseable {
        public:
            FunctionParameterParser();
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::unique_ptr<LiteralParser> _identifierParser;
            std::unique_ptr<Parsing::SkeletonParser> _skeletonParser;
    };
}

#endif