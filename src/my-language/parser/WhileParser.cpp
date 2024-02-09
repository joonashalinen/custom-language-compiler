#include "WhileParser.h"

MyLanguage::WhileParser::WhileParser(OperatedChainParser* operatedChainParser)
{
    this->_skeletonParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "while", 
            {
                {"token-value", "while"},
                {"expression", "E"},
                {"token-value", "do"},
                {"expression", "E"}
            },
            std::map<std::string, IParseable*>{
                {"E", operatedChainParser}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::WhileParser::parse(std::vector<DToken>& tokens, int position)
{
	return this->_skeletonParser->parse(tokens, position);
}
