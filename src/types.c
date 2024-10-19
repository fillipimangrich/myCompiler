#include "types.h"

const char* TokenString[] = {
    "DEF",         // def
    "RETURN",      // return
    "IF",          // if
    "ELSE",        // else
    "FOR",         // for
    "BREAK",       // break

    "IDENT",       // identificador comum
    "FUNC_IDENT",  // identificador de função

    "INT",         // int
    "FLOAT",       // float
    "STRING",      // string
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

    "NOT",         // not
    "AND",         // and
    "OR",          // or   

    "LBRACE",      // {
    "RBRACE",      // }
    "LPAREN",      // (
    "RPAREN",      // )
    "LBRACK",      // [
    "RBRACK",      // ]

    "SEMICOLON",   // ;
    "COMMA",       // ,

    "NULL_TOK",    // null
    "END_OF_FILE", // fim do arquivo
    "UNKNOWN"      // desconhecido
};