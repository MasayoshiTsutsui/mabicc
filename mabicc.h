
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
    int len;        // token length
};

// node kinds of ast
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // < 
    ND_LE,  // <=
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind; // type of node
    Node *lhs;     // left subtree
    Node *rhs;     // right subtree
    int val;       // used when kind is ND_NUM
};


Token *tokenize();
Node *expr();
void gen(Node *node);
void error(char *fmt, ...);