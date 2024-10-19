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
    char lexeme[256];  // máximo de 256 caracteres por token
} Token;


#endif // TYPES_H

