#include "Location.h"

Text::Location::Location():
	_positionIndex(-1), _line(-1), _column(-1) {
}

Text::Location::Location(int positionIndex, int line, int column):
	_positionIndex(positionIndex), _line(line), _column(column) {
}

Text::Location::Location(int positionIndex, const std::string::iterator textStart)
{
	this->_positionIndex = positionIndex;
	// Determine the line number.
	this->_line = 1 + std::count_if(textStart, textStart + positionIndex + 1, [](char c) {return c == '\n';});
	// Determine the column number.
	int lastNewLineIndex = std::accumulate(
		textStart, 
		textStart + positionIndex + 1, 
		std::pair<int, int>{0, 0}, // A pair of the form: (index of the last newline encountered, current character index). 
		[](std::pair<int, int> acc, char c) {
			return (
				c == '\n' ? 
					std::pair<int, int>{acc.second, acc.second + 1} :
					std::pair<int, int>{acc.first, acc.second + 1}
			);
		}
	).first;
	this->_column = positionIndex - lastNewLineIndex + 1;
}

int Text::Location::positionIndex()
{
	return this->_positionIndex;
}

int Text::Location::line()
{
	return this->_line;
}

int Text::Location::column()
{
	return this->_column;
}

std::string Text::Location::toString()
{
	return "line " + std::to_string(this->_line) + ", column " + std::to_string(this->_column);
}