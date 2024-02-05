#include "ExpressionChain.h"
#include "TokenSequence.h"

ExpressionChain::ExpressionChain(
        std::vector<DToken<>>& tokens,
        ExpressionMap& expressions,
        std::set<std::string> nonUnaryOperators
    ):
    _tokenSequence(TokenSequence{tokens}), 
    _tokens(tokens),
    _expressions(expressions),
    _nonUnaryOperators(nonUnaryOperators) {
    
}

std::shared_ptr<DExpression> ExpressionChain::parse(int position) {
    this->_tokenSequence.setPosition(position);

    // First encountered expression.
    auto firstExpression = this->_expressions.parse(position);
    this->_tokenSequence.setPosition(firstExpression->endPos);

    // The next token past the first expression.
    auto nextToken = this->_tokenSequence.consume();
    
    // If there is nothing recognizable past the first expression then we simply return 
    // the first expression.
    if (!(this->_expressions.expressionConstructors().contains(nextToken.type))) {
        return firstExpression;
    } else {
        // We assume the next token to be an operator but not a unary operator.
        if (this->_nonUnaryOperators.contains(nextToken.type)) {
            // The non-unary expression is parsed starting from before the first expression 
            // because the first expression becomes the non-unary expression's child.
            auto nonUnaryExpression = this->_expressions.parseWith(nextToken.type, position);
            // If there is nothing recognizable past the non-unary expression then we simply 
            // return the non-unary expression.
            if (!(this->_expressions.expressionConstructors().contains(nextToken.type))) {
                return nonUnaryExpression;
            } else {
                // We parse the rest of the expression chain 
                // to the right of the non-unary expression 
                // (but still including the right-most child of the non-unary expression).
                auto restChain = ExpressionChain{
                    this->_tokens,
                    this->_expressions,
                    this->_nonUnaryOperators
                };
                auto rightMostChild = (DExpression) (*(*(nonUnaryExpression->children.end() - 1)));
                auto restExpression = restChain.parse(rightMostChild.startPos);

                // Make the rest of the expression chain the right-most child of the non-unary expression.
                *(nonUnaryExpression->children.end() - 1) = restExpression;
                return nonUnaryExpression;
            }
        } else {
            throw std::runtime_error(
                "The token at position " + std::to_string(nextToken.startPos) + 
                " was expected to be a binary operator."
            );
        }
    }

    return std::shared_ptr<DExpression>(new DExpression{{}, "None", 0, 0});
}