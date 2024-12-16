#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tac.h"

TACContext tac_ctx;

void init_tac() {
    printf("[DEBUG] Initializing TAC context\n");
    tac_ctx.first = NULL;
    tac_ctx.last = NULL;
    tac_ctx.temp_count = 0;
    tac_ctx.label_count = 0;
    printf("[DEBUG] TAC context initialized\n");
}

char* new_temp() {
    char* temp = (char*)malloc(16);
    if (!temp) {
        printf("[DEBUG] Failed to allocate memory for temporary variable\n");
        return NULL;
    }
    
    int result = snprintf(temp, 16, "t%d", tac_ctx.temp_count++);
    if (result < 0 || result >= 16) {
        printf("[DEBUG] Error formatting temporary variable name\n");
        free(temp);
        return NULL;
    }
    
    printf("[DEBUG] Created new temporary variable: %s\n", temp);
    return temp;
}

char* new_label() {
    char* label = malloc(16);
    snprintf(label, 16, "L%d", tac_ctx.label_count++);
    return label;
}

TACOp* create_tac_op(TACOpType op, char* result, char* arg1, char* arg2) {
    printf("[DEBUG] Creating TAC operation type: %d\n", op);
    printf("[DEBUG] Input - Result: %s, Arg1: %s, Arg2: %s\n", 
           result ? result : "NULL", 
           arg1 ? arg1 : "NULL", 
           arg2 ? arg2 : "NULL");
    
    TACOp* tac_op = (TACOp*)calloc(1, sizeof(TACOp)); // Use calloc to initialize all fields to 0
    if (!tac_op) {
        printf("[DEBUG] Failed to allocate TAC operation\n");
        return NULL;
    }
    
    tac_op->op = op;
    if (result) {
        tac_op->result = result;
        if (!tac_op->result) {
            printf("[DEBUG] Failed to duplicate result string\n");
            free(tac_op);
            return NULL;
        }
    }
    
    if (arg1) {
        tac_op->arg1 = malloc(strlen(arg1) + 1);
        if (!tac_op->arg1) {
            printf("[DEBUG] Failed to duplicate arg1 string\n");
            free(tac_op->result);
            free(tac_op);
            return NULL;
        }
        strcpy(tac_op->arg1, arg1);
    }
    
    if (arg2) {
        tac_op->arg2 = malloc(strlen(arg2) + 1);
        if (!tac_op->arg2) {
            printf("[DEBUG] Failed to duplicate arg2 string\n");
            free(tac_op->result);
            free(tac_op->arg1);
            free(tac_op);
            return NULL;
        }
        strcpy(tac_op->arg2, arg2);
    }
    
    tac_op->next = NULL;
    
    printf("[DEBUG] Successfully created TAC operation\n");
    return tac_op;
}

void add_tac_op(TACOp* op) {
    if (!op) {
        printf("[DEBUG] Attempting to add NULL TAC operation\n");
        return;
    }

    printf("[DEBUG] Adding TAC operation type: %d\n", op->op);
    printf("[DEBUG] Operation details - Result: %s, Arg1: %s, Arg2: %s\n",
           op->result ? op->result : "NULL",
           op->arg1 ? op->arg1 : "NULL",
           op->arg2 ? op->arg2 : "NULL");

    // Ensure op->next is NULL
    op->next = NULL;

    if (!tac_ctx.first) {
        printf("[DEBUG] First TAC operation\n");
        tac_ctx.first = op;
        tac_ctx.last = op;
    } else if (tac_ctx.last) {
        printf("[DEBUG] Appending TAC operation\n");
        tac_ctx.last->next = op;
        tac_ctx.last = op;
    } else {
        printf("[DEBUG] Error: Inconsistent TAC context state\n");
        free(op->result);
        free(op->arg1);
        free(op->arg2);
        free(op);
        return;
    }
    printf("[DEBUG] TAC operation added successfully\n");
}

