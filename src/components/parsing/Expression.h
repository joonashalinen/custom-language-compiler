#ifndef PARSING_EXPRESSION_HH
#define PARSING_EXPRESSION_HH

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../tokenization/DToken.h"
#include "../text/Location.h"

/**
 * A node in the Abstract Syntax Tree of a language being parsed.
 */
class Expression {
    public:
        Expression();
        Expression(std::string type, int startPos, int endPos);
        Text::Location startLocation();
        Text::Location endLocation();
        std::map<std::string, std::string>& subTypes();
        void setSubTypes(std::map<std::string, std::string> subTypes);
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
        /**
         * Get the sub-type values of the expression's children. If the given type parameter is not 
         * '*', will only return sub-type values for children who have the given type.
         */
        std::vector<std::string> extractChildSubTypeValues(std::string type, std::string subType);
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
        Text::Location _startLocation;
        Text::Location _endLocation;
        std::map<std::string, std::string> _subTypes;
        std::vector<std::shared_ptr<Expression>> _children;
        std::shared_ptr<Expression> _parent;
        std::string _type;
        int _startPos;
        int _endPos;
        std::vector<DToken> _tokens;
};

#endif