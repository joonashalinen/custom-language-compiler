#ifndef MY_LANGUAGE_MODULE_IR_GENERATOR_HH
#define MY_LANGUAGE_MODULE_IR_GENERATOR_HH

#include "TIRCommand.h"
#include "../../components/data_structures/FoldableNode.h"
#include "../../components/data_structures/LinkedMap.h"
#include "../../components/data_structures/BatchStack.h"
#include "IRCommandFactory.h"
#include "IRGenerator.h"
#include <map>
#include <stack>
#include <set>

namespace MyLanguage {
    /**
     * A wrapper for the IR generator of our custom programming language 
     * to provide IR generation for module expressions, which consist of function 
     * definitions.
     */
    class ModuleIRGenerator {
        public:
            ModuleIRGenerator();
            /**
             * Generate the IR commands for each function under the given 
             * module expression.
             */
            std::map<std::string, std::vector<TIRCommand>> generate(
                IRGenerator::TExpression moduleExpression
            );
        private:
            IRGenerator _generator;
    };
};

#endif