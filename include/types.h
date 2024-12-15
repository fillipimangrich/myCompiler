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
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_PARAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_FOR,
    NODE_RETURN,
    NODE_CALL,
    NODE_EXPRESSION,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_IDENTIFIER,
    NODE_LITERAL,
    NODE_BLOCK
} NodeType;

typedef struct ASTNode {
    NodeType type;
    Token token;
    struct ASTNode* next;
    union NodeData {
        // For binary operations (arithmetic, logical, comparison)
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            TokenType op;
        } binary;
        
        // For variable declarations
        struct {
            TokenType var_type;     // INT, FLOAT, STRING
            struct ASTNode *id;     // identifier
            struct ASTNode *dims;   // array dimensions, if any
        } var_decl;
        
        // For function declarations
        struct {
            TokenType return_type;
            struct ASTNode *id;     // function name
            struct ASTNode *params; // parameter list
            struct ASTNode *body;   // function body
        } function;
        
        // For if statements
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;
        
        // For for loops
        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *increment;
            struct ASTNode *body;
        } for_stmt;
        
        // For return statements
        struct {
            struct ASTNode *expr;
        } return_stmt;
        
        // For function calls
        struct {
            struct ASTNode *function;
            struct ASTNode *args;
        } call;
        
        // For blocks of statements
        struct {
            struct ASTNode **statements;
            int count;
        } block;
        
        // For literals (numbers, strings)
        struct {
            TokenType literal_type;
            union {
                int int_value;
                float float_value;
                char *string_value;
            } literal;
        } literal;
    } data;

} ASTNode;

#endif // TYPES_H