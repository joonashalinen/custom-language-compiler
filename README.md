# Introduction
This repository is for a compiler of a simple custom programming language. The programming language is a part of the course project of [this course](https://hy-compilers.github.io/spring-2024/) at the University of Helsinki. The compiler is written entirely in C++.

# Implemented features
* Base language (20p)
* Functions (15p)
* Break and continue (5p) + return values for break expressions (2p)
* Pointers (10p)

# Important folders and files
* `src/components` contains reusable classes.
* `src/my-language` contains code that is more specific to this project
* `src/test/end-to-end-cases` contains all end-to-end tests.
* `src/test/end_to_end.py` is the Python script that runs all end-to-end tests.
* `src/main.out` is the prebuilt Linux executable of the compiler program.

## Entry points to compiler stages
* `src/my-language/tokenizer/Tokenizer.cpp` is where the tokenizer is configured.
* `src/my-language/parser/Parser.cpp` is the entry point for the parser.
* `src/my-language/type-checker/TypeChecker.cpp` contains the type checking code.
* `src/my-langauge/ir-generator/IRGenerator.cpp` and `ModuleIRGenerator.cpp` contain the IR generation code.
* `src/my-language/assembly-generator/AssemblyGenerator.cpp` and `X86AssemblyGenerator.cpp` contain the code for assembly generation.

# Building the project
The compiler is already prebuilt in the file `src/main.out`. However, if you want to rebuild the executable yourself, you need to have gcc 13.1 installed and available for use via the shell command `g++-13`. Then, you can build the project on Linux by running `make -f make_linux.mk all` in the `src` folder.

# End-to-end tests
The end-to-end tests located in `src/test/end-to-end-cases` are of the form:
```
<code written in the custom language>
!expect!
<expected prints>
```
or if an error is expected then of the form
```
<code written in the custom language>
!expect-compiler-error!
<substring that we expect to be contained in the compiler error>
```

## Running the end-to-end tests
You can run the end-to-end tests by executing the shell command `python3 end_to_end.py` in the `src/test` folder.
