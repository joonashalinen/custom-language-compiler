#ifndef PARSING_DEXPRESSION_HH
#define PARSING_DEXPRESSION_HH

#include <string>
#include "../data_structures/DNode.h"

typedef struct DExpression DExpression;

struct DExpression: public DNode<DExpression> {
    std::string type;
    int startPos;
    int endPos;
};

#endif