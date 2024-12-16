#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "types.h"
#include "ast.h"
#include "table.h"
#include "error.h"

typedef struct Scope {
    SymbolTable* symbols;
    struct Scope* parent;
} Scope;

typedef struct {
    Scope* current_scope;
} SemanticContext;

extern SemanticContext semantic_ctx;

void init_semantic();

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

// Type checking functions
TokenType get_expression_type(ASTNode* node);
bool are_types_compatible(TokenType type1, TokenType type2);
TokenType get_result_type(TokenType op, TokenType left_type, TokenType right_type);

#endif // SEMANTIC_H
