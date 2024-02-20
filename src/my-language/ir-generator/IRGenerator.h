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
        using ExpressionIRGenerator = std::function<
            std::vector<IRCommand>(
                IRGenerator*,
                std::shared_ptr<Expression>, 
                std::vector<std::vector<IRCommand>>
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
            std::vector<IRCommand> generateNumber(
                std::shared_ptr<Expression> expression, 
                std::vector<std::vector<IRCommand>> accs
            );
            /**
             * Generates the IR commands for a chain expression.
             */
            std::vector<IRCommand> generateChain(
                std::shared_ptr<Expression> expression, 
                std::vector<std::vector<IRCommand>> accs
            );
        private:
            IRCommandFactory _commandFactory;
            std::map<std::string, ExpressionIRGenerator> _irGenerators;
    };
};

#endif