#include "OperatedChainParser.h"
#include "TokenSequence.h"
#include <iostream>

OperatedChainParser::OperatedChainParser(
        IParseable& parser,
        std::map<std::string, IParseable*> nonUnaryParsers,
        std::map<std::string, int> precedenceLevels
    ):
    _parser(parser),
    _nonUnaryParsers(nonUnaryParsers),
    _precedenceLevels(precedenceLevels) {
    
}

std::shared_ptr<Expression> OperatedChainParser::parse(std::vector<DToken>& tokens, int position) {
    auto tokenSequence = TokenSequence{tokens};
    tokenSequence.setPosition(position);

    // First encountered expression.
    auto firstExpression = this->_parser.parse(tokens, position);
    tokenSequence.setPosition(firstExpression->endPos());

    // The next token past the first expression.
    auto nextToken = tokenSequence.consume();

    // If the next token is a recognized non-unary operator.
    if (this->_nonUnaryParsers.contains(nextToken.type)) {
        // The non-unary expression is parsed starting from the start of the first expression 
        // because the first expression becomes the non-unary expression's child.
        auto nonUnaryExpression = this->_nonUnaryParsers.at(nextToken.type)->parse(tokens, position);
        tokenSequence.setPosition(nonUnaryExpression->endPos());

        // If a further non-unary operator is encountered.
        if (this->_nonUnaryParsers.contains(tokenSequence.peek().type)) {
            auto rightMostChild = (Expression) (*(*(nonUnaryExpression->children().end() - 1)));
            auto restExpression = this->parse(tokens, rightMostChild.startPos());

            // Find the first leftmost descendant of the rest of the expression chain 
            // that has a higher precedence level than the non-unary expression.
            auto highestLeftChild = this->_firstHigherPrecedenceLeftChild(
                restExpression, 
                this->precedenceLevel(nonUnaryExpression)
            );

            // Remove the right-most child of the non-unary expression since 
            // we will replace it with some possibly other expression from the 
            // rest of the expression chain.
            nonUnaryExpression->children().erase(nonUnaryExpression->children().end() - 1);

            // Make the non-unary expression take the place of the found left descendant,
            // adding the descendant as its child.
            Expression::replaceAsParent(nonUnaryExpression, highestLeftChild);

            // Return the root node of the parse tree fragment.
            return Expression::earliestAncestor(nonUnaryExpression);
        } else {
            // We return the single non-unary expression encountered.
            return nonUnaryExpression;
        }
    } else {
        // We return the first expression.
        return firstExpression;
    }
}

int OperatedChainParser::precedenceLevel(std::shared_ptr<Expression> expression)
{
    // Get the precedence level of the expression type.
    // The expression has by default the lowest precedence level 
    // in case no explicit precedence level is set.
    int precedenceLevel = -1;
    if (this->_precedenceLevels.contains(expression->type())) {
        precedenceLevel = this->_precedenceLevels.at(expression->type());
        
    } else if (this->_precedenceLevels.contains(expression->rootToken().value)) {
        precedenceLevel = this->_precedenceLevels.at(expression->rootToken().value);
    }

    return precedenceLevel;
}

std::shared_ptr<Expression> OperatedChainParser::_firstHigherPrecedenceLeftChild(std::shared_ptr<Expression> expression, int precedence)
{
    // If the precedence level is higher than the one we are comparing against.
    if (this->precedenceLevel(expression) > precedence) {
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

IParseable& OperatedChainParser::parser()
{
    return this->_parser;
}

std::map<std::string, IParseable*> OperatedChainParser::nonUnaryParsers()
{
    return this->_nonUnaryParsers;
}

std::map<std::string, int> OperatedChainParser::precedenceLevels()
{
    return this->_precedenceLevels;
}
