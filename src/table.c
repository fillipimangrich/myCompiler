#include "table.h"

unsigned int hash(const char* name) {
    unsigned int hash_value = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        hash_value = 31 * hash_value + name[i];
    }
    return hash_value % TABLE_SIZE;
}

SymbolTable* create_table() {
    SymbolTable* sym_table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!sym_table) return NULL;
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        sym_table->table[i] = NULL;
    }
    return sym_table;
}

void insert_symbol_with_type(SymbolTable* sym_table, Token* token, TypeInfo type_info) {
    unsigned int index = hash(token->lexeme);
    
    Symbol* existing = lookup_symbol(sym_table, token);
    if (existing) {
        semantic_error("Symbol already declared", token);
        return;
    }
    
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!new_symbol) return;
    
    new_symbol->token = token;
    new_symbol->type_info = type_info;
    new_symbol->next = sym_table->table[index];
    sym_table->table[index] = new_symbol;
}

Symbol* lookup_symbol(SymbolTable* sym_table, Token* token) {
    if (!sym_table || !token) return NULL;
    
    unsigned int index = hash(token->lexeme);
    Symbol* current = sym_table->table[index];
    
    while (current != NULL) {
        if (strcmp(current->token->lexeme, token->lexeme) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void print_separator(int max_name_width, int max_type_width, int max_func_width){
    printf("+");
    for (int i = 0; i < max_name_width; i++) printf("-");
    printf("+");
    for (int i = 0; i < max_type_width; i++) printf("-");
    printf("+");
    for (int i = 0; i < max_func_width; i++) printf("-");
    printf("+\n");
}

void print_table(SymbolTable* sym_table) {
    if (!sym_table) {
        printf("Symbol Table is NULL\n");
        return;
    }

    // First pass: determine maximum widths
    int max_name_width = 11;  // minimum width for "Symbol Name"
    int max_type_width = 4;   // minimum width for "Type"
    int max_func_width = 10;  // minimum width for "Is Function"

    // Scan for maximum widths
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbol* current = sym_table->table[i];
        while (current != NULL) {
            if (current->token) {
                int name_len = strlen(current->token->lexeme);
                int type_len = strlen(TokenString[current->type_info.type]);
                
                max_name_width = (name_len > max_name_width) ? name_len : max_name_width;
                max_type_width = (type_len > max_type_width) ? type_len : max_type_width;
            }
            current = current->next;
        }
    }

    // Add padding
    max_name_width += 2;
    max_type_width += 2;
    max_func_width += 2;



    // Print table header
    print_separator(max_name_width, max_type_width, max_func_width);
    printf("| %-*s | %-*s | %-*s |\n",
           max_name_width-2, "Symbol Name",
           max_type_width-2, "Type",
           max_func_width-2, "Is Function");
    print_separator(max_name_width, max_type_width, max_func_width);

    // Print table contents
    bool is_empty = true;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbol* current = sym_table->table[i];
        while (current != NULL) {
            is_empty = false;
            if (current->token) {
                printf("| %-*s | %-*s | %-*s |\n",
                       max_name_width-2, current->token->lexeme,
                       max_type_width-2, TokenString[current->type_info.type],
                       max_func_width-2, current->type_info.is_function ? "Yes" : "No");
            }
            current = current->next;
        }
    }

    if (is_empty) {
        int total_width = max_name_width + max_type_width + max_func_width + 4;
        printf("| %-*s |\n", total_width-2, "Table is empty");
    }

    // Print table footer
    print_separator(max_name_width, max_type_width, max_func_width);
}