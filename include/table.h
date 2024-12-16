#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "error.h"

#define TABLE_SIZE 100

// Add type information structure
typedef struct {
    TokenType type;          // INT, FLOAT, STRING, etc.
    int is_function;         // Whether this is a function
    TokenType return_type;   // Return type if it's a function
} TypeInfo;

typedef struct Symbol {
    Token* token;
    TypeInfo type_info;      // Add type information
    struct Symbol* next;
} Symbol;

typedef struct {
    Symbol* table[TABLE_SIZE];
} SymbolTable;

unsigned int hash(const char* name);
SymbolTable* create_table();
void insert_symbol_with_type(SymbolTable* sym_table, Token* token, TypeInfo type_info);
Symbol* lookup_symbol(SymbolTable* sym_table, Token* token);
void print_table(SymbolTable* sym_table);

#endif // TABLE_H