#include "TokenSequence.h"
#include <cassert>
#include <iostream>

TokenSequence::TokenSequence(std::vector<DToken>& tokens):
    _tokens(tokens), _position(0) {
    assert(tokens.size() != 0);
}

DToken TokenSequence::peek() {
    if (this->_position < (int) (this->_tokens.size())) {
        return this->_tokens.at(this->_position);
    } else {
        auto endToken = DToken{
            "end",
            "",
            (*(this->_tokens.end() - 1)).endPos,
            (*(this->_tokens.end() - 1)).endPos
        };
        return endToken;
    }
}

DToken TokenSequence::consume() {
    auto token = this->peek();
    this->_position = this->_position + 1;
    return token;
}

void TokenSequence::setPosition(int position) {
    this->_position = position;
}

int TokenSequence::position() {
    return this->_position;
}