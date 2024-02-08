#include "BlockParser.h"

Parsing::BlockParser::BlockParser(ParentheticalParser& parser):
    _parser(parser)
{
}

std::shared_ptr<Expression> Parsing::BlockParser::parse(std::vector<DToken>& tokens, int position)
{
	auto sequence = TokenSequence{tokens};
    sequence.setPosition(position);

    this->_parser.parse(tokens, position);

    // Replace the root parenthetical expression with 
}