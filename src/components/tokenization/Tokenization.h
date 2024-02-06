#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "DToken.h"
#include "../text/ITextPattern.h"
#include "../text/RegexPattern.h"

#ifndef TOKENIZATION_HH
#define TOKENIZATION_HH

namespace Tokenization {
    using TToken = std::string;
    using TokenPattern = std::pair<TToken, std::shared_ptr<ITextPattern>>;
    class Tokenizer {
        public:
            Tokenizer();
            DToken recognizeToken(std::string text);
            std::vector<DToken> tokenize(std::string text);
            void addRegexPattern(TToken type, std::string regex);
        private:
            std::vector<std::shared_ptr<TokenPattern>> _patterns;
    };
}

#endif