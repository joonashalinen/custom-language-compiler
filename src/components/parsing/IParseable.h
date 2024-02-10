#ifndef PARSING_IPARSEABLE_HH
#define PARSING_IPARSEABLE_HH

#include <utility>
#include <memory>
#include "../tokenization/DToken.h"
#include "Expression.h"

class IParseable {
    public:
        virtual std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position) = 0;
        virtual bool canParseAt(std::vector<DToken>& tokens, int position) {
            try {
                this->parse(tokens, position);
                return true;
            } catch (...) {
                return false;
            }
        }
};

#endif