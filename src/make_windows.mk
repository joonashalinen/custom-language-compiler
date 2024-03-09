# A sample Makefile
# This Makefile demonstrates and explains 
# Make Macros, Macro Expansions,
# Rules, Targets, Dependencies, Commands, Goals
# Artificial Targets, Pattern Rule, Dependency Rule.

# Comments start with a # and go to the end of the line.

# Here is a simple Make Macro.
LINK_TARGET = main.exe

#Test target.
TEST_TARGET = test.exe

MAIN_OBJ = main.o

OBJS =  \
	my-language/tokenizer/Tokenizer.o \
	my-language/parser/Parser.o \
	my-language/parser/ChainParser.o \
	my-language/parser/IfParser.o \
	my-language/parser/WhileParser.o \
	my-language/parser/FunctionCallParser.o \
	my-language/parser/VariableDeclarationParser.o \
	my-language/parser/FunctionParser.o \
	my-language/parser/ModuleParser.o \
	my-language/parser/ExpressionFactory.o \
	my-language/ir-generator/IRGenerator.o \
	my-language/ir-generator/TIRCommand.o \
	my-language/ir-generator/IRCommandFactory.o \
	my-language/assembly-generator/AssemblyGenerator.o \
	my-language/assembly-generator/X86AssemblyGenerator.o \
	components/tokenization/Tokenization.o \
	components/text/RegexPattern.o \
	components/parsing/OperatedChainParser.o \
	components/parsing/TokenSequence.o \
	components/parsing/MapParser.o \
	components/parsing/UnaryParser.o \
	components/parsing/BinaryParser.o \
	components/parsing/LiteralParser.o \
	components/parsing/Expression.o \
	components/parsing/ParentheticalParser.o \
	components/parsing/ChainParser.o \
	components/parsing/SkeletonParser.o \
	components/parsing/ConflictParser.o \
	components/parsing/ListParser.o \
	components/structured-language/VariableStack.o \

TEST_OBJS = \
	my-language/tokenizer/test/Tokenizer.test.o \
	my-language/parser/test/Parser.test.o \
	my-language/ir-generator/test/IRGenerator.test.o \
	my-language/assembly-generator/test/X86AssemblyGenerator.test.o \
	components/parsing/test/OperatedChainParser.test.o \

TEST_OBJ = \
	my-language/parser/test/Parser.test.o
# Here is a Make Macro defined by two Macro Expansions.
# A Macro Expansion may be treated as a textual replacement of the Make Macro.
# Macro Expansions are introduced with $ and enclosed in (parentheses).
REBUILDABLES = $(MAIN_OBJ) $(OBJS) $(LINK_TARGET)

# Here is a simple Rule (used for "cleaning" your build environment).
# It has a Target named "clean" (left of the colon ":" on the first line),
# no Dependencies (right of the colon),
# and two Commands (indented by tabs on the lines that follow).
# The space before the colon is not required but added here for clarity.
clean : 
	rm $(OBJS) $(TEST_OBJS) $(MAIN_OBJ)
	echo Clean done

# There are two standard Targets your Makefile should probably have:
# "all" and "clean", because they are often command-line Goals.
# Also, these are both typically Artificial Targets, because they don't typically
# correspond to real files named "all" or "clean".  

# The rule for "all" is used to incrementally build your system.
# It does this by expressing a dependency on the results of that system,
# which in turn have their own rules and dependencies.
all : $(LINK_TARGET)
	echo All done

test : $(TEST_TARGET)
	echo Tests done

# There is no required order to the list of rules as they appear in the Makefile.
# Make will build its own dependency tree and only execute each rule only once
# its dependencies' rules have been executed successfully.

# Here is a Rule that uses some built-in Make Macros in its command:
# $@ expands to the rule's target, in this case "test_me.exe".
# $^ expands to the rule's dependencies.
$(LINK_TARGET) : $(MAIN_OBJ) $(OBJS)
	g++ -Wall -Werror -ansi -pedantic-errors -g -o $@ $^

# Compiles the test target.
$(TEST_TARGET) : $(OBJS) $(TEST_OBJ)
	g++ -Wall -Werror -ansi -pedantic-errors -g -o $@ $^

# Here is a Pattern Rule, often used for compile-line.
# It says how to create a file with a .o suffix, given a file with a .cpp suffix.
# The rule's command uses some built-in Make Macros:
# $@ for the pattern-matched target
# $< for the pattern-matched dependency
%.o : %.cpp
	g++ -Wall -Werror -ansi -pedantic-errors -std=c++20 -g -o $@ -c $<

# These are Dependency Rules, which are rules without any command.
# Dependency Rules indicate that if any file to the right of the colon changes,
# the target to the left of the colon should be considered out-of-date.
# The commands for making an out-of-date target up-to-date may be found elsewhere
# (in this case, by the Pattern Rule above).
# Dependency Rules are often used to capture header file dependencies.
# Main.o : Main.h

# Alternatively to manually capturing dependencies, several automated
# dependency generators exist.  Here is one possibility (commented out)...
# %.dep : %.cpp
#   g++ -M $(FLAGS) $< > $@
# include $(OBJS:.o=.dep)