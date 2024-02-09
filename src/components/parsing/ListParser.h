#ifndef PARSING_LIST_PARSER_HH
#define PARSING_LIST_PARSER_HH

#include "../../components/tokenization/DToken.h"
#include "IParseable.h"
#include "TokenSequence.h"
#include "OperatedChainParser.h"
#include "ParentheticalParser.h"
#include "ChainParser.h"
#include "MapParser.h"
#include <string>
#include <functional>

namespace Parsing {
    /**
     * Parser for a list of expressions that are separated by a separator. The list is surrounded by 
     * specified tokens.
     */
    class ListParser: public IParseable {
        public:
            ListParser(
                std::string type, 
                std::string beginCharacter, 
                std::string endCharacter,
                std::string separator, 
                MapParser& elementParser
            );
            std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position);
        private:
            MapParser& _elementParser;
            std::unique_ptr<Parsing::ParentheticalParser> _parentheticalParser;
            std::unique_ptr<Parsing::ChainParser> _chainParser;
    };
}

#endif