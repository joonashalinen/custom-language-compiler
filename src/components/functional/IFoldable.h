#ifndef FUNCTIONAL_IFOLDABLE_HH
#define FUNCTIONAL_IFOLDABLE_HH

#include <functional>
#include <memory>

namespace Functional {
    template <class TAccumulator, class TValue>
    class IFoldable {
        public:
            virtual TAccumulator fold(
                std::function<TAccumulator(TValue, TAccumulator)> f, 
                TAccumulator acc
            ) = 0;
        private:
            
    };
};

#endif