#ifndef MY_LANGUAGE_IR_GENERATOR_HH
#define MY_LANGUAGE_IR_GENERATOR_HH

#include "TIRCommand.h"
#include "../../components/data_structures/FoldableNode.h"
#include "IRCommandFactory.h"
#include <map>

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
            /**
             * The result returned upon generating the IR code for an expression. The result 
             * contains firstly a variable name, which may be a variable that contains 
             * the result of the expression's calculations. The second value is the list of 
             * IR commands generated.
             */
            using TGeneratorResult = std::pair<TIRVariable, std::vector<TIRCommand>>;
            using TGeneratorResults = std::vector<TGeneratorResult>;

            /**
             * Type of a function that is used for generating the IR for a specific expression type. 
             * The function takes as its first input the this pointer pointing to the IRGenerator class. 
             * The second variable is the Expression that the IR code is being generated for. 
             * The third variable is a list of the results returned by the expression's children upon 
             * having their IR code generated.
             */
            using TExpressionIRGenerator = std::function<
                TGeneratorResult(
                    IRCommandFactory&,
                    std::shared_ptr<Expression>, 
                    std::vector<TGeneratorResult>
                )
            >;

            IRGenerator();
            /**
             * Generate the resulting IR commands from the given abstract syntax tree.
             */
            std::vector<TIRCommand> generate(TExpression root);
        private:
            IRCommandFactory _commandFactory;
            std::map<std::string, TExpressionIRGenerator> _irGenerators;
    };
};

#endif