#include "IfParser.h"

MyLanguage::IfParser::IfParser(OperatedChainParser* operatedChainParser)
{
    this->_skeletonParser = std::unique_ptr<Parsing::SkeletonParser>(
        new Parsing::SkeletonParser{
            "if", 
            {
                {"token-value", "if"},
                {"expression", "E"},
                {"token-value", "then"},
                {"expression", "E"},
                {"trail", ""},
                {"token-value", "else"},
                {"expression", "E"}
            },
            std::map<std::string, IParseable*>{
                {"E", operatedChainParser}
            }
        }
    );
}

std::shared_ptr<Expression> MyLanguage::IfParser::parse(std::vector<DToken>& tokens, int position)
{
	return this->_skeletonParser->parse(tokens, position);
}
