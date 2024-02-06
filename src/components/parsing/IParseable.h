#ifndef PARSING_IPARSEABLE_HH
#define PARSING_IPARSEABLE_HH

#include <utility>
#include "DExpression.h"
#include <memory>
#include "../tokenization/DToken.h"

class IParseable {
    public:
        virtual std::shared_ptr<DExpression> parse(std::vector<DToken>& tokens, int position) = 0;
};

#endif