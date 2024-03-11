#ifndef MY_LANGUAGE_IR_GENERATOR_HH
#define MY_LANGUAGE_IR_GENERATOR_HH

#include "TIRCommand.h"
#include "../../components/data_structures/FoldableNode.h"
#include "../../components/data_structures/LinkedMap.h"
#include "../../components/data_structures/BatchStack.h"
#include "IRCommandFactory.h"
#include <map>
#include <stack>
#include <set>

namespace MyLanguage {
    class IRGenerator;
    /**
     * The IR generator of our custom programming language. 
     * Can be used to transform a given abstract syntax tree into 
     * a list of commands in the Intermediate Representation (IR) language.
     */
    class IRGenerator {
        public:
            using TExpression = std::shared_ptr<Expression>;
            struct DGeneratorContext;

            /**
             * Type of a function that is used for generating the IR for a specific expression type. 
             * The function takes as its first input the this pointer pointing to the IRGenerator class. 
             * The second variable is the Expression that the IR code is being generated for. 
             * The third variable is a list of the results returned by the expression's children upon 
             * having their IR code generated.
             */
            using TIRGenerator = std::function<
                DGeneratorContext*(
                    DGeneratorContext*,
                    TExpression
                )
            >;

            /**
             * Type of the context object being passed around when generating 
             * IR commands from the abstract syntax tree by folding over it.
             */
            struct DGeneratorContext {
                std::map<std::string, TIRGenerator>* irGenerators {nullptr};
                IRCommandFactory* commandFactory {nullptr};
                DataStructures::BatchStack<std::string> variableStack {};
                DataStructures::BatchStack<std::string> labelStack {};
                DataStructures::BatchStack<std::string> loopLabelStack {};
                DataStructures::LinkedMap<std::string> symbolTable {};
                std::vector<TIRCommand> irCommands {};
                int loopLevel {0};
            };

            IRGenerator();
            /**
             * Generate the resulting IR commands from the given abstract syntax tree.
             */
            std::vector<TIRCommand> generate(TExpression root);
            /**
             * Map of the IR generating functions for each parse tree expression type.
             */
            std::map<std::string, TIRGenerator>& irGenerators();
        private:
            IRCommandFactory _commandFactory;
            std::map<std::string, TIRGenerator> _irGenerators;
    };
};

#endif