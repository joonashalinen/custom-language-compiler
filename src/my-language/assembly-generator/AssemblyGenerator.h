#ifndef MY_LANGUAGE_ASSEMBLY_GENERATOR_HH
#define MY_LANGUAGE_ASSEMBLY_GENERATOR_HH

#include "../ir-generator/TIRCommand.h"
#include "../../components/structured-language/VariableStack.h"
#include "../../components/parsing/Expression.h"
#include <functional>
#include <map>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <set>

namespace MyLanguage {
    /**
     * Class that can be used to generate assembly code.
     */
    class AssemblyGenerator {
        public:
            using TExpression = std::shared_ptr<Expression>;
            /**
             * Type of a function that generates assembly code from a given IR command..
             */
            using TGenerator = std::function<std::string(
                StructuredLanguage::VariableStack&,
                std::string indent,
                TIRCommand
            )>;
            AssemblyGenerator();
            /**
             * Generates assembly code from the given list of IR commands.
             */
            std::string generate(std::vector<TIRCommand> irCommands);
            std::string prelude();
            std::string indent();
            std::map<std::string, TGenerator>& generators();
            void setPrelude(std::string prelude);
            void setIndent(std::string indent);
        private:
            /**
             * Contains the assembly generator functions for each type of IRCommand.
             */
            std::map<std::string, TGenerator> _generators;
            /**
             * The assembly declarations and code that precedes the generated program's assembly code.
             */
            std::string _prelude;
            /**
             * The indent string we use for a single indentation level.
             */
            std::string _indent;
            /**
             * Data structure for managing the memory locations of variables.
             */
            StructuredLanguage::VariableStack _variableStack;
    };
};

#endif