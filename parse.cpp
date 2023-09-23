#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include "mabicc.hpp"
#include "debug.hpp"

// input program
extern char *user_input;

// func for reporting errors
// same ary as printf
void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    if (pos > 0)
        fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if next token is expected one, read forward
// and return True. Otherwise False.
bool consume(Token* &token, std::string op) {
    if (token->kind != TK_RESERVED ||
        op.size() != token->len ||
        !std::equal(op.begin(), op.end(), token->str))
        return false;
    token = token->next;
    return true;
}

Token *consume_tk(Token* &token, TokenKind tk) {
    if (token->kind != tk)
        return NULL; 
    Token *tmp = token;
    token = token->next;
    return tmp;
}

// if next token is expected one, read forward
// Otherwise raise error.
void expect(Token* &token, std::string op) {
    if (token->kind != TK_RESERVED ||
        op.size() != token->len ||
        !std::equal(op.begin(), op.end(), token->str))
        error_at(token->str, "It's not \"%s\" but \"%s\"", op, token->str);
    token = token->next;
}

// if next token is number, read forward and return the number.
// Otherwise, raise error.
int expect_number(Token* &token) {
    if (token->kind != TK_NUM)
        error_at(token->str, "It's not a number.");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(Token* &token) {
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Node *ary0, Node *ary1) {
    Node *node = (Node*)calloc(1, sizeof(Node));
    node->kind = kind;
    node->ary0 = ary0;
    node->ary1 = ary1;
    return node;
}

Node *new_node_num(int val) {
    Node *node = (Node*)calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

void program(Token* &token);
Node *stmt(Token* &token);
Node *expr(Token* &token);
Node *assign(Token* &token);
Node *equality(Token* &token);
Node *relational(Token* &token);
Node *add(Token* &token);
Node *mul(Token* &token);
Node *unary(Token* &token);
Node *primary(Token* &token);

LVar *find_lvar(Token *tok);

Node *code[100];

// local variables
LVar *locals = NULL;

void program(Token* &token) {
    int i = 0;
    while(!at_eof(token)) {
        code[i++] = stmt(token);
    }
    code[i] = NULL;
}

Node *stmt(Token* &token) {
    Node *node;
    Node *cur;

    if (consume_tk(token, TK_FOR)) {
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect(token, "(");
        if (!consume(token, ";")) {
            node->ary0 = expr(token);
            expect(token, ";");
        }
        else // if 0th ary is empty: for (;*;*)
            node->ary0 = NULL;

        if (!consume(token, ";")) {
            node->ary1 = expr(token);
            expect(token, ";");
        }
        else // if 1st ary is empty: for (*;;*)
            node->ary1 = NULL;

        if (!consume(token, ")")) {
            node->ary2 = expr(token);
            expect(token, ")");
        }
        else // if 2nd ary is empty: for (*;*;)
            node->ary2 = NULL;
        node->ary3 = stmt(token);
    } else if (consume_tk(token, TK_WHILE)) {
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect(token, "(");
        node->ary0 = expr(token);
        expect(token, ")");
        node->ary1 = stmt(token);
    } else if (consume_tk(token, TK_IF)) {
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect(token, "(");
        node->ary0 = expr(token);
        expect(token, ")");
        node->ary1 = stmt(token);
        if (consume_tk(token, TK_ELSE))
            node->ary2 = stmt(token);
        else
            node->ary2 = NULL;
    } else if (consume_tk(token, TK_RETURN)) {
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->ary0 = expr(token);
        expect(token, ";");
    } else if (consume(token, "{")) {
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        cur = node;
        while (!consume(token, "}")) {
            cur->next = stmt(token);
            cur = cur->next;
        }
        cur->next = NULL;
    } else {
        node = expr(token);
        expect(token, ";");
    }
    return node;
}

Node *expr(Token* &token) {
    return assign(token);
}

Node *assign(Token* &token) {
    Node *node = equality(token);

    if (consume(token, "="))
        node = new_node(ND_ASSIGN, node, assign(token));
    return node;
}

Node *equality(Token* &token) {
    Node *node = relational(token);

    for (;;) {
        if (consume(token, "=="))
            node = new_node(ND_EQ, node, relational(token));
        else if (consume(token, "!="))
            node = new_node(ND_NE, node, relational(token));
        else
            return node;
    }
}

Node *relational(Token* &token) {
    Node *node = add(token);

    for (;;) {
        if (consume(token, "<"))
            node = new_node(ND_LT, node, add(token));
        else if (consume(token, "<="))
            node = new_node(ND_LE, node, add(token));
        else if (consume(token, ">"))
            node = new_node(ND_LT, add(token), node);
        else if (consume(token, ">="))
            node = new_node(ND_LE, add(token), node);
        return node;
    }
}

Node *add(Token* &token) {
    Node *node = mul(token);
    
    for (;;) {
        if (consume(token, "+"))
            node = new_node(ND_ADD, node, mul(token));
        else if (consume(token, "-"))
            node = new_node(ND_SUB, node, mul(token));
        else
            return node;
    }
}

Node *mul(Token* &token) {
    Node *node = unary(token);

    for (;;) {
        if (consume(token, "*"))
            node = new_node(ND_MUL, node, unary(token));
        else if (consume(token, "/"))
            node = new_node(ND_DIV, node, unary(token));
        else
            return node;
    }
}

Node *unary(Token* &token) {
    if (consume(token, "+"))
        return unary(token);
    if (consume(token, "-"))
        return new_node(ND_SUB, new_node_num(0), unary(token));
    return primary(token);
}

Node *primary(Token* &token) {
    // if next token is '(', there should be '(' expr ')'
    if (consume(token, "(")) {
        Node *node = expr(token);
        expect(token, ")");
        return node;
    }
    Token *tok = consume_tk(token, TK_IDENT);
    if (tok) {
        Node *node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = (LVar*)calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            if (locals)
                lvar->offset = locals->offset + 8;
            else // when it's the first lvar 
                lvar->offset = 0;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }
    return new_node_num(expect_number(token));
}

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(var->name, tok->str, var->len))
            return var;
    }
    return NULL;
}