#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "types.h"
#include "ast.h"
#include "table.h"
// semantic.h
typedef struct Scope {
    SymbolTable* symbols;
    struct Scope* parent;
} Scope;

typedef struct {
    Scope* current_scope;
} SemanticContext;

extern SemanticContext semantic_ctx;

// Initialize a new scope
Scope* create_scope(Scope* parent);

// Enter a new scope
void enter_scope();

// Exit current scope
void exit_scope();

// Look up a symbol in the current scope chain
Symbol* lookup_symbol_scoped(Token* token);

// Semantic analysis functions
void analyze_node(ASTNode* node);

#endif // SEMANTIC_H
