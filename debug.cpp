#include <stdio.h>
#include "mabicc.hpp"
#define DEBUG 0

void dbgprint(char *fmt, ...) {
    if (DEBUG)
        printf("%s", fmt);
}

void tkprint() {
    extern Token *token;
    Token *cur = token;
    while (cur) {
        printf("%.*s\n", cur->len, cur->str);
        cur = cur->next;
    }
}

const char *getNodeName(NodeKind nk) {
    switch (nk) {
        case ND_ADD: return "ADD";
        case ND_SUB: return "SUB";
        case ND_MUL: return "MUL";
        case ND_DIV: return "DIV";
        case ND_EQ: return "EQ";
        case ND_NE: return "NE";
        case ND_LT: return "LT";
        case ND_LE: return "LE";
        case ND_ASSIGN: return "ASSIGN";
        case ND_LVAR: return "LVAR";
        case ND_NUM: return "NUM";
        case ND_RETURN: return "RETURN";
        case ND_IF: return "IF";
        default: return NULL;
    }
}

void dfs_print(Node *node, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%s ", getNodeName(node->kind));
    if (node->kind == ND_NUM)
        printf("%d", node->val);
    printf("\n");

    if (!node->ary0)
        return;
    dfs_print(node->ary0, depth + 1);

    if (!node->ary1)
        return;
    dfs_print(node->ary1, depth + 1);

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("\n");
}

void ndprint() {
    printf("\nNODE TREE::\n\n");
    for (int i = 0; code[i]; i++) {
        dfs_print(code[i], 0);
    }
}