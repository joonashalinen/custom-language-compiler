#ifndef TEXT_LOCATION_HH
#define TEXT_LOCATION_HH

#include <string>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace Text {
    /**
     * A location in a text string.
     */
    class Location {
        public:
            Location();
            Location(int positionIndex, int line, int column);
            Location(int positionIndex, const std::string::iterator textStart);
            int positionIndex();
            int line();
            int column();
            std::string toString();
        private:
            int _positionIndex;
            int _line;
            int _column;
    };
};

#endif