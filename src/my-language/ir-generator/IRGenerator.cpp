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
         * IR generation function for a return expression inside a function definition.
         */
        IRGenerator::DGeneratorContext* generateReturn(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // If the return statement returns a proper value instead of returning nothing.
            if (expression->children().size() > 0) {
                // Get the variable storing the return value.
                auto irVariable = context->variableStack.pop(1).at(0);
                // Generate the IR command for the return statement.
                auto command = context->commandFactory->createWriteFunctionReturn(irVariable);
                context->irCommands.insert(context->irCommands.end(), command);
            }  else {
                // If the return statement does not return a value, we make it return 0.
                auto irVariable = context->commandFactory->nextVariable();
                auto loadIntCommand = context->commandFactory->createLoadIntConst(0, irVariable);
                auto returnCommand = context->commandFactory->createWriteFunctionReturn(irVariable);
                context->irCommands.insert(context->irCommands.end(), loadIntCommand);
                context->irCommands.insert(context->irCommands.end(), returnCommand);
            }
            return context;
        }

        /**
         * IR generation function for a break expression inside a loop.
         */
        IRGenerator::DGeneratorContext* generateBreak(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->loopLevel == 0) {
                throw std::runtime_error("Encountered a break statement outside of a loop.");
            } else {
                auto endLabel = context->loopLabelStack.stack().top();
                // Generate jump command that jumps past the loop.
                auto jump = context->commandFactory->createJump(endLabel);
                context->irCommands.insert(context->irCommands.end(), jump);

                // If the break statement has a return value.
                if (expression->children().size() > 0) {
                    // Generate command to copy the return value to the variable storing the result of the loop.
                    auto breakResultVar = context->variableStack.pop();
                    auto loopResultVar = context->loopVariableStack.stack().top();
                    auto copy = context->commandFactory->createCopy(breakResultVar, loopResultVar);
                    context->irCommands.insert(context->irCommands.end(), copy);
                }

                return context;
            }
        }

        /**
         * IR generation function for a break expression inside a loop.
         */
        IRGenerator::DGeneratorContext* generateContinue(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            if (context->loopLevel == 0) {
                throw std::runtime_error("Encountered a continue statement outside of a loop.");
            } else {
                auto continueLabel = context->loopLabelStack.top(2).at(0);
                // Generate jump command that jumps to the condition portion of the loop.
                auto jump = context->commandFactory->createJump(continueLabel);
                context->irCommands.insert(context->irCommands.end(), jump);
                return context;
            }
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
            auto callCommand = context->commandFactory->createCall(
                expression->subTypes().at("name"), 
                argumentVars,
                resultVariable
            );

            context->irCommands.insert(context->irCommands.end(), callCommand);
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
         * Generates the IR commands for the boolean and/or binary operations.
         */
        IRGenerator::DGeneratorContext* inGenerateBoolean(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression,
            int childIndex,
            std::string operation
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
                // Create the label skipping the rest of the boolean expression.
                auto skipLabel = context->commandFactory->nextLabel();
                // Create the label ending the whole boolean expression. We wish to jump here from the end 
                // of the right variable's section.
                auto endLabel = context->commandFactory->nextLabel();
                // Create the conditional jump command for checking where we should jump next.
                auto condJump = operation == "and" ? 
                    context->commandFactory->createCondJump(leftResultVar, rightLabel, skipLabel) : 
                    context->commandFactory->createCondJump(leftResultVar, skipLabel, rightLabel);

                context->irCommands.insert(context->irCommands.end(), condJump);
                context->irCommands.insert(context->irCommands.end(), rightIRLabel);
                context->labelStack.push({skipLabel, endLabel});

            } else if (childIndex == 1) {
                // If we have just generated the IR commands for the right side of the boolean expression.

                // Get the variable storing the result of the right side.
                auto rightResultVar = context->variableStack.pop();
                // Create the variable for storing the result of the whole boolean expression.
                auto resultIRVar = context->commandFactory->nextVariable();
                // Create the command for copying the right result into the final result variable.
                auto copy = context->commandFactory->createCopy(rightResultVar, resultIRVar);
                
                auto endLabel = context->labelStack.pop();
                auto skipLabel = context->labelStack.pop();

                // Create the command for jumping past the skip-section.
                auto jump1 = context->commandFactory->createJump(endLabel);
                auto jump2 = context->commandFactory->createJump(endLabel);
                // Create the commands for the skip-section of the boolean expression.
                auto skipIRLabel = context->commandFactory->createLabel(skipLabel);
                auto loadSkipResult = context->commandFactory->createLoadBoolConst(
                    operation == "and" ? "false" : "true", 
                    resultIRVar
                );
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
         * Does post-order IR generation logic for while-loops.
         */
        IRGenerator::DGeneratorContext* generateWhile(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            context->loopLevel = context->loopLevel - 1;
            // Return the result of the loop.
            auto resultVar = context->loopVariableStack.pop();
            context->variableStack.push({resultVar});
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
            // Create the IR label beginning the condition portion of the while-do block.
            auto conditionLabel = context->commandFactory->nextLabel();
            auto conditionIRLabel = context->commandFactory->createLabel(conditionLabel);
            // Create the variable for storing the result of the loop.
            auto resultVar = context->commandFactory->nextVariable();
            // 0 (Unit) is the default result of the loop. Thus, we create a command for setting this value to the result variable. 
            // This value will get overriden if there is a break statement within the loop that returns a value.
            auto writeResult = context->commandFactory->createLoadIntConst("0", resultVar);
            
            context->irCommands.insert(context->irCommands.end(), writeResult);
            context->irCommands.insert(context->irCommands.end(), conditionIRLabel);
            context->labelStack.stack().push(conditionLabel);
            context->loopLevel = context->loopLevel + 1;
            context->loopVariableStack.stack().push(resultVar);
            return context;
        }

        /**
         * Does pre-order IR generation logic for function statements.
         */
        IRGenerator::DGeneratorContext* preGenerateFunction(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Generate the label beginning the function.
            auto label = context->commandFactory->createFunctionLabel(expression->subTypes().at("name"));
            context->irCommands.insert(context->irCommands.end(), label);
            return context;
        }

        /**
         * Does post-order IR generation logic for function definitions.
         */
        IRGenerator::DGeneratorContext* generateFunctionDefinition(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression
        ) {
            // Check if there is a return statement within the definition.
            auto returns = std::count_if(expression->children().begin(), expression->children().end(), [](auto child) {
                return child->type() == "return";
            });
            // If there is no return statement.
            if (returns == 0) {
                // Generate IR code for returning 0.
                auto irVariable = context->commandFactory->nextVariable();
                auto loadIntCommand = context->commandFactory->createLoadIntConst("0", irVariable);
                auto returnCommand = context->commandFactory->createWriteFunctionReturn(irVariable);
                context->irCommands.insert(context->irCommands.end(), loadIntCommand);
                context->irCommands.insert(context->irCommands.end(), returnCommand);
            }
            return context;
        }

        /**
         * Does in-order IR generation logic for function parameter lists.
         */
        IRGenerator::DGeneratorContext* inGenerateFunctionParameterList(
            IRGenerator::DGeneratorContext* context,
            std::shared_ptr<Expression> expression,
            int childIndex
        ) {
            auto parameter = expression->children().at(childIndex);
            auto parameterName = parameter->subTypes().at("name");
            auto irVariable = context->commandFactory->nextVariable();
            auto command = context->commandFactory->createLoadFunctionParam(childIndex, irVariable);
            context->symbolTable.insert(parameterName, irVariable);
            context->irCommands.insert(context->irCommands.end(), command);
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

                // Get the label beginning the condition portion of the loop.
                auto conditionLabel = context->labelStack.stack().top();
                // Add the condition label and end label to the stack of loop labels. This stack is necessary 
                // for implementing break and continue statements within the do-portion.
                context->loopLabelStack.push({conditionLabel});
                context->loopLabelStack.push({endLabel});

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

                // Remove the loop labels for this while loop, since 
                // break and continue statements cannot appear outside the do-portion for this loop.
                context->loopLabelStack.pop(2);
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
            } else if (expression->type() == "function") {
                return preGenerateFunction(context, expression);
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
            } else if (
                expression->type() == "binary-operator" && 
                (expression->subTypes().at("name") == "and" || expression->subTypes().at("name") == "or")
            ) {
                return inGenerateBoolean(context, expression, childIndex, expression->subTypes().at("name"));
            } else if (expression->type() == "function-parameter-list") {
                return inGenerateFunctionParameterList(context, expression, childIndex);
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
        this->_irGenerators.insert({"unary-operator", IRGenerators::generateFunctionCall});
        this->_irGenerators.insert({"function-call", IRGenerators::generateFunctionCall});
        this->_irGenerators.insert({"variable-declaration", IRGenerators::generateVariableDeclaration});
        this->_irGenerators.insert({"identifier", IRGenerators::generateIdentifier});
        this->_irGenerators.insert({"block", IRGenerators::generateBlock});
        this->_irGenerators.insert({"parenthetical", IRGenerators::nullGenerator});
        this->_irGenerators.insert({"if", IRGenerators::generateIf});
        this->_irGenerators.insert({"while", IRGenerators::generateWhile});
        this->_irGenerators.insert({"function", IRGenerators::nullGenerator});
        this->_irGenerators.insert({"function-parameter-list", IRGenerators::nullGenerator});
        this->_irGenerators.insert({"function-parameter", IRGenerators::nullGenerator});
        this->_irGenerators.insert({"function-definition", IRGenerators::generateFunctionDefinition});
        this->_irGenerators.insert({"return", IRGenerators::generateReturn});
        this->_irGenerators.insert({"break", IRGenerators::generateBreak});
        this->_irGenerators.insert({"continue", IRGenerators::generateContinue});
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