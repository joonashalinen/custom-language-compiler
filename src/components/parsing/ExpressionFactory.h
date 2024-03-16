#ifndef PARSING_EXPRESSION_FACTORY_HH
#define PARSING_EXPRESSION_FACTORY_HH

namespace Parsing {
    /**
     * Factory for manually creating Expression objects.
     */
    class ExpressionFactory {
        public:
            ExpressionFactory();
            /**
             * Create an expression equivalent to what the BinaryParser would produce.
             */
            std::shared_ptr<Expression> createBinaryOperation(
                std::string operationName, 
                std::shared_ptr<Expression> leftChild,
                std::shared_ptr<Expression> rightChild
            );
        private:
            
    };
};

#endif