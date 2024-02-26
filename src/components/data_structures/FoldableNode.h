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
        using InFoldFunction = std::function<TAccumulator(TAccumulator, TNode, int)>;

        public:
            FoldableNode(TNode node);
            /**
             * Performs a fold over the tree data structure. The given folding function is 
             * applied in post-order, i.e. first for a parent node's children.
             */
            TAccumulator fold(FoldFunction f, TAccumulator acc);
            void setPreFolder(FoldFunction f);
            void setInFolder(InFoldFunction f);
        private:
            TNode _node;
            /**
             * A folding function that is performed for a parent node before it is for its children.
             */
            FoldFunction _preFolder = [](TAccumulator acc, TNode x){return acc;};
            /**
             * A folding function that is performed for a parent node in-between every 
             * fold on its children.
             */
            InFoldFunction _inFolder = [](TAccumulator acc, TNode x, int child){return acc;};
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
        // First, we perform the pre-order folding function on the node.
        acc = this->_preFolder(acc, this->_node);
        // If the node has no children then we can simply perform the function  
        // on the node.
        if (this->_node->children().size() == 0) {
            return f(acc, this->_node);
        } else {
            // Fold over the node's children.
            int i = 0;
            std::for_each(
                this->_node->children().begin(), 
                this->_node->children().end(),
                [&acc, &f, this, &i](TNode node) {
                    auto foldableChild = FoldableNode<TAccumulator, TNode>{node};
                    foldableChild.setPreFolder(this->_preFolder);
                    foldableChild.setInFolder(this->_inFolder);
                    acc = foldableChild.fold(f, acc);
                    // Perform the in-order folding function for the parent node.
                    acc = this->_inFolder(acc, this->_node, i);
                    i = i + 1;
                }
            );
            // Perform the folding function for the current node with the latest accumulator value.
            return f(acc, this->_node);
        }
    }
    template<class TAccumulator, class TNode>
    inline void FoldableNode<TAccumulator, TNode>::setPreFolder(FoldFunction f)
    {
        this->_preFolder = f;
    }

    template<class TAccumulator, class TNode>
    inline void FoldableNode<TAccumulator, TNode>::setInFolder(InFoldFunction f)
    {
        this->_inFolder = f;
    }
};

#endif