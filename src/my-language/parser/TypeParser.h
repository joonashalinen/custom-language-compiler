#ifndef MY_LANGAUGE_PARSER_TYPE_PARSER_HH
#define MY_LANGAUGE_PARSER_TYPE_PARSER_HH

#include "../../components/parsing/LiteralParser.h"
#include "../../components/parsing/SkeletonParser.h"
#include "../../components/parsing/ParentheticalParser.h"

namespace MyLanguage {
    class TypeParser: public IParseable {
        public:
            TypeParser();
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
        std::unique_ptr<LiteralParser> _literalParser;
        std::unique_ptr<Parsing::ParentheticalParser> _parentheticalParser;
    };
};

#endif