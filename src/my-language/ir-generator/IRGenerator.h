#ifndef MY_LANGUAGE_IR_GENERATOR_HH
#define MY_LANGUAGE_IR_GENERATOR_HH

#include "../../components/parsing/Expression.h"
#include "../../components/data_structures/FoldableNode.h"
#include "IRCommand.h"
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
        /**
         * The result returned upon generating the IR code for an expression. The result 
         * contains firstly a variable name, which may be a variable that contains 
         * the result of the expression's calculations. The second value is the list of 
         * IR commands generated.
         */
        using TGeneratorResult = std::pair<TIRVariable, std::vector<IRCommand>>;

        /**
         * Type of a function that is used for generating the IR for a specific expression type. 
         * The function takes as its first input the this pointer pointing to the IRGenerator class. 
         * The second variable is the Expression that the IR code is being generated for. 
         * The third variable is a list of the results returned by the expression's children upon 
         * having their IR code generated.
         */
        using TExpressionIRGenerator = std::function<
            TGeneratorResult(
                IRGenerator*,
                std::shared_ptr<Expression>, 
                std::vector<TGeneratorResult>
            )
        >;

        public:
            IRGenerator();
            /**
             * Generate the resulting IR commands from the given abstract syntax tree.
             */
            std::vector<IRCommand> generate(std::shared_ptr<Expression> root);
            /**
             * Generates the IR commands for a literal expression.
             */
            TGeneratorResult generateNumber(
                std::shared_ptr<Expression> expression, 
                std::vector<TGeneratorResult> childResults
            );
            /**
             * Generates the IR commands for a chain expression.
             */
            TGeneratorResult generateChain(
                std::shared_ptr<Expression> expression, 
                std::vector<TGeneratorResult> childResults
            );
        private:
            IRCommandFactory _commandFactory;
            std::map<std::string, TExpressionIRGenerator> _irGenerators;
    };
};

#endif