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
Scope* create_scope(Scope* parent) {
    Scope* scope = (Scope*)malloc(sizeof(Scope));
    scope->symbols = create_table();
    scope->parent = parent;
    return scope;
}

// Enter a new scope
void enter_scope() {
    semantic_ctx.current_scope = create_scope(semantic_ctx.current_scope);
}

// Exit current scope
void exit_scope() {
    if (semantic_ctx.current_scope->parent) {
        Scope* parent = semantic_ctx.current_scope->parent;
        free(semantic_ctx.current_scope->symbols);
        free(semantic_ctx.current_scope);
        semantic_ctx.current_scope = parent;
    }
}

// Look up a symbol in the current scope chain
Symbol* lookup_symbol_scoped(Token* token) {
    Scope* scope = semantic_ctx.current_scope;
    while (scope) {
        Symbol* symbol = lookup_symbol(scope->symbols, token);
        if (symbol) return symbol;
        scope = scope->parent;
    }
    return NULL;
}

// Semantic analysis functions
void analyze_node(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_FUNCTION:
            // Check if function is already defined
            if (lookup_symbol_scoped(&node->data.function.id->token)) {
                printf("Error: Function %s already defined\n", 
                       node->data.function.id->token.lexeme);
                exit(1);
            }
            
            // Add function to symbol table
            insert_symbol(semantic_ctx.current_scope->symbols, 
                         &node->data.function.id->token);
            
            // Create new scope for function body
            enter_scope();
            
            // Analyze parameters
            analyze_node(node->data.function.params);
            
            // Analyze function body
            analyze_node(node->data.function.body);
            
            exit_scope();
            break;
            
        case NODE_VAR_DECL:
            // Check if variable is already defined in current scope
            if (lookup_symbol(semantic_ctx.current_scope->symbols, 
                            &node->data.var_decl.id->token)) {
                printf("Error: Variable %s already defined in current scope\n",
                       node->data.var_decl.id->token.lexeme);
                exit(1);
            }
            
            // Add variable to symbol table
            insert_symbol(semantic_ctx.current_scope->symbols, 
                         &node->data.var_decl.id->token);
            break;
            
        case NODE_IDENTIFIER:
            // Check if variable is defined
            if (!lookup_symbol_scoped(&node->token)) {
                printf("Error: Undefined variable %s\n", node->token.lexeme);
                exit(1);
            }
            break;
        case NODE_BINARY_OP:
            analyze_node(node->data.binary.left);
            analyze_node(node->data.binary.right);
            break;
            
        case NODE_UNARY_OP:
            analyze_node(node->data.binary.left);
            break;
            
        case NODE_IF:
            analyze_node(node->data.if_stmt.condition);
            analyze_node(node->data.if_stmt.then_branch);
            if (node->data.if_stmt.else_branch) {
                analyze_node(node->data.if_stmt.else_branch);
            }
            break;
            
        case NODE_FOR:
            enter_scope();
            analyze_node(node->data.for_stmt.init);
            analyze_node(node->data.for_stmt.condition);
            analyze_node(node->data.for_stmt.increment);
            analyze_node(node->data.for_stmt.body);
            exit_scope();
            break;
            
        case NODE_RETURN:
            analyze_node(node->data.return_stmt.expr);
            break;
            
        case NODE_CALL:
            analyze_node(node->data.call.function);
            analyze_node(node->data.call.args);
            break;
            
        case NODE_BLOCK:
            enter_scope();
            for (int i = 0; i < node->data.block.count; i++) {
                analyze_node(node->data.block.statements[i]);
            }
            exit_scope();
            break;
            
        case NODE_ASSIGN:
            analyze_node(node->data.binary.left);
            analyze_node(node->data.binary.right);
            break;
            
        case NODE_LITERAL:
            // Nothing to analyze for literals
            break;
            
        case NODE_PROGRAM:
            for (int i = 0; i < node->data.block.count; i++) {
                analyze_node(node->data.block.statements[i]);
            }
            break;
    }
}

#endif // SEMANTIC_H
