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
    case ND_FOR:
        jmplabel = labelIdx++;
        if (node->ary0)
            gen(node->ary0);
        printf(".Lbegin%d:\n", jmplabel);
        if (node->ary1)
            gen(node->ary1); // eval condition
        else
            printf("  push 1\n");
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", jmplabel);
        if (node->ary3)
            gen(node->ary3); // execute inside loop
        if (node->ary2)
            gen(node->ary2); // execute *** -> for(;;***)
        printf("  jmp .Lbegin%d\n", jmplabel);
        printf(".Lend%d:\n", jmplabel);
        return;
    case ND_WHILE:
        jmplabel = labelIdx++;
        printf(".Lbegin%d:\n", jmplabel);
        gen(node->ary0);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", jmplabel);
        gen(node->ary1);
        printf("  jmp .Lbegin%d\n", jmplabel);
        printf(".Lend%d:\n", jmplabel);
        return;
    case ND_IF:
        jmplabel = labelIdx++;
        gen(node->ary0);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (!node->ary2) { // when else is not used
            printf("  je  .Lend%d\n", jmplabel);
            gen(node->ary1);
            printf(".Lend%d:\n", jmplabel);
        } else {
            printf("  je  .Lelse%d\n", jmplabel);
            gen(node->ary1);
            printf("  jmp .Lend%d\n", jmplabel);
            printf(".Lelse%d:\n", jmplabel);
            gen(node->ary2);
            printf(".Lend%d:\n", jmplabel);
        }
        return;
    case ND_RETURN:
        gen(node->ary0);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_BLOCK:
        while (node->next) {
            gen(node->next);
            printf("  pop rax\n");
            node = node->next;
        }
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
        gen_lval(node->ary0);
        gen(node->ary1);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    gen(node->ary0);
    gen(node->ary1);

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
