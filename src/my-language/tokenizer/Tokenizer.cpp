#include "Tokenizer.h"

Tokenizer::Tokenizer(Tokenization::Tokenizer tokenizer):
    tokenizer(tokenizer)
{
    // Configure the different token patterns of our language.
    this->tokenizer.addRegexPattern("if", "if");
    this->tokenizer.addRegexPattern("whitespace", "\\s+");
    this->tokenizer.addRegexPattern("comment", "(//|#).*\n");
    this->tokenizer.addRegexPattern("number", "[0-9]+");
    this->tokenizer.addRegexPattern("identifier", "([A-Z] | [a-z])([A-Z] | [a-z] | [0-9])+");
}