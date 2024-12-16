#include "semantic.h"

SemanticContext semantic_ctx;

void init_semantic() {
    printf("[DEBUG] Initializing semantic context\n");
    semantic_ctx.current_scope = create_scope(NULL);
    printf("[DEBUG] Created initial scope\n");
}

Scope* create_scope(Scope* parent) {
    printf("[DEBUG] Creating new scope with parent: %p\n", (void*)parent);
    Scope* scope = (Scope*)malloc(sizeof(Scope));
    scope->symbols = create_table();
    scope->parent = parent;
    printf("[DEBUG] Created scope: %p\n", (void*)scope);
    return scope;
}

void enter_scope() {
    printf("[DEBUG] Entering new scope\n");
    Scope* new_scope = create_scope(semantic_ctx.current_scope);
    semantic_ctx.current_scope = new_scope;
    printf("[DEBUG] Current scope is now: %p\n", (void*)semantic_ctx.current_scope);
}

void exit_scope() {
    if (semantic_ctx.current_scope->parent) {
        printf("[DEBUG] Exiting scope: %p\n", (void*)semantic_ctx.current_scope);
        Scope* old_scope = semantic_ctx.current_scope;
        printf("[DEBUG] Symbol table for exiting scope:\n");
        print_table(old_scope->symbols);
        semantic_ctx.current_scope = old_scope->parent;
        // Free the old scope (you might want to add proper cleanup)
        free(old_scope->symbols);
        free(old_scope);
        printf("[DEBUG] Returned to scope: %p\n", (void*)semantic_ctx.current_scope);
        printf("[DEBUG] Symbol table for current scope:\n"); 
        print_table(semantic_ctx.current_scope->symbols);
    }
}

Symbol* lookup_symbol_scoped(Token* token) {
    printf("[DEBUG] Looking up symbol: %s\n", token->lexeme);
    Scope* current = semantic_ctx.current_scope;
    while (current) {
        Symbol* symbol = lookup_symbol(current->symbols, token);
        if (symbol) {
            printf("[DEBUG] Found symbol in scope: %p\n", (void*)current);
            return symbol;
        }
        printf("[DEBUG] Symbol not found in scope %p, checking parent\n", (void*)current);
        current = current->parent;
    }
    printf("[DEBUG] Symbol not found in any scope\n");
    return NULL;
}

TokenType get_expression_type(ASTNode* node) {
    if (!node) return UNKNOWN;
    
    switch (node->type) {
        case NODE_LITERAL:
            return node->data.literal.literal_type;
            
        case NODE_IDENTIFIER: {
            Symbol* sym = lookup_symbol_scoped(&node->token);
            if (sym) {
                return sym->type_info.type;
            }
            return UNKNOWN;
        }
        
        case NODE_BINARY_OP: {
            TokenType left_type = get_expression_type(node->data.binary.left);
            TokenType right_type = get_expression_type(node->data.binary.right);
            return get_result_type(node->data.binary.op, left_type, right_type);
        }
        
        case NODE_CALL: {
            Symbol* func = lookup_symbol_scoped(&node->data.call.function->token);
            if (func && func->type_info.is_function) {
                return func->type_info.return_type;
            }
            return UNKNOWN;
        }
        
        default:
            return UNKNOWN;
    }
}

bool are_types_compatible(TokenType type1, TokenType type2) {
    if (type1 == UNKNOWN || type2 == UNKNOWN) {
        return false;
    }
    
    // Same types are always compatible
    if (type1 == type2) {
        return true;
    }
    
    // Integer and float are compatible
    if ((type1 == INT && type2 == FLOAT) || 
        (type1 == FLOAT && type2 == INT)) {
        return true;
    }
    
    // INT and INT_CONST are compatible
    if ((type1 == INT && type2 == INT_CONST) ||
        (type1 == INT_CONST && type2 == INT)) {
        return true;
    }
    
    // FLOAT and FLOAT_CONST are compatible
    if ((type1 == FLOAT && type2 == FLOAT_CONST) ||
        (type1 == FLOAT_CONST && type2 == FLOAT)) {
        return true;
    }
    
    // String and STRING_CONST are compatible
    if ((type1 == STRING && type2 == STRING_CONST) ||
        (type1 == STRING_CONST && type2 == STRING)) {
        return true;
    }
    
    // String operations are only compatible with other strings
    if (type1 == STRING || type2 == STRING ||
        type1 == STRING_CONST || type2 == STRING_CONST) {
        return (type1 == STRING || type1 == STRING_CONST) && 
               (type2 == STRING || type2 == STRING_CONST);
    }
    
    return false;
}

