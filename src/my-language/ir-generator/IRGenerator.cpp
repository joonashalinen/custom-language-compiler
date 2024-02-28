#include "IRGenerator.h"
#include <iostream>
#include <cassert>

namespace MyLanguage {

    /**
     * Functions for generating IR commands for different types of expressions.
     */
    namespace IRGenerators {
    
        /**
         * Generator that does nothing. Useful for passing over abstract syntax tree nodes that 
         * have no meaning for IR generation.
         */
        IRGenerator::DGeneratorContext* nullGenerator(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            return context;
        }

        /**
         * Generates the IR commands for a number literal expression.
         */
        IRGenerator::DGeneratorContext* generateNumber(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto number = expression->rootToken().value;
            auto variable = context->commandFactory->nextVariable();
            auto command = context->commandFactory->createLoadIntConst(number, variable);
            context->variableStack.push({variable});
            context->irCommands.insert(context->irCommands.end(), command);
            return context;
        }

        /**
         * Generates the IR commands for a boolean literal expression.
         */
        IRGenerator::DGeneratorContext* generateBoolean(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto booleanValue = expression->rootToken().value;
            auto variable = context->commandFactory->nextVariable();
            auto command = context->commandFactory->createLoadBoolConst(booleanValue, variable);
            context->variableStack.push({variable});
            context->irCommands.insert(context->irCommands.end(), command);
            return context;
        }

        /**
         * IR generation function for a variable declaration expression.
         */
        IRGenerator::DGeneratorContext* generateVariableDeclaration(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            assert(expression->subTypes().contains("name"));
            auto variableName = expression->subTypes().at("name");
            // The value on the right side of the variable is given by the IR variable
            // at the top of the variable stack.
            auto rightVariable = (context->variableStack.pop(1)).at(0);
            // Create the IR variable for the newly created variable.
            auto irVariable = context->commandFactory->nextVariable();
            // Insert the newly created variable to the symbol table and the variable stack.
            context->symbolTable.insert(variableName, irVariable);
            context->variableStack.push({irVariable});
            // Assign the value on the right side to the newly created variable.
            auto command = context->commandFactory->createCopy(rightVariable, irVariable);
            context->irCommands.insert(context->irCommands.end(), command);
            return context;
        }

        /**
         * IR generation function for an identifier expression.
         */
        IRGenerator::DGeneratorContext* generateIdentifier(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            auto name = expression->subTypes().at("literal-value");
            auto irVariable = context->symbolTable.at(name);
            context->variableStack.push({irVariable});
            return context;
        }

        /**
         * IR generation function for a block expression.
         */
        IRGenerator::DGeneratorContext* generateBlock(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Leave the block's local variable scope.
            context->symbolTable.popFront();
            return context;
        }

        /**
         * Generates the IR commands for a chain expression.
         */
        IRGenerator::DGeneratorContext* generateChain(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (expression->children().size() > 0) {
                auto irVariables = context->variableStack.pop(expression->children().size());
                std::string lastIRVariable = irVariables.back();
                context->variableStack.push({lastIRVariable});
            }
            return context;
        }

        /**
         * Generates the IR commands for a function call expression. 
         * Unary and binary operations other than '=' are also interpreted as function calls.
         */
        IRGenerator::DGeneratorContext* generateFunctionCall(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            assert(expression->subTypes().contains("name"));

            // Gather the variable names storing the argument values.
            auto argumentVars = context->variableStack.pop(expression->children().size());
            // Reserve the next variable we want to store the function call's result in.
            auto resultVariable = context->commandFactory->nextVariable();
            // Create the IR command for the function call.
            auto command = context->commandFactory->createCall(
                expression->subTypes().at("name"), 
                argumentVars,
                resultVariable
            );

            context->irCommands.insert(context->irCommands.end(), command);
            context->variableStack.push({resultVariable});

            return context;
        }

