#include "VariableStack.h"

namespace StructuredLanguage {
	VariableStack::VariableStack()
	{
        
	}

	void VariableStack::push(std::vector<std::string> variables) {
		std::for_each(variables.begin(), variables.end(), [this](std::string variable) {
            this->_locations.insert({variable, this->nextLocation()});
        });
	}

	int VariableStack::location(std::string variable)
	{
		if (this->_locations.contains(variable)) {
            return this->_locations.at(variable);
        } else {
            throw std::runtime_error("No variable in stack: '" + variable + "'.");
        }
	}

	int VariableStack::nextLocation()
	{
		auto nextLocation = this->_currentLocation;
        this->_currentLocation = this->_currentLocation + 8;
        return nextLocation;
	}

	int VariableStack::size()
	{
		return this->_locations.size() * 8;
	}
}