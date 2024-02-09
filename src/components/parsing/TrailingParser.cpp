#include "TrailingParser.h"

TrailingParser::TrailingParser(std::vector<std::string> types):
    _types(types) {
}

std::shared_ptr<Expression> TrailingParser::parse(std::vector<DToken>& tokens, int position)
{
	
}
