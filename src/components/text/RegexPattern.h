#ifndef TEXT_REGEXPATTERN_HH
#define TEXT_REGEXPATTERN_HH

#include <string>
#include <regex>
#include "ITextPattern.h"

class RegexPattern: public ITextPattern {
    public:
        std::regex regex;
        RegexPattern(std::string regex);
        std::pair<int, std::string> recognize(std::string text) override;
};

#endif