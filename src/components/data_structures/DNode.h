#ifndef DATA_STRUCTURES_DNODE_HH
#define DATA_STRUCTURES_DNODE_HH

#include <vector>
#include <memory>

template <class TChild>
struct DNode {
    std::vector<std::shared_ptr<TChild>> children {};
};

#endif