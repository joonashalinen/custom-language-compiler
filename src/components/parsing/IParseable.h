#ifndef PARSING_IPARSEABLE_HH
#define PARSING_IPARSEABLE_HH

#include <utility>
#include <memory>
#include "../tokenization/DToken.h"
#include "Expression.h"
#include <iostream>

class IParseable {
    public:
        virtual std::shared_ptr<Expression> parse(std::vector<DToken>& tokens, int position) = 0;
        virtual bool canParseAt(std::vector<DToken>& tokens, int position) {
            try {
                this->parse(tokens, position);
                return true;
            } /* catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                return false;
            } */ catch (...) {
                return false;
            }
        }
};

#endif