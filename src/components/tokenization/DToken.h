#include <string>

#ifndef TOKENIZATION_DTOKEN_HH
#define TOKENIZATION_DTOKEN_HH

// The data of a Token detected during tokenization.
struct DToken {
    std::string type;
    std::string value;
    int startPos;
    int endPos;
};

#endif