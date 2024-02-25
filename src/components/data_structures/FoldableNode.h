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
            FoldableNode(TNode node, FoldFunction preFolder = [](TAccumulator acc, TNode x){return acc;});
            /**
             * Performs a fold over the tree data structure. The given folding function is 
             * applied in post-order, i.e. first for a parent node's children.
             */
            TAccumulator fold(FoldFunction f, TAccumulator acc);
        private:
            TNode _node;
            /**
             * A folding function that is performed for a parent node before it is for its children.
             */
            FoldFunction _preFolder;
    };

    template <class TAccumulator, class TNode>
    inline FoldableNode<TAccumulator, TNode>::FoldableNode(
        TNode node,
        FoldableNode::FoldFunction preFolder
        ): _node(node), _preFolder(preFolder) {
    }

    template <class TAccumulator, class TNode>
    inline TAccumulator FoldableNode<TAccumulator, TNode>::fold(
        FoldableNode::FoldFunction f, 
        TAccumulator acc
    ) {
        // First, we perform the pre-order folding function on the node.
        acc = this->_preFolder(acc, this->_node);
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