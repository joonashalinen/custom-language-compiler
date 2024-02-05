#include <string>

#ifndef TOKENIZATION_DTOKEN_HH
#define TOKENIZATION_DTOKEN_HH

template<typename TType = std::string>
// The data of a Token detected during tokenization.
struct DToken {
    TType type;
    std::string value;
    int startPos;
    int endPos;
};

#endif