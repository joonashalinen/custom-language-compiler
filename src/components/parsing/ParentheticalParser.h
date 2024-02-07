#ifndef PARSING_PARENTHETICAL_PARSER_HH
#define PARSING_PARENTHETICAL_PARSER_HH

#include "IParseable.h"

namespace Parsing {
    class ParentheticalParser: public IParseable {
        public:
            ParentheticalParser(std::string type, std::string beginCharacter, std::string endCharacter, IParseable& parser);
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            std::string _type;
            std::string _beginCharacter;
            std::string _endCharacter;
            IParseable& _parser;
    };
};

#endif