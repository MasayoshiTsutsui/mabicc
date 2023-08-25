#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "mabicc.hpp"
#include "debug.hpp"

// token attended
Token *token;

// input program
char *user_input;

int main(int argc, char **argv) {
    if (argc != 2) {
        error("invalid number of inputs.\n");
        return 1;
    }

    // tokenize & parse
    user_input = argv[1];
    token = tokenize();
    program();

    // prefix of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // prologue
    // allocate stack space for 
    // 26 local variable
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    // generate code from formula one by one
    for (int i = 0; code[i]; i++) {
        gen(code[i]);

        // there should be 1 value on stack top
        // after executing a code line
        printf("  pop rax\n");
    }

    // epilogue
    // result of last formula is on RAX
    // ret returns it
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}