TokenType get_result_type(TokenType op, TokenType left_type, TokenType right_type) {
    // Handle comparison operators
    if (op == LT || op == GT || op == LEQ || op == GEQ || 
        op == EQ || op == NEQ || op == AND || op == OR) {
        
        // Check if operands are compatible for comparison
        if (!are_types_compatible(left_type, right_type)) {
            printf("[DEBUG] Type mismatch in comparison operation\n");
            return UNKNOWN;
        }
        
        return INT;  // Boolean operations return integer (0 or 1)
    }
    
    // If types aren't compatible, return UNKNOWN
    if (!are_types_compatible(left_type, right_type)) {
        printf("[DEBUG] Incompatible types in operation: %d and %d\n", left_type, right_type);
        return UNKNOWN;
    }
    
    // String concatenation
    if (left_type == STRING && right_type == STRING && op == PLUS) {
        return STRING;
    }
    
    // Arithmetic operations
    if (op == PLUS || op == MINUS || op == MULT || op == DIV || op == MOD) {
        // String type is not allowed in arithmetic operations except PLUS
        if ((left_type == STRING || right_type == STRING) && op != PLUS) {
            printf("[DEBUG] Invalid arithmetic operation with string type\n");
            return UNKNOWN;
        }
        
        // If either operand is float, result is float
        if (left_type == FLOAT || right_type == FLOAT) {
            return FLOAT;
        }
        // Integer operations
        if (left_type == INT && right_type == INT) {
            return INT;
        }
    }
    
    printf("[DEBUG] Unknown operation result type\n");
    return UNKNOWN;
}

