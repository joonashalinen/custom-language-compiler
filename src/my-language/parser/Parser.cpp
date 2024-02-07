#include "Parser.h"

MyLanguage::Parser::Parser() {
    // Create parsers.
    this->_mapParser = std::unique_ptr<MapParser>(new MapParser{});
    this->_literalParser = std::unique_ptr<LiteralParser>(new LiteralParser{"identifier"});
    this->_binaryParser = std::unique_ptr<BinaryParser>(new BinaryParser{"binary-operator", *(this->_mapParser)});
    this->_unaryParser = std::unique_ptr<UnaryParser>(new UnaryParser{"unary-operator", *(this->_mapParser)});
    this->_topLevelParser = std::unique_ptr<OperatedChainParser>(
        new OperatedChainParser{*(this->_mapParser), std::set<std::string>{"binary-operator"}}
    );

    // Set the look-forward parsing rules used when in a general expression parsing context.
    this->_mapParser->setParsers(
        std::map<std::string, IParseable*>{
            {"identifier", this->_literalParser.get()},
            {"binary-operator", this->_binaryParser.get()},
            {"unary-operator", this->_unaryParser.get()}
        }
    );

    // Set expression precedence levels.
    this->_topLevelParser->setPrecedenceLevels(
        std::map<std::string, int>{
            {"identifier", 9},
            {"unary-operator", 8},
            {"binary-operator", 3}
        }
    );
}

std::shared_ptr<Expression> MyLanguage::Parser::parse(std::vector<DToken>& tokens, int position) {
	return this->_topLevelParser->parse(tokens, position);
}