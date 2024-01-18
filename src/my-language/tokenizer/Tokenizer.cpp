#include "Tokenizer.h"

Tokenizer::Tokenizer(Tokenization::Tokenizer tokenizer):
    tokenizer(tokenizer)
{
    // Configure the different token patterns of our language.
    this->tokenizer.addRegexPattern("whitespace", "\\s+");
    this->tokenizer.addRegexPattern("comment", "(//|#).*\n");
    this->tokenizer.addRegexPattern("number", "[0-9]+");
    this->tokenizer.addRegexPattern("if", "if");
    this->tokenizer.addRegexPattern("identifier", "[A-Za-z_][A-Za-z0-9_-]+");
    this->tokenizer.addRegexPattern("binary-operator", "\\+ | - | * | / | = | == | != | \\< | \\<= | \\> | \\>=");
    this->tokenizer.addRegexPattern("parentheses", "[()]");
}