        /**
         * Generates the IR commands for the '=' binary operation.
         */
        IRGenerator::DGeneratorContext* generateAssignment(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            assert(expression->children().size() == 2);
            assert(expression->children().at(0)->type() == "identifier");
            // Name of the variable being assigned to.
            auto variableName = expression->children().at(0)->subTypes().at("literal-value");
            // Name of the corresponding IR variable.
            auto leftIRVariable = context->symbolTable.at(variableName);
            // The value on the right side of the variable is given by the IR variable
            // at the top of the variable stack.
            auto rightIRVariable = (context->variableStack.pop(1)).at(0);
            // Assign the value on the right side to the newly created variable.
            auto command = context->commandFactory->createCopy(rightIRVariable, leftIRVariable);
            context->irCommands.insert(context->irCommands.end(), command);
            // Push Unit to the variable stack. Assignment operations 
            // always return Unit instead of an actual value.
            context->variableStack.push({"Unit"});
            return context;
        }

        /**
         * Generates the IR commands for the boolean 'and' binary operation.
         */
        IRGenerator::DGeneratorContext* inGenerateAnd(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression,
            int childIndex
        ) {
            assert(expression->children().size() == 2);
            
            // If we have just generated the IR commands for the left side of the and-expression.
            if (childIndex == 0) {
                // Get the variable storing the result of the left side.
                auto leftResultVar = context->variableStack.pop();
                // Create the label we wish to move to in the case the left result is true.
                // This is the label beginning the section where the right side of the 
                // and-expression is evaluated.
                auto rightLabel = context->commandFactory->nextLabel();
                auto rightIRLabel = context->commandFactory->createLabel(rightLabel);
                // Create the label skipping the rest of the and-expression. In case the result of the left side 
                // is false, we wish to skip the rest of the and-expression and return false.
                auto skipLabel = context->commandFactory->nextLabel();
                // Create the label ending the whole and-expression. We wish to jump here from the end 
                // of the right variable's section.
                auto endLabel = context->commandFactory->nextLabel();
                // Create the conditional jump command for checking where we should jump next.
                auto condJump = context->commandFactory->createCondJump(leftResultVar, rightLabel, skipLabel);

                context->irCommands.insert(context->irCommands.end(), condJump);
                context->irCommands.insert(context->irCommands.end(), rightIRLabel);
                context->labelStack.push({skipLabel, endLabel});

            } else if (childIndex == 1) {
                // If we have just generated the IR commands for the right side of the and-expression.

                // Get the variable storing the result of the right side.
                auto rightResultVar = context->variableStack.pop();
                // Create the variable for storing the result of the whole and-expression.
                auto resultIRVar = context->commandFactory->nextVariable();
                // Create the command for copying the right result into the final result variable.
                // The right result is logically equivalent to the final result of the whole and-expression.
                auto copy = context->commandFactory->createCopy(rightResultVar, resultIRVar);
                
                auto endLabel = context->labelStack.pop();
                auto skipLabel = context->labelStack.pop();

                // Create the command for jumping past the skip-section.
                auto jump1 = context->commandFactory->createJump(endLabel);
                auto jump2 = context->commandFactory->createJump(endLabel);
                // Create the commands for the skip-section of the and-expression.
                auto skipIRLabel = context->commandFactory->createLabel(skipLabel);
                auto loadSkipResult = context->commandFactory->createLoadBoolConst("false", resultIRVar);
                // Create the end label command.
                auto endIRLabel = context->commandFactory->createLabel(endLabel);

                context->irCommands.insert(context->irCommands.end(), copy);
                context->irCommands.insert(context->irCommands.end(), jump1);
                context->irCommands.insert(context->irCommands.end(), skipIRLabel);
                context->irCommands.insert(context->irCommands.end(), loadSkipResult);
                context->irCommands.insert(context->irCommands.end(), jump2);
                context->irCommands.insert(context->irCommands.end(), endIRLabel);

                context->variableStack.push({resultIRVar});
            }
            
            return context;
        }

        /**
         * Generates IR commands for binary operators.
         */
        IRGenerator::DGeneratorContext* generateBinaryOperator(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (expression->subTypes().at("name") == "=") {
                return generateAssignment(context, expression);
            } else if (!((std::set<std::string>{"and", "or"}).contains(expression->subTypes().at("name")))) {
                return generateFunctionCall(context, expression);
            } else {
                return context;
            }
        }

