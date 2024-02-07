#include "Expression.h"
#include <algorithm>

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

std::shared_ptr<Expression> Expression::earliestAncestor(std::shared_ptr<Expression> expression)
{
	if (expression->parent()) {
		auto parent = expression->parent();
		while (parent->parent()) {
			parent = parent->parent();
		}
		return parent;
	} else {
		// If the expression has no parent, then the expression 
		// itself is its own earliest ancestor.
		return expression;
	}
}

void Expression::addChild(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> child)
{
    expression->children().insert(expression->children().end(), child);
    child->setParent(expression);
}

void Expression::removeChild(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> child) {
	auto childInChildren = std::find(expression->children().begin(), expression->children().end(), child);
	if (childInChildren != expression->children().end()) {
		expression->children().erase(childInChildren);
		child->setParent(std::shared_ptr<Expression>());
	}
}

void Expression::replaceChild(
	std::shared_ptr<Expression> expression, 
	std::shared_ptr<Expression> replacer, 
	std::shared_ptr<Expression> replacee
) {
	auto child = std::find(expression->children().begin(), expression->children().end(), replacee);
	if (child != expression->children().end()) {
		replacer->setParent(expression);
		replacee->setParent(std::shared_ptr<Expression>());
		*child = replacer;
	}
}

void Expression::replaceAsParent(std::shared_ptr<Expression> replacer, std::shared_ptr<Expression> replacee) {
	Expression::replace(replacer, replacee);
	Expression::addChild(replacer, replacee);
}

void Expression::replace(std::shared_ptr<Expression> replacer, std::shared_ptr<Expression> replacee) {
	auto parent = replacee->parent();
	if (parent) {
		Expression::replaceChild(parent, replacer, replacee);
	}
}