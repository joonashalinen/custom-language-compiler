#ifndef DATA_STRUCTURES_DNODE_HH
#define DATA_STRUCTURES_DNODE_HH

#include <vector>
#include <memory>

template <class T>
struct DNode {
    std::vector<std::shared_ptr<T>> children {};
    std::shared_ptr<T> parent {};
};

#endif