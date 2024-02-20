#ifndef DATA_STRUCTURES_FOLDABLE_NODE_HH
#define DATA_STRUCTURES_FOLDABLE_NODE_HH

#include <memory>
#include <functional>
#include "../functional/IFoldable.h"
#include <algorithm>
#include <numeric>
#include <iostream>

namespace DataStructures {
    /**
     * A node in a tree data structure that supports a fold operation 
     * (as fold is understood in functional programming). For details on the type of 
     * the fold operation, see the comment for FoldableNode::fold.
     */
    template <class TAccumulator, class TNode>
    class FoldableNode: public Functional::IFoldable<std::vector<TAccumulator>, std::shared_ptr<TNode>> {        
        using FoldFunction = std::function<std::vector<TAccumulator>(
            std::shared_ptr<TNode>,
            std::vector<TAccumulator>
        )>;

        public:
            FoldableNode(std::shared_ptr<TNode> node);
            /**
             * Performs the folding function on the whole tree starting from the node. 
             * If the node has no children, the folding function is simply performed on the 
             * node itself with the given accumulator value. If the node has children, then 
             * TAccumulator::fold() is called recursively for its children. The resulting accumulators 
             * are then given to the folding function when it is called for the node itself.
             */
            std::vector<TAccumulator> fold(FoldFunction f, std::vector<TAccumulator> acc);
            /**
             * Convenience method that gets rid of the need for wrapping the given accumulator 
             * and the resulting one in vectors. The reason they need to be wrapped in the other fold 
             * function is in order to implement the IFoldable interface.
             */
            TAccumulator fold(FoldFunction f, TAccumulator acc);
        private:
            std::shared_ptr<TNode> _node;

    };

    template <class TAccumulator, class TNode>
    inline FoldableNode<TAccumulator, TNode>::FoldableNode(
        std::shared_ptr<TNode> node
        ): _node(node) {
    }

    template <class TAccumulator, class TNode>
    inline std::vector<TAccumulator> FoldableNode<TAccumulator, TNode>::fold(
        FoldableNode::FoldFunction f, 
        std::vector<TAccumulator> acc
    ) {
        // If the node has no children then we can simply perform the function  
        // on the node.
        if (this->_node->children().size() == 0) {
            std::cout << "node has no children" << std::endl;
            return f(this->_node, acc);
        } else {
            std::cout << "node has children" << std::endl;
            auto results = std::vector<TAccumulator>(this->_node->children().size());
            // Gather all the fold results from the node's children.
            std::transform(
                this->_node->children().begin(), 
                this->_node->children().end(), 
                results.begin(),
                [f, acc, this](std::shared_ptr<TNode> node) {
                    auto result = (FoldableNode<TAccumulator, TNode>{node}).fold(f, acc);
                    return result.at(0);
                }
            );
            // Perform the folding function for the current node with the results from its child nodes.
            return f(this->_node, results);
        }
    }

    template<class TAccumulator, class TNode>
    inline TAccumulator FoldableNode<TAccumulator, TNode>::fold(FoldFunction f, TAccumulator acc)
    {
        return (this->fold(f, std::vector<TAccumulator>{acc})).at(0);
    }
};

#endif