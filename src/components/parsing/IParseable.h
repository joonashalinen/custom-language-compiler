#ifndef PARSING_IPARSEABLE_HH
#define PARSING_IPARSEABLE_HH

#include <utility>
#include "DExpression.h"
#include <memory>

class IParseable {
    public:
        virtual std::shared_ptr<DExpression> parse(int position) = 0;
};

#endif