#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#define DEBUG 0

void dbgprint(char *fmt, ...) {
    if (DEBUG)
        printf("%s", fmt);
}


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
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

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
Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // skip space
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strchr("+-*/()", *p)) {
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


// node kinds of ast
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind; // type of node
    Node *lhs;     // left subtree
    Node *rhs;     // right subtree
    int val;       // used when kind is ND_NUM
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *primary();

Node *expr() {
    Node *node = mul();
    
    for (;;) {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = primary();

    for (;;) {
        if (consume('*'))
            node = new_node(ND_MUL, node, primary());
        else if (consume('/'))
            node = new_node(ND_DIV, node, primary());
        else
            return node;
    }
}

Node *primary() {
    // if next token is '(', there should be '(' expr ')'
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }
    // otherwise, number
    return new_node_num(expect_number());
}

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
    }
    printf("  push rax\n");
}

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