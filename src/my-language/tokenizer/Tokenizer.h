#include "../../components/tokenization/Tokenization.h"

#ifndef MY_LANGUAGE_TOKENIZER_HH
#define MY_LANGUAGE_TOKENIZER_HH

/**
 * The tokenizer of our custom language's compiler.
 */
class Tokenizer {
    public:
        Tokenizer(Tokenization::Tokenizer tokenizer);
        Tokenization::Tokenizer tokenizer;
};

#endif