#include "SkeletonParser.h"
#include <iostream>

Parsing::SkeletonParser::SkeletonParser(
    std::string type,
    std::vector<std::pair<std::string, std::string>> pattern, 
    std::map<std::string, IParseable*> parsers
): _type(type), _pattern(pattern), _parsers(parsers) {

}

std::shared_ptr<Expression> Parsing::SkeletonParser::parse(std::vector<DToken>& tokens, int position)
{
    auto sequence = TokenSequence{tokens};
	sequence.setPosition(position);

    auto expressions = std::vector<std::shared_ptr<Expression>>{};
    auto expressionTokens = std::vector<DToken>{};

    for (int i = 0; i < ((int) this->_pattern.size()); i++) {
        auto element = this->_pattern.at(i);
        auto elementType = element.first;
        auto elementValue = element.second;

        // If the pattern is next expecting to see a token.
        if (
            (elementType == "token-value" && sequence.peek().value == elementValue) || 
            (elementType == "token-type" && sequence.peek().type == elementValue)
        ) {
            auto token = sequence.consume();
            // If the next token is expected to match a token type instead of a literal token value.
            if (elementType == "token-type") {
                // Then we create a literal expression from the token and add it to the 
                // pattern expressions values.
                auto expression = std::shared_ptr<Expression>(
                    new Expression{
                        elementValue,
                        sequence.position() - 1,
                        sequence.position()
                    }
                );
                expression->tokens().insert(expression->tokens().end(), token);
                expressions.insert(expressions.end(), expression);
            } else {
                // Conversely, a token that is matched by 
                // literal value is not added to the pattern expression's children, since 
                // it is interpreted to give no semantic extra information (because every expression matching 
                // the pattern will always have the same literal tokens). Instead, 
                // the literal token is added to the skeleton expression's list of its own tokens.
                expressionTokens.insert(expressionTokens.end(), token);
            }

        } else if (elementType == "expression" && this->_parsers.contains(elementValue)) {
            // If the pattern element is an expression that is recognized, we parse it using 
            // the given parsing rule.
            auto parser = this->_parsers.at(elementValue);
            auto expression = parser->parse(sequence.tokens(), sequence.position());
            expressions.insert(expressions.end(), expression);
            sequence.setPosition(expression->endPos());
        
        } else if (elementType == "trail") {
            // If the pattern has an optional trailing portion but the next part of the pattern 
            // does not match it, we end parsing.
            // If the trail element is the last pattern element we also wish to end parsing.
            if (
                (i == ((int) this->_pattern.size() - 1))  || 
                !(this->_nextPatternElementMatches(this->_pattern.at(i + 1), sequence.peek()))
            ) {
                break;
            } else {
                // Else, we advance to the next pattern element to continue parsing from there.
                continue;
            }
        } else {
            throw std::runtime_error(
                "Skeleton pattern could not match the next pattern element '" + elementValue + "'" +
                " with the next token '" + sequence.peek().value + "'."
            );
        }
    }

    auto result = std::shared_ptr<Expression>(
        new Expression{
            this->_type,
            position,
            sequence.position()
        }
    );
    result->setChildren(expressions);
    result->setTokens(expressionTokens);

    return result;
}

bool Parsing::SkeletonParser::_nextPatternElementMatches(std::pair<std::string, std::string> element, DToken token)
{
    auto elementType = element.first;
    auto elementValue = element.second;

    return (
        (elementType == "token-value" && token.value == elementValue) || 
        (elementType == "token-type" && token.type == elementValue) || 
        (elementType == "expression" && this->_parsers.contains(elementValue))
    );
}