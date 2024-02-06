#ifndef PARSING_TOKEN_SEQUENCE_HH
#define PARSING_TOKEN_SEQUENCE_HH

#include "../tokenization/DToken.h"
#include <vector>

/**
 * A data structure containing the tokens that are 
 * processed during parsing.
 */
class TokenSequence {
    public:
        TokenSequence(std::vector<DToken>& tokens);
        DToken peek();
        DToken consume();
        void setPosition(int position);
        int position();
    private:
        std::vector<DToken>& _tokens;
        int _position;
};

#endif