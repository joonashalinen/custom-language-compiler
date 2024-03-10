#ifndef MY_LANGUAGE_X86_ASSEMBLY_GENERATOR_HH
#define MY_LANGUAGE_X86_ASSEMBLY_GENERATOR_HH

#include "AssemblyGenerator.h"
#include "../ir-generator/TIRCommand.h"
#include <iostream>
#include <cassert>

namespace MyLanguage {
    /**
     * A generator for producing assembly code for x86-64 processors.
     */
    class X86AssemblyGenerator {
        public:
            X86AssemblyGenerator();
            std::string generate(std::map<std::string, std::vector<TIRCommand>> irCommands);
        private:
            AssemblyGenerator _assemblyGenerator;
    };
};

#endif