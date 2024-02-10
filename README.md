# Introduction
This repository is for a compiler of a simple custom programming language. The programming language is a part of the course project of [this course](https://hy-compilers.github.io/spring-2024/) at the University of Helsinki.

# The Programming Language
An example of a program written in the language to be implemented is as follows:
``` C++
var n: Int = read_int();
print_int(n);
while n > 1 do {
    if n % 2 == 0 then {
        n = n / 2;
    } else {
        n = 3*n + 1;
    }
    print_int(n);
}
```

# Project Folder Structure
The folder structure of the project's source code consists of three parts. Firstly, most of the functional code of the project exists in a library of reusable classes. These classes exist in the folder src/components. Secondly, the configuration code that uses these reusable classes to produce the compiler of our custom language lives within the folder src/my-language. Finally, third party libraries live in the folder src/libraries. The only third party libraries used in this project are the testing frameworks doctest and catch2.
