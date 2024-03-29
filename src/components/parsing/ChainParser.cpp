#include "ChainParser.h"
#include <iostream>

Parsing::ChainParser::ChainParser(
    std::string type,
    std::string separator,
    IParseable& parser,
    std::function<bool(std::vector<DToken>& tokens, int position)> separatorOptionalityRule
):
    _type(type), _separator(separator), _parser(parser), _separatorOptionalityRule(separatorOptionalityRule)
{
}

std::shared_ptr<Expression> Parsing::ChainParser::parse(std::vector<DToken>& tokens, int position)
{
	auto sequence = TokenSequence{tokens};
    sequence.setPosition(position);

    // First, we parse the expressions of the chain.

    auto expressions = std::vector<std::shared_ptr<Expression>>{};
    auto subTypes = std::map<std::string, std::string>{};
    std::shared_ptr<Expression> expression;
    // While there is a parsing rule for the next token.
    while (this->_parser.canParseAt(tokens, sequence.position())) {
        // Parse the next expression.
        expression = this->_parser.parse(tokens, sequence.position());
        expressions.insert(expressions.end(), expression);

        // Check whether we should expect a separator to follow the expression.
        sequence.setPosition(expression->endPos());
        auto separatorIsOptional = this->_separatorOptionalityRule(tokens, sequence.position());

        // If we do expect a separator to follow but one is not present.
        if (!separatorIsOptional && sequence.peek().value != this->_separator) {
            throw std::runtime_error(
                "Expected a separator character '" + this->_separator + "' to follow but '" +
                sequence.peek().value + "' was encountered instead at " + sequence.peek().startLocation.toString() 
            );
        } else {
            // Skip over potential (optional) following separators.
            while (sequence.peek().value == this->_separator) {
                sequence.consume();
            }
        }
    }

    // Determine whether the chain has a closing separator or not.
    if ((sequence.position() > 0) && (sequence.tokens().at(sequence.position() - 1).value != this->_separator)) {
        subTypes.insert({"openness", "open"});
    } else {
        subTypes.insert({"openness", "closed"});
    }

    // Next, we form the resulting root expression.

    auto result = std::shared_ptr<Expression>(
        new Expression{
            this->_type,
            position,
            sequence.position()
        }
    );
    result->setChildren(expressions);
    result->setSubTypes(subTypes);
    return result;
}
