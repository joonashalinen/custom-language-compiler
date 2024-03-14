#include "WhileParser.h"

MyLanguage::WhileParser::WhileParser(OperatedChainParser* operatedChainParser)
{
    this->_operatedChainParser = operatedChainParser;

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
    auto mapParser = ((MapParser&) this->_operatedChainParser->parser());

    if (!(mapParser.parsers().contains("break"))) {
        mapParser.parsers().insert({"break", this->_breakParser.get()});
    }
    if (!(mapParser.parsers().contains("continue"))) {
        mapParser.parsers().insert({"continue", this->_continueParser.get()});
    }

	auto result = this->_mainParser->parse(tokens, position);
    this->_restore();
    return result;
}

bool MyLanguage::WhileParser::canParseAt(std::vector<DToken>& tokens, int position)
{
    auto sequence = TokenSequence{tokens};
    sequence.setPosition(position);
    return sequence.peek().value == "while";
}

void MyLanguage::WhileParser::_restore()
{
    auto mapParser = ((MapParser&) this->_operatedChainParser->parser());
    if (mapParser.parsers().contains("break")) {
        mapParser.parsers().erase("break");
    }
    if (mapParser.parsers().contains("continue")) {
        mapParser.parsers().erase("continue");
    }
}