#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include "mabicc.hpp"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("left val is not a varibale in assignment");

    std::cout << "  mov rax, rbp" << std::endl;
    std::cout << "  sub rax, " << node->offset << std::endl;
    std::cout << "  push rax" << std::endl;
}

int labelIdx = 0;

void gen(Node *node) {
    int jmplabel;
    switch (node->kind) {
    case ND_FOR:
        jmplabel = labelIdx++;
        if (node->ary0)
            gen(node->ary0);
        std::cout << ".Lbegin" << jmplabel << ":" << std::endl;
        if (node->ary1)
            gen(node->ary1); // eval condition
        else
            std::cout << "  push 1" << std::endl;
        std::cout << "  pop rax" << std::endl;
        std::cout << "  cmp rax, 0" << std::endl;
        std::cout << "  je  .Lend" << jmplabel << std::endl;
        if (node->ary3)
            gen(node->ary3); // execute inside loop
        if (node->ary2)
            gen(node->ary2); // execute *** -> for(;;***)
        std::cout << "  jmp .Lbegin" << jmplabel << std::endl;
        std::cout << ".Lend" << jmplabel << ":" << std::endl;
        return;
    case ND_WHILE:
        jmplabel = labelIdx++;
        std::cout << ".Lbegin" << jmplabel << ":" << std::endl;
        gen(node->ary0);
        std::cout << "  pop rax" << std::endl;
        std::cout << "  cmp rax, 0" << std::endl;
        std::cout << "  je  .Lend" << jmplabel << std::endl;
        gen(node->ary1);
        std::cout << "  jmp .Lbegin" << jmplabel << std::endl;
        std::cout << ".Lend" << jmplabel << ":" << std::endl;
        return;
    case ND_IF:
        jmplabel = labelIdx++;
        gen(node->ary0);
        std::cout << "  pop rax" << std::endl;
        std::cout << "  cmp rax, 0" << std::endl;
        if (!node->ary2) { // when else is not used
            std::cout << "  je  .Lend" << jmplabel << std::endl;
            gen(node->ary1);
            std::cout << ".Lend" << jmplabel << ":" << std::endl;
        } else {
            std::cout << "  je  .Lelse" << jmplabel << std::endl;
            gen(node->ary1);
            std::cout << "  jmp .Lend" << jmplabel << std::endl;
            std::cout << ".Lelse" << jmplabel << ":" << std::endl;
            gen(node->ary2);
            std::cout << ".Lend" << jmplabel << ":" << std::endl;
        }
        return;
    case ND_RETURN:
        gen(node->ary0);
        std::cout << "  pop rax" << std::endl;
        std::cout << "  mov rsp, rbp" << std::endl;
        std::cout << "  pop rbp" << std::endl;
        std::cout << "  ret" << std::endl;
        return;
    case ND_BLOCK:
        while (node->next) {
            gen(node->next);
            std::cout << "  pop rax" << std::endl;
            node = node->next;
        }
        return;
    case ND_NUM:
        std::cout << "  push " << node->val << std::endl;
        return;
    case ND_LVAR:
        gen_lval(node);
        std::cout << "  pop rax" << std::endl;
        std::cout << "  mov rax, [rax]" << std::endl;
        std::cout << "  push rax" << std::endl;
        return;
    case ND_ASSIGN:
        gen_lval(node->ary0);
        gen(node->ary1);
        std::cout << "  pop rdi" << std::endl;
        std::cout << "  pop rax" << std::endl;
        std::cout << "  mov [rax], rdi" << std::endl;
        std::cout << "  push rdi" << std::endl;
        return;
    }

    gen(node->ary0);
    gen(node->ary1);

    std::cout << "  pop rdi" << std::endl;
    std::cout << "  pop rax" << std::endl;

    switch (node->kind) {
        case ND_EQ:
            std::cout << "  cmp rax, rdi" << std::endl;
            std::cout << "  sete al" << std::endl;
            std::cout << "  movzb rax, al" << std::endl;
            break;
        case ND_NE:
            std::cout << "  cmp rax, rdi" << std::endl;
            std::cout << "  setne al" << std::endl;
            std::cout << "  movzb rax, al" << std::endl;
            break;
        case ND_LT:
            std::cout << "  cmp rax, rdi" << std::endl;
            std::cout << "  setl al" << std::endl;
            std::cout << "  movzb rax, al" << std::endl;
            break;
        case ND_LE:
            std::cout << "  cmp rax, rdi" << std::endl;
            std::cout << "  setle al" << std::endl;
            std::cout << "  movzb rax, al" << std::endl;
            break;
        case ND_ADD:
            std::cout << "  add rax, rdi" << std::endl;
            break;
        case ND_SUB:
            std::cout << "  sub rax, rdi" << std::endl;
            break;
        case ND_MUL:
            std::cout << "  imul rax, rdi" << std::endl;
            break;
        case ND_DIV:
            std::cout << "  cqo" << std::endl;
            std::cout << "  idiv rdi" << std::endl;
            break;
    }
    std::cout << "  push rax" << std::endl;
}