void analyze_node(ASTNode* node) {
    if (!node) return;
        
    switch (node->type) {
        case NODE_VAR_DECL: {
            printf("[DEBUG] Analyzing variable declaration\n");
            TypeInfo type_info = {
                .type = node->data.var_decl.var_type,
                .is_function = 0,
                .return_type = UNKNOWN
            };
            insert_symbol_with_type(semantic_ctx.current_scope->symbols, 
                                  &node->data.var_decl.id->token, 
                                  type_info);
            printf("[DEBUG] Inserted variable symbol: %s\n", node->data.var_decl.id->token.lexeme);
            break;
        }
        
        case NODE_ASSIGN: {
            printf("[DEBUG] Analyzing assignment\n");
            analyze_node(node->data.binary.left);
            analyze_node(node->data.binary.right);
            
            Symbol* left_sym = lookup_symbol_scoped(&node->data.binary.left->token);
            if (!left_sym) {
                printf("[DEBUG] Assignment error: undeclared variable %s\n", node->data.binary.left->token.lexeme);
                semantic_error("Assignment to undeclared variable", 
                             &node->data.binary.left->token);
            } else {
                printf("[DEBUG] Assignment to valid variable: %s\n", node->data.binary.left->token.lexeme);
                
                // Get types of both sides
                TokenType left_type = left_sym->type_info.type;
                TokenType right_type = get_expression_type(node->data.binary.right);
                
                printf("[DEBUG] Left type: %s (%d)\n", TokenString[left_type], left_type);
                printf("[DEBUG] Right type: %s (%d)\n", TokenString[right_type], right_type);
                printf("[DEBUG] Checking type compatibility...\n");
                
                // Check if types are compatible
                if (!are_types_compatible(left_type, right_type)) {
                    printf("[DEBUG] Type mismatch in assignment: %s (%s) = (%s)\n", 
                           node->data.binary.left->token.lexeme, 
                           TokenString[left_type],
                           TokenString[right_type]);
                    semantic_error("Type mismatch in assignment",
                                 &node->data.binary.left->token);
                } else {
                    printf("[DEBUG] Assignment types match: %s = %s\n", 
                           TokenString[left_type], 
                           TokenString[right_type]);
                }
            }
            break;
        }
        
        case NODE_IDENTIFIER: {
            printf("[DEBUG] Analyzing identifier: %s\n", node->token.lexeme);
            Symbol* symbol = lookup_symbol_scoped(&node->token);
            if (!symbol) {
                printf("[DEBUG] Identifier error: undeclared identifier %s\n", node->token.lexeme);
                semantic_error("Use of undeclared identifier", &node->token);
            } else {
                printf("[DEBUG] Found valid identifier: %s\n", node->token.lexeme);
            }
            break;
        }

        case NODE_FUNCTION: {
            printf("[DEBUG] Analyzing function declaration\n");
            TypeInfo type_info = {
                .type = node->data.function.return_type,
                .is_function = 1,
                .return_type = node->data.function.return_type
            };
            insert_symbol_with_type(semantic_ctx.current_scope->symbols,
                                  &node->data.function.id->token,
                                  type_info);
            
            enter_scope();
            
            if (node->data.function.params) {
                analyze_node(node->data.function.params);
            }
            
            if (node->data.function.body) {
                analyze_node(node->data.function.body);
            }
            
            exit_scope();
            break;
        }

        case NODE_PARAM: {
            printf("[DEBUG] Analyzing function parameter\n");
            TypeInfo type_info = {
                .type = node->data.var_decl.var_type,
                .is_function = 0,
                .return_type = UNKNOWN
            };
            insert_symbol_with_type(semantic_ctx.current_scope->symbols,
                                  &node->data.var_decl.id->token,
                                  type_info);
            
            // Analyze next parameter if exists
            if (node->next) {
                analyze_node(node->next);
            }
            break;
        }
        
        case NODE_BINARY_OP:
            printf("[DEBUG] Analyzing binary operation\n");
            analyze_node(node->data.binary.left);
            analyze_node(node->data.binary.right);
            break;
            
        case NODE_UNARY_OP:
            printf("[DEBUG] Analyzing unary operation\n");
            analyze_node(node->data.binary.left);
            break;
            
        case NODE_IF:
            printf("[DEBUG] Analyzing if statement\n");
            analyze_node(node->data.if_stmt.condition);
            analyze_node(node->data.if_stmt.then_branch);
            if (node->data.if_stmt.else_branch) {
                printf("[DEBUG] Analyzing else branch\n");
                analyze_node(node->data.if_stmt.else_branch);
            }
            break;
            
        case NODE_FOR:
            printf("[DEBUG] Analyzing for loop\n");
            enter_scope();
            analyze_node(node->data.for_stmt.init);
            analyze_node(node->data.for_stmt.condition);
            analyze_node(node->data.for_stmt.increment);
            analyze_node(node->data.for_stmt.body);
            exit_scope();
            break;
            
        case NODE_RETURN:
            printf("[DEBUG] Analyzing return statement\n");
            analyze_node(node->data.return_stmt.expr);
            break;
            
        case NODE_CALL:
            printf("[DEBUG] Analyzing function call\n");
            analyze_node(node->data.call.function);
            analyze_node(node->data.call.args);
            break;
            
        case NODE_BLOCK:
            printf("[DEBUG] Analyzing block\n");
            enter_scope();
            for (int i = 0; i < node->data.block.count; i++) {
                analyze_node(node->data.block.statements[i]);
            }
            exit_scope();
            break;
            
        case NODE_LITERAL:
            printf("[DEBUG] Analyzing literal\n");
            // Nothing to analyze for literals
            break;
            
        case NODE_PROGRAM:
            printf("[DEBUG] Analyzing program\n");
            for (int i = 0; i < node->data.block.count; i++) {
                analyze_node(node->data.block.statements[i]);
            }
            break;

        case NODE_EXPRESSION:
            printf("[DEBUG] Analyzing expression\n");
            if (node->data.binary.left) {
                analyze_node(node->data.binary.left);
            }
            if (node->data.binary.right) {
                analyze_node(node->data.binary.right);
            }
            
            // Validate types for binary operations
            if (node->data.binary.op == PLUS || node->data.binary.op == MINUS 
            || node->data.binary.op == MULT || node->data.binary.op == DIV
            || node->data.binary.op == MOD || node->data.binary.op == LT
            || node->data.binary.op == GT || node->data.binary.op == LEQ
            || node->data.binary.op == GEQ || node->data.binary.op == EQ
            || node->data.binary.op == NEQ) {
                int left_type = get_expression_type(node->data.binary.left);
                int right_type = get_expression_type(node->data.binary.right);
                
                if (!are_types_compatible(left_type, right_type)) {
                    semantic_error("Type mismatch in expression", &node->token);
                }
                
                // Set result type based on operation and operand types
                node->type = get_result_type(node->data.binary.op, left_type, right_type);
            }
            break;
    }
}
