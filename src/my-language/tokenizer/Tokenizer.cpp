#include "Tokenizer.h"

Tokenizer::Tokenizer(Tokenization::Tokenizer tokenizer):
    tokenizer(tokenizer)
{
    // Configure the different token patterns of our language.
    this->tokenizer.addRegexPattern("whitespace", "\\s+");
    this->tokenizer.addRegexPattern("comment", "(//|#).*\n");
    this->tokenizer.addRegexPattern("number", "[0-9]+");
    this->tokenizer.addRegexPattern("if", "if");
    this->tokenizer.addRegexPattern("then", "then");
    this->tokenizer.addRegexPattern("else", "else");
    this->tokenizer.addRegexPattern("while", "while");
    this->tokenizer.addRegexPattern("do", "do");
    this->tokenizer.addRegexPattern("boolean", "(true|false)");
    this->tokenizer.addRegexPattern("binary-operator", "(\\+|\\*|\\/|\\=|\\=\\=|\\!\\=|\\<|\\<\\=|\\>|\\>\\=|%|and|or)");
    this->tokenizer.addRegexPattern("minus", "-");
    this->tokenizer.addRegexPattern("unary-operator", "not");
    this->tokenizer.addRegexPattern("parentheses", "[(){}]");
    this->tokenizer.addRegexPattern("statement-separator", ";");
    this->tokenizer.addRegexPattern("statement-separator", ",");
    this->tokenizer.addRegexPattern("identifier", "[A-Za-z_][A-Za-z0-9_-]*");
}