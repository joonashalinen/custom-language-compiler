#include "OperatedChainParser.h"
#include "TokenSequence.h"
#include <iostream>

OperatedChainParser::OperatedChainParser(
        MapParser& expressions,
        std::set<std::string> nonUnaryOperators,
        std::map<std::string, int> precedenceLevels
    ):
    _expressions(expressions),
    _nonUnaryOperators(nonUnaryOperators),
    _precedenceLevels(precedenceLevels) {
    
}

std::shared_ptr<Expression> OperatedChainParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);

    // First encountered expression.
    auto firstExpression = this->_expressions.parse(tokens, position);
    tokenSequence.setPosition(firstExpression->endPos());

    // The next token past the first expression.
    auto nextToken = tokenSequence.consume();
    
    // If there is nothing recognizable past the first expression then we simply return 
    // the first expression.
    if (!(this->_expressions.expressionConstructors().contains(nextToken.type))) {
        return firstExpression;
    } else {
        // We assume the next token to be an operator but not a unary operator.
        if (this->_nonUnaryOperators.contains(nextToken.type)) {
            // The non-unary expression is parsed starting from before the first expression 
            // because the first expression becomes the non-unary expression's child.
            auto nonUnaryExpression = this->_expressions.parseWith(tokens, nextToken.type, position);
            // If there is nothing recognizable past the non-unary expression then we simply 
            // return the non-unary expression.
            if (!(this->_expressions.expressionConstructors().contains(nextToken.type))) {
                return nonUnaryExpression;
            } else {
                // We parse the rest of the expression chain 
                // to the right of the non-unary expression 
                // (but still including the right-most child of the non-unary expression).
                auto restChain = OperatedChainParser{
                    this->_expressions,
                    this->_nonUnaryOperators,
                    this->_precedenceLevels
                };

                auto rightMostChild = (Expression) (*(*(nonUnaryExpression->children().end() - 1)));
                auto restExpression = restChain.parse(tokens, rightMostChild.startPos());
    
                // Find the first leftmost descendant of the rest of the expression chain 
                // that has a higher precedence level than the non-unary expression.
                /* auto highestLeftChild = this->_firstHigherPrecedenceLeftChild(
                    restExpression, 
                    this->precedenceLevel(nonUnaryExpression->type)
                ); */

                // Make the found higher precedence expression the right-most child of the non-unary expression.
                *(nonUnaryExpression->children().end() - 1) = restExpression;
                return nonUnaryExpression;
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
        // We assume the expression then has a left child we can recurse over.
        if (expression->children().size() > 0) {
            return this->_firstHigherPrecedenceLeftChild(expression->children().at(0), precedence);
        } else {
            throw std::runtime_error("Could not find higher precedence left child even though one should exist.");
        }
    }
}

