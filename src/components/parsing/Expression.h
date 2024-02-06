#ifndef PARSING_EXPRESSION_HH
#define PARSING_EXPRESSION_HH

#include <string>
#include <vector>
#include <memory>

class Expression {
    public:
        Expression(std::string type, int startPos, int endPos);
        std::string type();
        int startPos();
        int endPos();
        std::vector<std::shared_ptr<Expression>>& children();
        std::shared_ptr<Expression> parent();
        void setParent(std::shared_ptr<Expression> parent);
        static void addChild(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> child);
    private:
        std::vector<std::shared_ptr<Expression>> _children;
        std::shared_ptr<Expression> _parent;
        std::string _type;
        int _startPos;
        int _endPos;
};

#endif