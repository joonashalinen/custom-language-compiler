#ifndef DATA_STRUCTURES_BATCH_STACK_HH
#define DATA_STRUCTURES_BATCH_STACK_HH

#include <stack>
#include <vector>

namespace DataStructures {
    /**
     * A stack that you can pop and push multiple values from / to.
     */
    template <class T>
    class BatchStack {
        public:
            BatchStack();
            void push(std::vector<T> values);
            std::vector<T> pop(int amount);
            T pop();
            std::stack<T>& stack();
        private:
            std::stack<T> _stack;
    };
    
    template<class T>
    inline BatchStack<T>::BatchStack()
    {
    }

    template<class T>
    inline void BatchStack<T>::push(std::vector<T> values)
    {
        std::for_each(values.begin(), values.end(), [this](T value) {
            this->_stack.push(value);
        });
    }

    template<class T>
    inline std::vector<T> BatchStack<T>::pop(int amount)
    {
        auto result = std::vector<T>{};
        for (int i = 0; i < amount; i++) {
            result.insert(result.end(), this->_stack.top());
            this->_stack.pop();
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    template<class T>
    inline T BatchStack<T>::pop()
    {
        auto result = this->_stack.top();
        this->_stack.pop();
        return result;
    }

    template<class T>
    inline std::stack<T>& BatchStack<T>::stack()
    {
        return this->_stack;
    }
};

#endif