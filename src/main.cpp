#include <iostream>
#include "components/tokenization/Tokenization.h"
#include "my-language/tokenizer/Tokenizer.h"

int main(int argc, char* argv[]) {
    auto tokenizer = Tokenizer{Tokenization::Tokenizer{}};
    auto tokens = tokenizer.tokenizer.tokenize("// test comment\n   if        ");
    std::cout << tokens.size() << std::endl;
    std::cout << tokens.at(0).type << std::endl;
    std::cout << tokens.at(0).value << std::endl;
    std::cout << tokens.at(0).startPos << std::endl;
    std::cout << tokens.at(0).endPos << std::endl;
}