#include "Tokenization.h"
#include <string>
#include <iostream>
#include <regex>
#include <string_view>
#include <stdexcept>

Tokenization::Tokenizer::Tokenizer():
    _patterns(std::vector<std::shared_ptr<TokenPattern>>{})
{

}

DToken<Tokenization::TToken> Tokenization::Tokenizer::recognizeToken(std::string text) {
    // Find the first pattern that matches the beginning of the text.
    auto pattern = std::find_if(this->_patterns.begin(), this->_patterns.end(), [&](auto& p) {
        // Find a match in the text for the given token pattern.
        auto match = p->second->recognize(text);
        // If the match exists and is at the beginning of the text.
        return (match.first == 0);
    });

    // If a matching pattern was found, return it.
    if (pattern != this->_patterns.end()) {
        // Get the match.
        auto match = (*pattern)->second->recognize(text);
        // Create a new token object from the match.
        DToken<TToken> token{
            (*pattern)->first, 
            match.second, 
            match.first, 
            match.first + ((int) match.second.size())
        };
        return token;
    } else {
        throw std::runtime_error("Error during tokenizing.");
    }
}

std::vector<DToken<Tokenization::TToken>> Tokenization::Tokenizer::tokenize(std::string text) {
    // List of found tokens in text.
    auto tokens = std::vector<DToken<TToken>>();
    // Current position in text.
    int pos = 0;
    while (true) {
        // If we have reached the end of the text.
        if (pos == text.size()) {
            // Add the end token.
            DToken<TToken> token {"end", "", pos, pos};
            tokens.insert(tokens.end(), token);
            // Return from the loop.
            break;
        } else {
            // We have not reached the end of the text. Thus, we should check 
            // if the beginning of the text matches with some token pattern.
            auto token = this->recognizeToken(text.substr(pos));
            // Adjust the token position to the absolute position in relation 
            // to the whole text.
            token.startPos = pos;
            token.endPos = pos + token.value.size();
            // Add the token to the result only if it is not a comment 
            // or whitespace, since we do not care about these.
            if (token.type != "comment" && token.type != "whitespace") {
                tokens.insert(tokens.end(), token);
            }
            // Advance to the end of the found token.
            pos = token.endPos;
        }
    }
    return tokens;
}

void Tokenization::Tokenizer::addRegexPattern(TToken type, std::string regex) {
    this->_patterns.insert(
        this->_patterns.end(), 
        std::shared_ptr<TokenPattern>(
            new TokenPattern(
                type, 
                std::shared_ptr<ITextPattern>(new RegexPattern{regex})
            )
        )
    );
}