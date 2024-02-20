#ifndef PARSING_EXPRESSION_HH
#define PARSING_EXPRESSION_HH

#include <string>
#include <vector>
#include <memory>
#include "../tokenization/DToken.h"

/**
 * A node in the Abstract Syntax Tree of a language being parsed.
 */
class Expression {
    public:
        Expression(std::string type, int startPos, int endPos);
        std::vector<DToken>& tokens();
        void setTokens(std::vector<DToken> tokens);
        DToken rootToken();
        std::string type();
        int startPos();
        int endPos();
        void setEndPos(int endPos);
        std::vector<std::shared_ptr<Expression>>& children();
        void setChildren(std::vector<std::shared_ptr<Expression>> children);
        std::shared_ptr<Expression> parent();
        void setParent(std::shared_ptr<Expression> parent);
        bool isLastChild();
        static std::shared_ptr<Expression> earliestAncestor(std::shared_ptr<Expression> expression);
        static void addChild(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> child);
        static void removeChild(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> child);
        static void replaceChild(
            std::shared_ptr<Expression> expression, 
            std::shared_ptr<Expression> replacer, 
            std::shared_ptr<Expression> replacee
        );
        static void replaceAsParent(std::shared_ptr<Expression> replacer, std::shared_ptr<Expression> replacee);
        static void replace(std::shared_ptr<Expression> replacer, std::shared_ptr<Expression> replacee);
    private:
        std::vector<std::shared_ptr<Expression>> _children;
        std::shared_ptr<Expression> _parent;
        std::string _type;
        int _startPos;
        int _endPos;
        std::vector<DToken> _tokens;
};

#endif