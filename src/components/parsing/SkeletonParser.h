#ifndef MY_LANGUAGE_PARSER_VARIABLE_DECLARATION_PARSER_HH
#define MY_LANGUAGE_PARSER_VARIABLE_DECLARATION_PARSER_HH

#include "IParseable.h"
#include "TokenSequence.h"
#include <algorithm>
#include <map>

namespace Parsing {
    class SkeletonParser: public IParseable {
        public:
            SkeletonParser(
                std::string type,
                std::vector<std::pair<std::string, std::string>> pattern, 
                std::map<std::string, IParseable*> parsers
            );
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::string _type;
            std::vector<std::pair<std::string, std::string>> _pattern;
            std::map<std::string, IParseable*> _parsers;
            bool _nextPatternElementMatches(std::pair<std::string, std::string> element, DToken token);
    };
};

#endif