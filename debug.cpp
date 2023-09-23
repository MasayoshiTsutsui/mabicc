#include <stdio.h>
#include <string>
#include <iostream>
#include "mabicc.hpp"
#define DEBUG 0

void dbgprint(char *fmt, ...) {
    if (DEBUG)
        std::cout << fmt << std::endl;
}

void tkprint(Token *token) {
    Token *cur = token;
    while (cur) {
        std::cout << std::string(cur->str, cur->len) << std::endl;
        cur = cur->next;
    }
}

std::string getNodeName(NodeKind nk) {
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
        std::cout << "  ";
    }
    std::cout << getNodeName(node->kind) << " ";
    if (node->kind == ND_NUM)
        std::cout << node->val;
    std::cout << std::endl;

    if (!node->ary0)
        return;
    dfs_print(node->ary0, depth + 1);

    if (!node->ary1)
        return;
    dfs_print(node->ary1, depth + 1);

    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cout << std::endl;
}

void ndprint() {
    std::cout << std::endl << "NODE TREE::" << std::endl << std::endl;
    for (int i = 0; code[i]; i++) {
        dfs_print(code[i], 0);
    }
}