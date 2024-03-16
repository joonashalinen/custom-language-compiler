#include "Expression.h"
#include <algorithm>

Expression::Expression(): _type("__NULL__"), _startPos(-1), _endPos(-1) {

}

Expression::Expression(
	std::string type, 
	int startPos, 
	int endPos
):
	_type(type), _startPos(startPos), _endPos(endPos)
{
}

Text::Location Expression::startLocation() {
	auto childLocation = this->_children.size() > 0 ? this->_children.front()->startLocation() : Text::Location{};
	auto firstTokenLocation = this->_tokens.size() > 0 ? this->_tokens.front().startLocation : Text::Location{};
	if (childLocation.positionIndex() != -1 && childLocation.positionIndex() < firstTokenLocation.positionIndex()) {
		return childLocation;
	}else {
		return firstTokenLocation;
	}
}

Text::Location Expression::endLocation() {
	auto childLocation = this->_children.size() > 0 ? this->_children.back()->endLocation() : Text::Location{};
	auto firstTokenLocation = this->_tokens.size() > 0 ? this->_tokens.back().startLocation : Text::Location{};
	if (childLocation.positionIndex() != -1 && childLocation.positionIndex() > firstTokenLocation.positionIndex()) {
		return childLocation;
	}else {
		return firstTokenLocation;
	}
}

std::map<std::string, std::string>& Expression::subTypes()
{
	return this->_subTypes;
}

void Expression::setSubTypes(std::map<std::string, std::string> subTypes)
{
	this->_subTypes = subTypes;
}

std::vector<DToken>& Expression::tokens()
{
	return this->_tokens;
}

void Expression::setTokens(std::vector<DToken> tokens)
{
	this->_tokens = tokens;
}

DToken Expression::rootToken()
{
	if (this->_tokens.size() > 0) {
		return this->_tokens.at(0);
	} else {
		throw std::runtime_error("The Expression has no root token.");
	}
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

void Expression::setEndPos(int endPos)
{
	this->_endPos = endPos;
	if (this->_parent && this->isLastChild() && this->_parent->endPos() < this->_endPos) {
		this->_parent->setEndPos(this->_endPos);
	}
}

std::vector<std::shared_ptr<Expression>>& Expression::children()
{
	return this->_children;
}

void Expression::setChildren(std::vector<std::shared_ptr<Expression>> children)
{
	this->_children = children;
}

std::shared_ptr<Expression> Expression::parent()
{
	return this->_parent;
}

void Expression::setParent(std::shared_ptr<Expression> parent)
{
    this->_parent = parent;
}

std::vector<std::string> Expression::extractChildSubTypeValues(std::string type, std::string subType)
{
	auto result = std::vector<std::string>{};
	std::for_each(
		this->children().begin(), 
		this->children().end(), 
		[&result, &type, &subType](std::shared_ptr<Expression> child) {
			if (type != "*" && child->type() == type) {
				auto value = child->subTypes().at(subType);
				result.insert(result.end(), value);
			}
		} 
	);
	return result;
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
	if (child->endPos() > expression->endPos()) {
		expression->setEndPos(child->endPos());
	}
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

bool Expression::isLastChild()
{
	if (this->_parent && this->_parent->children().size() > 0) {
		auto selfInParent = std::find_if(
			this->_parent->children().begin(), 
			this->_parent->children().end(), 
			[this](std::shared_ptr<Expression> child) {
				return child.get() == this;
			}
		);
		return (selfInParent == this->_parent->children().end() - 1);
	} else {
		return false;
	}
}