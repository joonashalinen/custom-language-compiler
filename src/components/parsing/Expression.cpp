#include "Expression.h"

Expression::Expression(std::string type, int startPos, int endPos):
    _type(type), _startPos(startPos), _endPos(endPos)
{
}

std::string Expression::type()
{
	return this->_type;
}

int Expression::startPos()
{
	return this->_startPos;
}

int Expression::endPos()
{
	return this->_endPos;
}

std::vector<std::shared_ptr<Expression>>& Expression::children()
{
	return this->_children;
}

std::shared_ptr<Expression> Expression::parent()
{
	return this->_parent;
}

void Expression::setParent(std::shared_ptr<Expression> parent)
{
    this->_parent = parent;
}

void Expression::addChild(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> child)
{
    expression->children().insert(expression->children().end(), child);
    child->setParent(expression);
}
