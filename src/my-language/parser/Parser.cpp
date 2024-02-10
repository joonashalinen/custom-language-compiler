#include "Parser.h"
#include <iostream>

MyLanguage::Parser::Parser() {
    // First, we create the parsers.

    this->_mapParser = std::unique_ptr<MapParser>(new MapParser{});
    this->_identifierLiteralParser = std::unique_ptr<LiteralParser>(new LiteralParser{"identifier"});
    this->_binaryParser = std::unique_ptr<BinaryParser>(new BinaryParser{"binary-operator", *(this->_mapParser)});
    this->_unaryParser = std::unique_ptr<UnaryParser>(new UnaryParser{"unary-operator", *(this->_mapParser)});
    
    this->_functionCallParser = std::unique_ptr<MyLanguage::FunctionCallParser>(
        new MyLanguage::FunctionCallParser{
            this->_identifierLiteralParser.get(),
            *(this->_mapParser)
        }
    );

    this->_identifierParser = std::unique_ptr<Parsing::ConflictParser>(
        new Parsing::ConflictParser{
            {this->_identifierLiteralParser.get(), this->_functionCallParser.get()}
        }
    );

    this->_operatedChainParser = std::unique_ptr<OperatedChainParser>(
        new OperatedChainParser{
            *(this->_mapParser), 
            std::map<std::string, IParseable*>{
                {"binary-operator", this->_binaryParser.get()}
            }
        }
    );

    this->_parentheticalParser = std::unique_ptr<Parsing::ParentheticalParser>(
        new Parsing::ParentheticalParser{"parenthetical", "(", ")", *(this->_operatedChainParser)}
    );

    this->_chainParser = std::unique_ptr<MyLanguage::ChainParser>(
        new MyLanguage::ChainParser{this->_operatedChainParser.get()}
    );

    this->_blockParser = std::unique_ptr<Parsing::ParentheticalParser>(
        new Parsing::ParentheticalParser{"block", "{", "}", *(this->_chainParser)}
    );

    this->_ifParser = std::unique_ptr<MyLanguage::IfParser>(
        new MyLanguage::IfParser{
            this->_operatedChainParser.get()
        }
    );

    this->_whileParser = std::unique_ptr<MyLanguage::WhileParser>(
        new MyLanguage::WhileParser{
            this->_operatedChainParser.get()
        }
    );

    // Next, we set the look-forward parsing rules used when in a general expression parsing context.

    this->_mapParser->setParsers(
        std::map<std::string, IParseable*>{
            {"identifier", this->_identifierParser.get()},
            {"unary-operator", this->_unaryParser.get()},
            {"(", this->_parentheticalParser.get()},
            {"{", this->_blockParser.get()},
            {"if", this->_ifParser.get()},
            {"while", this->_whileParser.get()}
        }
    );

    // Finally, we set the expression precedence levels.
    
    this->_operatedChainParser->setPrecedenceLevels(
        std::map<std::string, int>{
            {"identifier", 9}, {"if", 9}, {"while", 9}, {"var", 9}, {"function-call", 9},
            {"(", 9}, // An expression surrounded by parentheses.
            {"{", 9}, // A block expression.
            {"unary-operator", 8},
            {"*", 7}, {"/", 7}, {"%", 7},
            {"+", 6}, {"-", 6},
            {"<", 5}, {"<=", 5}, {">", 5}, {">=", 5},
            {"==", 4}, {"!=", 4},
            {"and", 3},
            {"or", 2},
            {"=", 1}
        }
    );
}

std::shared_ptr<Expression> MyLanguage::Parser::parse(std::vector<DToken>& tokens, int position) {
	return this->_chainParser->parse(tokens, position);
}