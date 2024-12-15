#ifndef AST_H
#define AST_H
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include <stdio.h>

void print_node_type(ASTNode* node);

void debug_node_creation(ASTNode* node);

ASTNode* create_node(NodeType type);

ASTNode* create_binary_op(TokenType op, ASTNode* left, ASTNode* right);

ASTNode* create_literal_int(int value);

ASTNode* create_literal_float(float value);

ASTNode* create_identifier(Token token);

ASTNode* create_literal_string(char* value);

ASTNode* create_var_decl(TokenType type, ASTNode* id, ASTNode* dims);

ASTNode* create_function(TokenType return_type, ASTNode* id, ASTNode* params, ASTNode* body);

ASTNode* create_if(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);

ASTNode* create_for(ASTNode* init, ASTNode* condition, ASTNode* increment, ASTNode* body);

ASTNode* create_return(ASTNode* expr);

ASTNode* create_call(ASTNode* function, ASTNode* args);

ASTNode* create_block(ASTNode** statements, int count);

ASTNode* create_assign(ASTNode* left, ASTNode* right);

ASTNode* create_unary_op(TokenType op, ASTNode* operand);

void analyze(ASTNode *ast_node);

#endif // AST_H
