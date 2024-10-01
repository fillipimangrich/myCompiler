#ifndef TYPES_H
#define TYPES_H

typedef enum {
    DEF, RETURN, IF, ELSE, FOR, BREAK, PRINT, READ, NEW,
    INT, FLOAT, STRING, IDENT, INT_CONST, FLOAT_CONST, STRING_CONST,
    ASSIGN, PLUS, MINUS, MULT, DIV, MOD, LT, GT, LEQ, GEQ, EQ, NEQ,
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACK, RBRACK, SEMICOLON, COMMA,
    END_OF_FILE, UNKNOWN
} TokenType;

char* TokenString[] = {
    "DEF",         // def
    "RETURN",      // return
    "IF",          // if
    "ELSE",        // else
    "FOR",         // for
    "BREAK",       // break
    "PRINT",       // print
    "READ",        // read
    "NEW",         // new

    "INT",         // int
    "FLOAT",       // float
    "STRING",      // string
    "IDENT",       // identificador
    "INT_CONST",   // constante inteira
    "FLOAT_CONST", // constante float
    "STRING_CONST",// constante string

    "ASSIGN",      // =
    "PLUS",        // +
    "MINUS",       // -
    "MULT",        // *
    "DIV",         // /
    "MOD",         // %
    "LT",          // <
    "GT",          // >
    "LEQ",         // <=
    "GEQ",         // >=
    "EQ",          // ==
    "NEQ",         // !=

    "LBRACE",      // {
    "RBRACE",      // }
    "LPAREN",      // (
    "RPAREN",      // )
    "LBRACK",      // [
    "RBRACK",      // ]
    "SEMICOLON",   // ;
    "COMMA",       // ,

    "END_OF_FILE", // fim do arquivo
    "UNKNOWN"      // desconhecido
};

typedef struct {
    TokenType type;
    char lexeme[256];  // máximo de 256 caracteres por token
} Token;


typedef enum {
    E0,
    E1,
    E2,
    E3,
    E4
} State;

typedef void(*Handler)(char *, State *);

#endif // TYPES_H

