#ifndef AST_H
#define AST_H
#include <string.h>
#include <stdlib.h>
#include "types.h"

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* create_binary_op(TokenType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_BINARY_OP);
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* create_literal_int(int value) {
    ASTNode* node = create_node(NODE_LITERAL);
    node->literal.literal_type = INT_CONST;
    node->literal.int_value = value;
    return node;
}

ASTNode* create_literal_float(float value) {
    ASTNode* node = create_node(NODE_LITERAL);
    node->literal.literal_type = FLOAT_CONST;
    node->literal.float_value = value;
    return node;
}

ASTNode* create_identifier(Token token) {
    ASTNode* node = create_node(NODE_IDENTIFIER);
    node->token = token;
    return node;
}
ASTNode* create_literal_string(char* value) {
    ASTNode* node = create_node(NODE_LITERAL);
    node->literal.literal_type = STRING_CONST;
    node->literal.string_value = strdup(value);
    return node;
}

ASTNode* create_var_decl(TokenType type, ASTNode* id, ASTNode* dims) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    node->var_decl.var_type = type;
    node->var_decl.id = id;
    node->var_decl.dims = dims;
    return node;
}

ASTNode* create_function(TokenType return_type, ASTNode* id, ASTNode* params, ASTNode* body) {
    ASTNode* node = create_node(NODE_FUNCTION);
    node->function.return_type = return_type;
    node->function.id = id;
    node->function.params = params;
    node->function.body = body;
    return node;
}

ASTNode* create_if(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = create_node(NODE_IF);
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* create_for(ASTNode* init, ASTNode* condition, ASTNode* increment, ASTNode* body) {
    ASTNode* node = create_node(NODE_FOR);
    node->for_stmt.init = init;
    node->for_stmt.condition = condition;
    node->for_stmt.increment = increment;
    node->for_stmt.body = body;
    return node;
}

ASTNode* create_return(ASTNode* expr) {
    ASTNode* node = create_node(NODE_RETURN);
    node->return_stmt.expr = expr;
    return node;
}

ASTNode* create_call(ASTNode* function, ASTNode* args) {
    ASTNode* node = create_node(NODE_CALL);
    node->call.function = function;
    node->call.args = args;
    return node;
}

ASTNode* create_block(ASTNode** statements, int count) {
    ASTNode* node = create_node(NODE_BLOCK);
    node->block.statements = statements;
    node->block.count = count;
    return node;
}

ASTNode* create_assign(ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_ASSIGN);
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* create_unary_op(TokenType op, ASTNode* operand) {
    ASTNode* node = create_node(NODE_UNARY_OP);
    node->binary.op = op;
    node->binary.left = operand;
    node->binary.right = NULL;
    return node;
}

void analyze(ASTNode *ast_node);

#endif // AST_H
