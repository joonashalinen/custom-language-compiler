#ifndef TOKENIZATION_DTOKEN_HH
#define TOKENIZATION_DTOKEN_HH

#include <string>
#include "../text/Location.h"

// The data of a Token detected during tokenization.
struct DToken {
    std::string type;
    std::string value;
    int startPos;
    int endPos;
    Text::Location startLocation {};
    Text::Location endLocation {};
};

#endif