#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define TABLE_SIZE 100

typedef struct Symbol {
    Token * token;
    struct Symbol *next;
} Symbol;

typedef struct {
    Symbol *table[TABLE_SIZE];
} SymbolTable;

unsigned int hash(const char *name);

SymbolTable* create_table();

void insert_symbol(SymbolTable *sym_table, Token *token);

Symbol* lookup_symbol(SymbolTable *sym_table, Token *token);

void print_table(SymbolTable *sym_table);

#endif // TABLE_H