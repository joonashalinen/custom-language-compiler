#ifndef MY_LANGUAGE_ASSEMBLY_GENERATOR_HH
#define MY_LANGUAGE_ASSEMBLY_GENERATOR_HH

#include "../ir-generator/IRCommand.h"

namespace MyLanguage {
    class AssemblyGenerator {
        public:
            AssemblyGenerator();
            std::string generate(std::vector<IRCommand> irCommands);
        private:
            
    };
};

#endif