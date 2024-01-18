#include "RegexPattern.h"
#include <iostream>

RegexPattern::RegexPattern(std::string regex):
    regex(
        regex,
        std::regex_constants::ECMAScript
    )
{
    
}

std::pair<int, std::string> RegexPattern::recognize(std::string text) {
    std::smatch match;
    auto found = std::regex_search(text, match, this->regex);
    if (found) {
        return std::pair<int, std::string>(match.position(0), match[0]);
    } else {
        return std::pair<int, std::string>(-1, "");
    }
}