char* generate_expression(ASTNode* node) {
    if (!node) {
        printf("[DEBUG] Null node in generate_expression\n");
        return NULL;
    }

    printf("[DEBUG] Generating expression for node type: %d\n", node->type);

    switch (node->type) {
        case NODE_LITERAL: {
            printf("[DEBUG] Handling literal node\n");
            printf("[DEBUG] Literal type: %d\n", node->data.literal.literal_type);
            
            char* temp = new_temp();
            char value_str[64];
    
            switch (node->data.literal.literal_type) {
                case INT_CONST:
                    printf("[DEBUG] Int value: %d\n", node->data.literal.literal.int_value);
                    snprintf(value_str, sizeof(value_str), "%d", 
                            node->data.literal.literal.int_value);
                    printf("[DEBUG] Formatted int string: %s\n", value_str);
                    break;
                    
                case FLOAT_CONST:
                    printf("[DEBUG] Float value: %f\n", node->data.literal.literal.float_value);    
                    snprintf(value_str, sizeof(value_str), "%.1f", 
                            node->data.literal.literal.float_value);
                    printf("[DEBUG] Formatted float string: %s\n", value_str);
                    break;
                    
                default:
                    printf("[DEBUG] Unknown literal type: %d\n", 
                           node->data.literal.literal_type);
                    free(temp);
                    return NULL;
            }
            
            TACOp* op = create_tac_op(
                node->data.literal.literal_type == INT_CONST ? TAC_LOAD_INT : TAC_LOAD_FLOAT,
                temp, value_str, NULL);
            if (!op) {
                free(temp);
                return NULL;
            }
            
            add_tac_op(op);
            return temp;
        }

        case NODE_IDENTIFIER:
            // if (!node->token.lexeme) {
            //     printf("[DEBUG] Null lexeme in identifier\n");
            //     return NULL;
            // }
            return (node->token.lexeme);

        case NODE_BINARY_OP: {
            if (!node->data.binary.left || !node->data.binary.right) {
                printf("[DEBUG] Null operand in binary operation\n");
                return NULL;
            }
            char* left = generate_expression(node->data.binary.left);
            char* right = generate_expression(node->data.binary.right);
            if (!left || !right) {
                printf("[DEBUG] Failed to generate expression for operands\n");
                free(left);
                free(right);
                return NULL;
            }
            char* temp = new_temp();
            TACOpType op;

            switch (node->data.binary.op) {
                case PLUS:  op = TAC_ADD; break;
                case MINUS: op = TAC_SUB; break;
                case MULT:  op = TAC_MUL; break;
                case DIV:   op = TAC_DIV; break;
                case MOD:   op = TAC_MOD; break;
                case LT:    op = TAC_LT;  break;
                case GT:    op = TAC_GT;  break;
                case LEQ:   op = TAC_LEQ; break;
                case GEQ:   op = TAC_GEQ; break;
                case EQ:    op = TAC_EQ;  break;
                case NEQ:   op = TAC_NEQ; break;
                case AND:   op = TAC_AND; break;
                case OR:    op = TAC_OR;  break;
                default:    
                    printf("[DEBUG] Unknown binary operator\n");
                    free(left);
                    free(right);
                    free(temp);
                    return NULL;
            }

            TACOp* tac_op = create_tac_op(op, temp, left, right);
            if (!tac_op) {
                printf("[DEBUG] Failed to create TAC operation\n");
                free(left);
                free(right);
                free(temp);
                return NULL;
            }
            add_tac_op(tac_op);
            return temp;
        }

        default:
            printf("[DEBUG] Unhandled node type in generate_expression: %d\n", node->type);
            return NULL;
    }
}

void generate_tac(ASTNode* node) {
    if (!node) {
        printf("[DEBUG] Null node in generate_tac\n");
        return;
    }

    printf("[DEBUG] Generating TAC for node type: %d\n", node->type);

    switch (node->type) {
        case NODE_PROGRAM: {
            printf("[DEBUG] Generating TAC for program\n");
            if (!node->data.block.statements) {
                printf("[DEBUG] No statements in program\n");
                return;
            }
            for (int i = 0; i < node->data.block.count; i++) {
                if (node->data.block.statements[i]) {
                    generate_tac(node->data.block.statements[i]);
                }
            }
            break;
        }
        
        case NODE_FUNCTION: {
            printf("[DEBUG] Generating TAC for function\n");
            
            // Get function name from the function node
            const char* func_name = NULL;
            func_name = node->data.function.id->token.lexeme;
            
            // Add function entry label
            TACOp* label_op = create_tac_op(TAC_LABEL, (char*)func_name, NULL, NULL);
            if (label_op) {
                add_tac_op(label_op);
                printf("[DEBUG] Added function label: %s\n", func_name ? func_name : "");
            }
            
            // Generate code for function body
            if (node->data.function.body) {
                generate_tac(node->data.function.body);
            }
            break;
        }

        case NODE_PARAM: {
            // Parameters are handled during function calls
            break;
        }

        case NODE_VAR_DECL: {
            // Variable declarations don't generate TAC code directly
            break;
        }
        
        case NODE_BLOCK: {
            printf("[DEBUG] Generating TAC for block\n");
            if (!node->data.block.statements) {
                printf("[DEBUG] No statements in block\n");
                return;
            }
            for (int i = 0; i < node->data.block.count; i++) {
                if (node->data.block.statements[i]) {
                    generate_tac(node->data.block.statements[i]);
                }
            }
            break;
        }

        case NODE_ASSIGN: {
            printf("[DEBUG] Generating assignment TAC\n");
            if (!node->data.binary.left || !node->data.binary.right) {
                printf("[DEBUG] Null operand in assignment\n");
                return;
            }
            char* right = generate_expression(node->data.binary.right);
            if (!right) {
                printf("[DEBUG] Failed to generate right-hand expression\n");
                return;
            }
            // if (!node->data.binary.left->token.lexeme) {
            //     printf("[DEBUG] Null lexeme in assignment target\n");
            //     free(right);
            //     return;
            // }
            TACOp* op = create_tac_op(TAC_ASSIGN, 
                (node->data.binary.left->token.lexeme), right, NULL);
            if (op) {
                add_tac_op(op);
            }
            break;
        }
        case NODE_RETURN: {
            printf("[DEBUG] Generating TAC for return statement\n");
            if (node->data.return_stmt.expr) {
                char* expr_result = generate_expression(node->data.return_stmt.expr);
                if (expr_result) {
                    TACOp* op = create_tac_op(TAC_RETURN, expr_result, NULL, NULL);
                    if (op) {
                        add_tac_op(op);
                    }
                }
            } else {
                TACOp* op = create_tac_op(TAC_RETURN, NULL, NULL, NULL);
                if (op) {
                    add_tac_op(op);
                }
            }
            break;
        }

        case NODE_EXPRESSION:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_IDENTIFIER:
        case NODE_LITERAL: {
            // These nodes are handled by generate_expression()
            generate_expression(node);
            break;
        }

        default:
            printf("[DEBUG] Unhandled node type in generate_tac: %d\n", node->type);
            break;
    }
}

