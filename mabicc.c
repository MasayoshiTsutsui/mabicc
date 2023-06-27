#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "mabicc.h"

#define DEBUG 0

void dbgprint(char *fmt, ...) {
    if (DEBUG)
        printf("%s", fmt);
}

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
    Node *node = expr();

    // prefix of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // track ast and generate codes
    gen(node);

    // there'd be the final result on stack top
    // load it on rax and return it
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}