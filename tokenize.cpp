#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <algorithm>
#include "mabicc.hpp"


// make new token and chain it to 'cur'.
Token *new_token(TokenKind kind, Token *cur, std::string::const_iterator str, int len) {
    Token *tok = (Token*)calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = (char*)&(*str);
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(std::string::const_iterator itr, std::string q) {
    return std::equal(q.begin(), q.end(), itr);
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') || (c == '_');
}

int isIn(char c, std::string s) {
    return std::find(s.begin(), s.end(), c) != s.end();
}

int str2int(std::string::const_iterator &s) {
    int val = 0;
    while (isdigit(*s)) {
        val *= 10;
        val += *s - '0';
        s++;
    }
    return val;
}


// tokenize input char 'p' and return it.
Token *tokenize(const std::string &user_input) {
    std::string::const_iterator input_itr = user_input.begin();
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*input_itr) {
        // skip space
        if (startswith(input_itr, " ")) {
            input_itr++;
            continue;
        }

        if (startswith(input_itr, "==") || startswith(input_itr, "!=") ||
            startswith(input_itr, "<=") || startswith(input_itr, ">=")) {
            cur = new_token(TK_RESERVED, cur, input_itr, 2);
            input_itr += 2;
            continue;
        }

        if (startswith(input_itr, "for") && !is_alnum(*(input_itr + 3))) { // 3 is length of "for"
            cur = new_token(TK_FOR, cur, input_itr, 3);
            input_itr += 3;
            continue;
        }

        if (startswith(input_itr, "while") && !is_alnum(*(input_itr + 5))) { // 3 is length of "for"
            cur = new_token(TK_WHILE, cur, input_itr, 5);
            input_itr += 5;
            continue;
        }

        if (startswith(input_itr, "if") && !is_alnum(*(input_itr + 2))) { // 2 is length of "if"
            cur = new_token(TK_IF, cur, input_itr, 2);
            input_itr += 2;
            continue;
        }

        if (startswith(input_itr, "else") && !is_alnum(*(input_itr + 4))) { // 4 is length of "else"
            cur = new_token(TK_ELSE, cur, input_itr, 4);
            input_itr += 4;
            continue;
        }

        if (startswith(input_itr, "return") && !is_alnum(*(input_itr + 6))) { // 6 is length of "return"
            cur = new_token(TK_RETURN, cur, input_itr, 6);
            input_itr += 6;
            continue;
        }

        if ('a' <= *input_itr && *input_itr <= 'z') {
            std::string::const_iterator begin_itr = input_itr;
            while(isalnum(*input_itr)) input_itr++;
            int strlen = std::distance(begin_itr, input_itr);
            cur = new_token(TK_IDENT, cur, begin_itr, strlen);
            continue;
        }

        if (isIn(*input_itr, "+-*/()<>=;{},")) {
            cur = new_token(TK_RESERVED, cur, input_itr++, 1);
            continue;
        }

        if (isdigit(*input_itr)) {
            cur = new_token(TK_NUM, cur, input_itr, 0);
            const std::string::const_iterator begin_itr = input_itr;
            cur->val = str2int(input_itr);
            cur->len = std::distance(begin_itr, input_itr);
            continue;
        }

        //error_at(input_itr, "Unable to tokenize.");
    }

    new_token(TK_EOF, cur, input_itr, 0);
    return head.next;
}