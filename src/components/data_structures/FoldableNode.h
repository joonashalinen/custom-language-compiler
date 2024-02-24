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
    class FoldableNode: public Functional::IFoldable<TAccumulator, TNode> {        
        using FoldFunction = std::function<TAccumulator(TAccumulator, TNode)>;

        public:
            FoldableNode(TNode node);
            /**
             * Performs the folding function on the whole tree starting from the node. 
             * If the node has no children, the folding function is simply performed on the 
             * node itself with the given accumulator value. If the node has children, then 
             * TAccumulator::fold() is called recursively for its children. The resulting accumulators 
             * are then given to the folding function when it is called for the node itself.
             */
            TAccumulator fold(FoldFunction f, TAccumulator acc);
        private:
            TNode _node;

    };

    template <class TAccumulator, class TNode>
    inline FoldableNode<TAccumulator, TNode>::FoldableNode(
        TNode node
        ): _node(node) {
    }

    template <class TAccumulator, class TNode>
    inline TAccumulator FoldableNode<TAccumulator, TNode>::fold(
        FoldableNode::FoldFunction f, 
        TAccumulator acc
    ) {
        // If the node has no children then we can simply perform the function  
        // on the node.
        if (this->_node->children().size() == 0) {
            return f(acc, this->_node);
        } else {
            // Fold over the node's children.
            std::for_each(
                this->_node->children().begin(), 
                this->_node->children().end(),
                [&acc, &f, this](TNode node) {
                    acc = (FoldableNode<TAccumulator, TNode>{node}).fold(f, acc);
                }
            );
            // Perform the folding function for the current node with the latest accumulator value.
            return f(acc, this->_node);
        }
    }
};

#endif