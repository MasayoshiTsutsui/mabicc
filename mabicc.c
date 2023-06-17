#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>


// tokens
typedef enum {
    TK_RESERVED, // non-number
    TK_NUM,      // integers
    TK_EOF,      // end of file
} TokenKind;

typedef struct Token Token;

// token type
struct Token {
    TokenKind kind; // token type
    Token *next;    // next token
    int val;        // integer num when kind is TK_NUM
    char *str;      // string of token
};

// token attended
Token *token;

// input program
char *user_input;


// func for reporting errors
// same ary as printf

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if next token is expected one, read forward
// and return True. Otherwise False.
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// if next token is expected one, read forward
// Otherwise raise error.
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "It's not '%c'.", op);
    token = token->next;
}

// if next token is number, read forward and return the number.
// Otherwise, raise error.
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "It's not a number.");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// make new token and chain it to 'cur'.
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// tokenize input char 'p' and return it.
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // skip space
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(token->str, "Unable to tokenize.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of inputs.\n");
        return 1;
    }

    user_input = argv[1];

    // tokenize
    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // beginning of formula must be a number.
    // check it and output first mov instr.
    printf("  mov rax, %d\n", expect_number());

    // consume tokens like `+ <num>` or `- <num>` 
    // and output assembly
    while (!at_eof()) {
        if (consume('+')) {
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }
    
    printf("  ret\n");
    return 0;
}