        /**
         * Does post-order IR generation logic for if-statements and if-else-statements.
         */
        IRGenerator::DGeneratorContext* generateIf(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Get the label ending the entire if-block.
            auto endLabel = context->labelStack.pop();
            // Create the corresponding IR command.
            auto endIRLabel = context->commandFactory->createLabel(endLabel);
            context->irCommands.insert(context->irCommands.end(), endIRLabel);
            return context;
        }

        /**
         * Generates the IR commands for a in if or an if-else expression during 
         * an in-order traversal of the abstract syntax tree.
         */
        IRGenerator::DGeneratorContext* inGenerateIf(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression,
            int childIndex
        ) {
            // If we have just generated the IR commands for the condition expression.
            if (childIndex == 0) {
                // Get the IR variable storing the result of the condition expression.
                auto conditionIRVariable = context->variableStack.pop();
                // Create the labels to traverse to in case the condition is false / true.
                auto onTrueLabel = context->commandFactory->nextLabel();
                auto onFalseLabel = context->commandFactory->nextLabel();
                // Create the IR command for checking which label to jump to.
                auto condJump = context->commandFactory->createCondJump(
                    conditionIRVariable, 
                    onTrueLabel,
                    onFalseLabel
                );
                // Create the IR command for the label starting the then-statement.
                auto onTrueIRLabel = context->commandFactory->createLabel(onTrueLabel);
                // Add the IR commands to the total list of generated IR commands.
                context->irCommands.insert(context->irCommands.end(), condJump);
                context->irCommands.insert(context->irCommands.end(), onTrueIRLabel);
                // If the if-statement includes an else-statement.
                if (expression->children().size() == 3) {
                    // Create a new label for the end of the entire if-statement.
                    context->labelStack.push({context->commandFactory->nextLabel()});
                }
                // Add the label traversed to in case the condition is false to the stack. 
                // This label is either the beginning of the else statement or the end of the 
                // entire if block, depending on whether the statement includes an else-statement.
                context->labelStack.push({onFalseLabel});

            } else if (childIndex == 1 && expression->children().size() == 3) {
                 // If we have just generated the IR commands for the then-statement 
                // and there is a further else-statement present (in which case the if-statement has 3 children).

                // Generate the IR label for the label beginning the else-statement.
                auto elseLabel = context->labelStack.pop();
                auto irLabel = context->commandFactory->createLabel(elseLabel);

                // Generate the jump statement for skipping past the else-statement 
                // at the end of the then-statement.
                auto skipElseCommand = context->commandFactory->createJump(context->labelStack.stack().top());
                
                context->irCommands.insert(context->irCommands.end(), skipElseCommand);
                context->irCommands.insert(context->irCommands.end(), irLabel);

            }

            return context;
        }

        /**
         * Does pre-order IR generation logic for while-do statements.
         */
        IRGenerator::DGeneratorContext* preGenerateWhile(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Generate the IR label beginning the condition portion of the while-do block.
            auto conditionLabel = context->commandFactory->nextLabel();
            auto conditionIRLabel = context->commandFactory->createLabel(conditionLabel);
            context->irCommands.insert(context->irCommands.end(), conditionIRLabel);
            context->labelStack.stack().push(conditionLabel);
            return context;
        }

