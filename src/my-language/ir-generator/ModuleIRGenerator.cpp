#include "ModuleIRGenerator.h"

MyLanguage::ModuleIRGenerator::ModuleIRGenerator():
    _generator(MyLanguage::IRGenerator{}) {
}

std::map<std::string, std::vector<MyLanguage::TIRCommand>> MyLanguage::ModuleIRGenerator::generate(
    IRGenerator::TExpression moduleExpression
)
{
    auto irCommands = std::map<std::string, std::vector<MyLanguage::TIRCommand>>{};
	auto functions = moduleExpression->children();
    std::for_each(functions.begin(), functions.end(), [&irCommands, this](auto f) {
        irCommands.insert({f->subTypes().at("name"), this->_generator.generate(f)});
    });
    return irCommands;
}
