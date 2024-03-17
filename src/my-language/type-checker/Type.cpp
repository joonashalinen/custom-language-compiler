#include "Type.h"

MyLanguage::Type::Type(std::string name):
    _name(name) {
}

std::string MyLanguage::Type::toString()
{
	return this->_name;
}
