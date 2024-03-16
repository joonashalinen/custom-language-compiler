#include "ParentheticalParser.h"
#include "TokenSequence.h"
#include <iostream>

Parsing::ParentheticalParser::ParentheticalParser(
    std::string type,
    std::string beginCharacter, 
    std::string endCharacter, 
    IParseable& parser
):
    _type(type), _beginCharacter(beginCharacter), _endCharacter(endCharacter), _parser(parser), _stripParentheses(false) {
}

std::shared_ptr<Expression> Parsing::ParentheticalParser::parse(std::vector<DToken>& tokens, int position)
{
    auto sequence = TokenSequence{tokens};
    sequence.setPosition(position);
    
    auto beginToken = sequence.consume();

    if (beginToken.value == this->_beginCharacter) {
        auto expression = this->_parser.parse(tokens, sequence.position());
        sequence.setPosition(expression->endPos());

        auto endToken = sequence.consume();

        if (endToken.value == this->_endCharacter) {
            if (!this->_stripParentheses) {
                auto result = std::shared_ptr<Expression>(
                    new Expression{this->_type, position, sequence.position()}
                );
                result->tokens().insert(result->tokens().end(), beginToken);
                result->tokens().insert(result->tokens().end(), endToken);
                result->children().insert(result->children().end(), expression);
                return result;
            } else {
                return expression;
            }
        } else {
            throw std::runtime_error(
                "Expected a '" + this->_endCharacter + "' but instead encountered '" + 
                endToken.value + "' at " + endToken.startLocation.toString()
            );
        }
    } else {
        throw std::runtime_error(
            "Expected a '" + this->_beginCharacter + "' but instead encountered '" + 
            beginToken.value + "' at " + beginToken.startLocation.toString()
        );
    }
}

void Parsing::ParentheticalParser::setStripParentheses(bool stripParentheses)
{
    this->_stripParentheses = stripParentheses;
}