#ifndef MY_LANGUAGE_VARIABLE_DECLARATION_PARSER_HH
#define MY_LANGUAGE_VARIABLE_DECLARATION_PARSER_HH

#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/OperatedChainParser.h"
#include "../../components/parsing/ConflictParser.h"

namespace MyLanguage {
    class VariableDeclarationParser: public IParseable {
        public:
            VariableDeclarationParser(OperatedChainParser* operatedChainParser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::shared_ptr<Parsing::SkeletonParser> _skeletonParser;
            std::shared_ptr<Parsing::SkeletonParser> _typedSkeletonParser;
            std::shared_ptr<Parsing::ConflictParser> _conflictParser;
            std::shared_ptr<Parsing::SkeletonParser> _createSkeletonParser(
                std::vector<std::pair<std::string, std::string>> pattern,
                OperatedChainParser* operatedChainParser
            );
    };
};

#endif