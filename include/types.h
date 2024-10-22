#ifndef TYPES_H
#define TYPES_H

typedef enum {
    DEF, RETURN, IF, ELSE, FOR, BREAK,
    IDENT, FUNC_IDENT,
    INT, FLOAT, STRING, INT_CONST, FLOAT_CONST, STRING_CONST,
    ASSIGN,
    PLUS, MINUS, MULT, DIV, MOD, LT, GT, LEQ, GEQ, EQ, NEQ, 
    NOT, AND, OR,
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACK, RBRACK,
    SEMICOLON, COMMA,
    NULL_TOK, 
    END_OF_FILE, 
    UNKNOWN
} TokenType;

extern const char* TokenString[];

typedef struct {
    TokenType type;
    char lexeme[256];
    int line;
    int column;
} Token;

typedef enum {
    NODE_FUNCTION,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_FOR,
    NODE_RETURN,
    NODE_EXPRESSION,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    Token token;

    union types
    {
        struct{
            struct ASTNode *left;
            struct ASTNode *right;
        } expression;

        struct{
            struct ASTNode *ident;
            struct ASTNode *value;
        } assign;

        struct{
            struct ASTNode *type;
        } vardecl;

    };
    
} ASTNode;


#endif // TYPES_H