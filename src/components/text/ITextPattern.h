#ifndef TEXT_ITEXTPATTERN_HH
#define TEXT_ITEXTPATTERN_HH

#include <string>
#include <utility>

/**
 * A pattern found in text.
*/
class ITextPattern {
    public:
        /**
         * Returns the position of the pattern and the matched 
         * text. If no match found, returns the pair (null location, "").
         */
        virtual std::pair<int, std::string> recognize(std::string text) = 0;
};

#endif