void print_tac() {
    printf("\n=== Generated TAC ===\n");
    TACOp* current = tac_ctx.first;
    while (current) {
        switch (current->op) {
            case TAC_LABEL:
                printf("%s:\n", current->result ? current->result : "NULL");
                break;
            case TAC_LOAD_INT:
                printf("%s = %s\n", 
                    current->result ? current->result : "NULL",
                    current->arg1 ? current->arg1 : "NULL");
                break;
            case TAC_LOAD_FLOAT:
                printf("%s = %s\n", 
                    current->result ? current->result : "NULL",
                    current->arg1 ? current->arg1 : "NULL");
                break;
            case TAC_ASSIGN:
                printf("%s = %s\n", 
                    current->result ? current->result : "NULL",
                    current->arg1 ? current->arg1 : "NULL");
                break;
            case TAC_ADD:
                printf("%s = %s + %s\n",
                    current->result ? current->result : "NULL",
                    current->arg1 ? current->arg1 : "NULL",
                    current->arg2 ? current->arg2 : "NULL");
                break;
            case TAC_RETURN:
                printf("return %s\n", 
                    current->result ? current->result : "");
                break;
            default:
                printf("[DEBUG] Unknown TAC operation type: %d\n", current->op);
                break;
        }
        current = current->next;
    }
    printf("=== End TAC ===\n\n");
}


void save_tac_to_file(const char* filename) {
    printf("[DEBUG] Opening file for TAC: %s\n", filename);
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return;
    }

    printf("[DEBUG] Starting to write TAC operations\n");
    TACOp* current = tac_ctx.first;
    while (current) {
        switch (current->op) {
            case TAC_ADD:
                fprintf(file, "%s = %s + %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_SUB:
                fprintf(file, "%s = %s - %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_MUL:
                fprintf(file, "%s = %s * %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_DIV:
                fprintf(file, "%s = %s / %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_MOD:
                fprintf(file, "%s = %s %% %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_ASSIGN:
                fprintf(file, "%s = %s\n", 
                    current->result, current->arg1);
                break;
            case TAC_LABEL:
                fprintf(file, "%s:\n", current->result);
                break;
            case TAC_JUMP:
                fprintf(file, "goto %s\n", current->result);
                break;
            case TAC_JUMPZ:
                fprintf(file, "if %s == 0 goto %s\n", 
                    current->arg1, current->result);
                break;
            case TAC_JUMPNZ:
                fprintf(file, "if %s != 0 goto %s\n", 
                    current->arg1, current->result);
                break;
            case TAC_LT:
                fprintf(file, "%s = %s < %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_GT:
                fprintf(file, "%s = %s > %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_LEQ:
                fprintf(file, "%s = %s <= %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_GEQ:
                fprintf(file, "%s = %s >= %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_EQ:
                fprintf(file, "%s = %s == %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_NEQ:
                fprintf(file, "%s = %s != %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_AND:
                fprintf(file, "%s = %s && %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_OR:
                fprintf(file, "%s = %s || %s\n", 
                    current->result, current->arg1, current->arg2);
                break;
            case TAC_NOT:
                fprintf(file, "%s = !%s\n", 
                    current->result, current->arg1);
                break;
            case TAC_LOAD_INT:
                fprintf(file, "%s = %s\n", 
                    current->result, current->arg1);
                break;
            case TAC_LOAD_FLOAT:
                fprintf(file, "%s = %s\n", 
                    current->result, current->arg1);
                break;
            case TAC_LOAD_STRING:
                fprintf(file, "%s = \"%s\"\n", 
                    current->result, current->arg1);
                break;
            case TAC_PARAM:
                fprintf(file, "param %s\n", current->result);
                break;
            case TAC_CALL:
                if (current->result)
                    fprintf(file, "%s = call %s\n", 
                        current->result, current->arg1);
                else
                    fprintf(file, "call %s\n", current->arg1);
                break;
            case TAC_RETURN:
                if (current->result)
                    fprintf(file, "return %s\n", current->result);
                else
                    fprintf(file, "return\n");
                break;
        }
        current = current->next;
    }

    fclose(file);
}