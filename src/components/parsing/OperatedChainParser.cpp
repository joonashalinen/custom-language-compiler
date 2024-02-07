#include "OperatedChainParser.h"
#include "TokenSequence.h"
#include <iostream>

OperatedChainParser::OperatedChainParser(
        MapParser& mapParser,
        std::set<std::string> nonUnaryOperators,
        std::map<std::string, int> precedenceLevels
    ):
    _mapParser(mapParser),
    _nonUnaryOperators(nonUnaryOperators),
    _precedenceLevels(precedenceLevels) {
    
}

std::shared_ptr<Expression> OperatedChainParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);

    // First encountered expression.
    auto firstExpression = this->_mapParser.parse(tokens, position);
    tokenSequence.setPosition(firstExpression->endPos());

    // The next token past the first expression.
    auto nextToken = tokenSequence.consume();
    
    // If there is nothing recognizable past the first expression then we simply return 
    // the first expression.
    if (!(this->_mapParser.parsers().contains(nextToken.type))) {
        return firstExpression;
    } else {
        // We assume the next token to be an operator but not a unary operator.
        if (this->_nonUnaryOperators.contains(nextToken.type)) {
            // The non-unary expression is parsed starting from the start of the first expression 
            // because the first expression becomes the non-unary expression's child.
            auto nonUnaryExpression = this->_mapParser.parseWith(tokens, nextToken.type, position);
            // If there is nothing recognizable past the non-unary expression then we simply 
            // return the non-unary expression.
            if (!(this->_mapParser.parsers().contains(nextToken.type))) {
                return nonUnaryExpression;
            } else {
                auto rightMostChild = (Expression) (*(*(nonUnaryExpression->children().end() - 1)));
                auto restExpression = this->parse(tokens, rightMostChild.startPos());
    
                // Find the first leftmost descendant of the rest of the expression chain 
                // that has a higher precedence level than the non-unary expression.
                auto highestLeftChild = this->_firstHigherPrecedenceLeftChild(
                    restExpression, 
                    this->precedenceLevel(nonUnaryExpression->type())
                );

                // Make the non-unary expression take the place of the found left descendant.
                Expression::replace(nonUnaryExpression, highestLeftChild);

                // Return the root node of the parse tree fragment.
                return Expression::earliestAncestor(nonUnaryExpression);
            }
        } else {
            throw std::runtime_error(
                "The token at position " + std::to_string(nextToken.startPos) + 
                " was expected to be a binary operator."
            );
        }
    }

    return std::shared_ptr<Expression>(new Expression{"None", 0, 0});
}

int OperatedChainParser::precedenceLevel(std::string expressionType)
{
    // Get the precedence level of the expression type.
    // The expression has by default the lowest precedence level 
    // in case no explicit precedence level is set.
    int precedenceLevel = -1;
    if (this->_precedenceLevels.contains(expressionType)) {
        precedenceLevel = this->_precedenceLevels.at(expressionType);
    }

    return precedenceLevel;
}

std::shared_ptr<Expression> OperatedChainParser::_firstHigherPrecedenceLeftChild(std::shared_ptr<Expression> expression, int precedence)
{
    // If the precedence level is higher than the one we are comparing against.
    if (this->precedenceLevel(expression->type()) > precedence) {
        // Then we have found the leftmost child that has a higher precedence level.
        return expression;
    } else {
        // Else, the precedence level is lower than or equal.
        // If the expression has a left child then we recurse over that.
        if (expression->children().size() > 0) {
            return this->_firstHigherPrecedenceLeftChild(expression->children().at(0), precedence);
        } else {
            // Otherwise, we have reached the left-most child, which must be a literal expression 
            // since it has no children. Since it is a literal expression we assume it to have 
            // the highest possible precedence. Thus, we return it.
            return expression;
        }
    }
}

void OperatedChainParser::setPrecedenceLevels(std::map<std::string, int> precedenceLevels)
{
    this->_precedenceLevels = precedenceLevels;
}