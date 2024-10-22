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

unsigned int hash(const char *name) {
    unsigned int hash_value = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        hash_value = 31 * hash_value + name[i];
    }
    return hash_value % TABLE_SIZE;
}

SymbolTable* create_table() {
    SymbolTable *sym_table = (SymbolTable *)malloc(sizeof(SymbolTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        sym_table->table[i] = NULL;
    }
    return sym_table;
}

void insert_symbol(SymbolTable *sym_table, Token *token) {
    unsigned int index = hash(token->lexeme);
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->token = token;
    new_symbol->next = sym_table->table[index];
    sym_table->table[index] = new_symbol;
}

Symbol* lookup_symbol(SymbolTable *sym_table, Token *token) {
    unsigned int index = hash(token->lexeme);
    Symbol *current = sym_table->table[index];
    while (current != NULL && strcmp(current->token->lexeme, token->lexeme) != 0) {
        current = current->next;
    }
    return current;
}

void print_table(SymbolTable *sym_table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbol *current = sym_table->table[i];
        if (current != NULL) {
            printf("Index %d: ", i);
            while (current != NULL) {
                printf("%s (%s), ", current->token->lexeme, current->token->lexeme);
                current = current->next;
            }
            printf("\n");
        }
    }
}

#endif // TABLE_H