        /**
         * Generates the IR commands for a while-do expression during 
         * an in-order traversal of the abstract syntax tree.
         */
        IRGenerator::DGeneratorContext* inGenerateWhile(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression,
            int childIndex
        ) {
            // If we have just generated the IR commands for the condition expression.
            if (childIndex == 0) {
                // Get the IR variable storing the result of the condition expression.
                auto conditionIRVariable = context->variableStack.pop();
                // Create the labels to traverse to in case the condition is false / true.
                auto doLabel = context->commandFactory->nextLabel();
                auto endLabel = context->commandFactory->nextLabel();
                // Create the IR command for checking which label to jump to.
                auto condJump = context->commandFactory->createCondJump(
                    conditionIRVariable, 
                    doLabel,
                    endLabel
                );
                // Create the IR command for the label starting the do-portion.
                auto doIRLabel = context->commandFactory->createLabel(doLabel);
                // Add the IR commands to the total list of generated IR commands.
                context->irCommands.insert(context->irCommands.end(), condJump);
                context->irCommands.insert(context->irCommands.end(), doIRLabel);
                // Add the label skipping the entire while-do block to the stack so it can be used later.
                context->labelStack.push({endLabel});

            } else if (childIndex == 1) {
                 // If we have just generated the IR commands for the do-portion.

                // Get the label skipping the whole while-block.
                auto endLabel = context->labelStack.pop();
                // Get the label beginning the condition.
                auto conditionLabel = context->labelStack.pop();

                // Create the IR command for jumping back to the condition portion.
                auto jumpToCondition = context->commandFactory->createJump(conditionLabel);
                // Create the IR label that skips the whole while-block.
                auto endIRLabel = context->commandFactory->createLabel(endLabel);
                
                context->irCommands.insert(context->irCommands.end(), jumpToCondition);
                context->irCommands.insert(context->irCommands.end(), endIRLabel);
            }

            return context;
        }

        /**
         * Top-level generator for generating IR commands for any given expression.
         */
        IRGenerator::DGeneratorContext* generateAny(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->irGenerators->contains(expression->type())) {
                // Choose the corresponding IR generator function.
                auto generate = context->irGenerators->at(expression->type());
                return generate(context, expression);
            } else {
                throw std::runtime_error("No generator found for type: '" + expression->type() + "'.");
            }
        }

        /**
         * Does any IR generation logic needed when traversing the abstract syntax tree 
         * in pre-order.
         */
        IRGenerator::DGeneratorContext* preGenerateAny(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Create new local variable scope when encountering a block expression.
            if (expression->type() == "block") {
                context->symbolTable.pushFront();
                return context;
            } else if (expression->type() == "while") {
                return preGenerateWhile(context, expression);
            } else {
                return context;
            }
        }

        /**
         * Does any IR generation logic needed when traversing the abstract syntax tree 
         * in in-order.
         */
        IRGenerator::DGeneratorContext* inGenerateAny(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression,
            int childIndex
        ) {
            // Create new local variable scope when encountering a block expression.
            if (expression->type() == "if") {
                return inGenerateIf(context, expression, childIndex);
            } else if (expression->type() == "while") {
                return inGenerateWhile(context, expression, childIndex);
            } else if (expression->type() == "binary-operator" && expression->subTypes().at("name") == "and") {
                return inGenerateAnd(context, expression, childIndex);
            } else {
                return context;
            }
        }
    }

    IRGenerator::IRGenerator()
    {
        this->_irGenerators.insert({"number", IRGenerators::generateNumber});
        this->_irGenerators.insert({"boolean", IRGenerators::generateBoolean});
        this->_irGenerators.insert({"chain", IRGenerators::generateChain});
        this->_irGenerators.insert({"binary-operator", IRGenerators::generateBinaryOperator});
        this->_irGenerators.insert({"function-call", IRGenerators::generateFunctionCall});
        this->_irGenerators.insert({"variable-declaration", IRGenerators::generateVariableDeclaration});
        this->_irGenerators.insert({"identifier", IRGenerators::generateIdentifier});
        this->_irGenerators.insert({"block", IRGenerators::generateBlock});
        this->_irGenerators.insert({"parenthetical", IRGenerators::nullGenerator});
        this->_irGenerators.insert({"if", IRGenerators::generateIf});
        this->_irGenerators.insert({"while", IRGenerators::nullGenerator});
    }

    std::vector<TIRCommand> IRGenerator::generate(std::shared_ptr<Expression> root)
    {
        auto context = IRGenerator::DGeneratorContext{&this->_irGenerators, &this->_commandFactory};
        auto foldable = DataStructures::FoldableNode<IRGenerator::DGeneratorContext*, IRGenerator::TExpression>{root};
        foldable.setPreFolder(IRGenerators::preGenerateAny);
        foldable.setInFolder(IRGenerators::inGenerateAny);

        return foldable.fold(IRGenerators::generateAny, &context)->irCommands;
    }

    std::map<std::string, IRGenerator::TIRGenerator>& IRGenerator::irGenerators()
    {
        return this->_irGenerators;
    }
}