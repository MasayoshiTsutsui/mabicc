#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include "mabicc.hpp"
#include "debug.hpp"



int main(int argc, char **argv) {
    if (argc != 2) {
        error("invalid number of inputs.\n");
        return 1;
    }

    // tokenize & parse
    const std::string user_input = argv[1];
    Token* token = tokenize(user_input);
    program(token);

    // prefix of assembly
    std::cout << ".intel_syntax noprefix" << std::endl;
    std::cout << ".global main" << std::endl;
    std::cout << "main:" << std::endl;

    // prologue
    // allocate stack space for 
    // 26 local variable
    std::cout << "  push rbp" << std::endl;
    std::cout << "  mov rbp, rsp" << std::endl;
    std::cout << "  sub rsp, 208" << std::endl;

    // generate code from formula one by one
    for (int i = 0; code[i]; i++) {
        gen(code[i]);

        // there should be 1 value on stack top
        // after executing a code line
        std::cout << "  pop rax" << std::endl;
    }

    // epilogue
    // result of last formula is on RAX
    // ret returns it
    std::cout << "  mov rsp, rbp" << std::endl;
    std::cout << "  pop rbp" << std::endl;
    std::cout << "  ret" << std::endl;
    return 0;
}