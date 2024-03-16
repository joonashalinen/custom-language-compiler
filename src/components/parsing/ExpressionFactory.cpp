#include "ExpressionFactory.h"

std::shared_ptr<Expression> Parsing::ExpressionFactory::createBinaryOperation(
    std::string operationName, 
    std::shared_ptr<Expression> leftChild,
    std::shared_ptr<Expression> rightChild
) {
	return std::shared_ptr<Expression>(nullptr);
}