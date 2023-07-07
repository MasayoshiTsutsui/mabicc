#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "mabicc.h"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("left val is not a varibale in assignment");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

int labelIdx = 0;

void gen(Node *node) {
    int jmplabel;
    switch (node->kind) {
    case ND_WHILE:
        jmplabel = labelIdx++;
        printf(".Lbegin%d:\n", jmplabel);
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", jmplabel);
        gen(node->rhs);
        printf("  jmp .Lbegin%d\n", jmplabel);
        printf(".Lend%d:\n", jmplabel);
        return;
    case ND_IF:
        jmplabel = labelIdx++;
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (!node->els) { // when else is not used
            printf("  je  .Lend%d\n", jmplabel);
            gen(node->rhs);
            printf(".Lend%d:\n", jmplabel);
        } else {
            printf("  je  .Lelse%d\n", jmplabel);
            gen(node->rhs);
            printf("  jmp .Lend%d\n", jmplabel);
            printf(".Lelse%d:\n", jmplabel);
            gen(node->els);
            printf(".Lend%d:\n", jmplabel);
        }
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NE:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
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
