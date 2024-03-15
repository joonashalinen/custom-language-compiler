#include "WhileParser.h"

MyLanguage::WhileParser::WhileParser(OperatedChainParser* operatedChainParser):
    _operatedChainParser(operatedChainParser), _parseLevel(0) {
    this->_breakParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "break", 
            {
                {"token-value", "break"},
                {"trail", ""},
                {"expression", "E"}
            },
            {
                {"E", this->_operatedChainParser}
            }
        }
    );

    this->_continueParser = std::unique_ptr<LiteralParser>(new LiteralParser{"continue"});

    this->_mainParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "while", 
            {
                {"token-value", "while"},
                {"expression", "E"},
                {"token-value", "do"},
                {"expression", "E"}
            },
            std::map<std::string, IParseable*>{
                {"E", this->_operatedChainParser}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::WhileParser::parse(std::vector<DToken>& tokens, int position)
{
    auto& mapParser = ((MapParser&) this->_operatedChainParser->parser());

    if (this->_parseLevel == 0) {
        mapParser.parsers().insert({"break", this->_breakParser.get()});
        mapParser.parsers().insert({"continue", this->_continueParser.get()});
    }
    this->_parseLevel = this->_parseLevel + 1;

	auto result = this->_mainParser->parse(tokens, position);
 
    this->_parseLevel = this->_parseLevel - 1;
    if (this->_parseLevel == 0) {
        mapParser.parsers().erase("break");
        mapParser.parsers().erase("continue");
    }
    return result;
}

bool MyLanguage::WhileParser::canParseAt(std::vector<DToken>& tokens, int position)
{
    auto sequence = TokenSequence{tokens};
    sequence.setPosition(position);
    return sequence.peek().value == "while";
}