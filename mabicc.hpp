
// tokens
typedef enum {
    TK_RESERVED, // non-number
    TK_IDENT,    // identifier
    TK_NUM,      // integers
    TK_EOF,      // end of file
    TK_RETURN,   // return
    TK_IF,       // if
    TK_ELSE,     // else
    TK_WHILE,    // while
    TK_FOR,      // for
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
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // < 
    ND_LE,     // <=
    ND_ASSIGN, // =
    ND_LVAR,   // local variable
    ND_NUM,    // integer
    ND_RETURN, // return
    ND_IF,     // if
    ND_WHILE,  // while
    ND_FOR,    // for
    ND_BLOCK,  // block {}
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind; // type of node
    Node *ary0;     // left subtree
    Node *ary1;     // right subtree
    Node *ary2;     // may used when kind is ND_IF, ND_WHILE, ND_FOR
    Node *ary3;     // may used when kind is ND_FOR
    int val;       // used when kind is ND_NUM
    int offset;    // used when kind is ND_LVAR
    Node *next;    // used when kind is ND_BLOCK
};

typedef struct LVar LVar;

// local variable type
struct LVar {
    LVar *next; // next lvar or NULL
    char *name; // lvar name
    int len;
    int offset;
};

Token *tokenize(const std::string &user_input);
void program(Token* &token);
extern Node *code[];
void gen(Node *node);
void error(const char *fmt, ...);
void error_at(char *loc, const char *fmt, ...);