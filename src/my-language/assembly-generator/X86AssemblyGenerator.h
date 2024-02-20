#ifndef MY_LANGUAGE_X86_ASSEMBLY_GENERATOR_HH
#define MY_LANGUAGE_X86_ASSEMBLY_GENERATOR_HH

#include "AssemblyGenerator.h"
#include <iostream>

namespace MyLanguage {
    /**
     * A generator for producing assembly code for x86-64 processors.
     */
    class X86AssemblyGenerator {
        public:
            X86AssemblyGenerator();
            std::string generate(std::vector<IRCommand> irCommands);
            /**
             * Generates the assembly code for a LoadIntConst IR command.
             */
            std::string generateLoadIntConst(IRCommand command);
        private:
            AssemblyGenerator _assemblyGenerator;
    };
};

#endif