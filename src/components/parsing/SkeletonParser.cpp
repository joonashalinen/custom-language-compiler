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

    std::for_each(
        this->_pattern.begin(), 
        this->_pattern.end(), 
        [&sequence, this, &expressions](std::pair<std::string, std::string> element) {
            auto elementType = element.first;
            auto elementValue = element.second;
            
            if (
                (elementType == "token-value" && sequence.peek().value == elementValue) || 
                (elementType == "token-type" && sequence.peek().type == elementValue)) 
            {
                auto token = sequence.consume();
                if (elementType == "token-type") {
                    auto expression = std::shared_ptr<Expression>(
                        new Expression{
                            elementValue,
                            sequence.position() - 1,
                            sequence.position()
                        }
                    );
                    expression->tokens().insert(expression->tokens().end(), token);
                    expressions.insert(expressions.end(), expression);
                }

            } else if (elementType == "expression" && this->_parsers.contains(elementValue)) {
                auto parser = this->_parsers.at(elementValue);
                auto expression = parser->parse(sequence.tokens(), sequence.position());
                expressions.insert(expressions.end(), expression);
                sequence.setPosition(expression->endPos());
            }
        }
    );

    auto result = std::shared_ptr<Expression>(
        new Expression{
            this->_type,
            position,
            sequence.position()
        }
    );
    result->setChildren(expressions);

    return result;
}