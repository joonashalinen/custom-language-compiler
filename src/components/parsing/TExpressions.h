#ifndef PARSING_TEXPRESSIONS_HH
#define PARSING_TEXPRESSIONS_HH

#include <functional>
#include <memory>
#include "../tokenization/DToken.h"
#include "IParseable.h"

namespace TExpressions {
    using ExpressionConstructor = const std::function<
        std::shared_ptr<IParseable>(std::vector<DToken>&)
    >;
}

#endif