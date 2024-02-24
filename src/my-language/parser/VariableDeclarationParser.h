#ifndef MY_LANGUAGE_VARIABLE_DECLARATION_PARSER_HH
#define MY_LANGUAGE_VARIABLE_DECLARATION_PARSER_HH

#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/OperatedChainParser.h"

namespace MyLanguage {
    class VariableDeclarationParser: public IParseable {
        public:
            VariableDeclarationParser(OperatedChainParser* operatedChainParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::unique_ptr<Parsing::SkeletonParser> _skeletonParser;
    };
};

#endif