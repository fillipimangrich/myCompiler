#include "ast.h"
#include <string.h>
#include "types.h"

void print_node_type(ASTNode* node) {
    printf("Created Node Type: ");
    switch(node->type) {
        case NODE_PROGRAM: printf("PROGRAM\n"); break;
        case NODE_FUNCTION: printf("FUNCTION\n"); break;
        case NODE_PARAM: printf("PARAM\n"); break;
        case NODE_VAR_DECL: printf("VAR_DECL\n"); break;
        case NODE_ASSIGN: printf("ASSIGN\n"); break;
        case NODE_IF: printf("IF\n"); break;
        case NODE_FOR: printf("FOR\n"); break;
        case NODE_RETURN: printf("RETURN\n"); break;
        case NODE_CALL: printf("CALL\n"); break;
        case NODE_EXPRESSION: printf("EXPRESSION\n"); break;
        case NODE_BINARY_OP: printf("BINARY_OP\n"); break;
        case NODE_UNARY_OP: printf("UNARY_OP\n"); break;
        case NODE_IDENTIFIER: printf("IDENTIFIER\n"); break;
        case NODE_LITERAL: printf("LITERAL\n"); break;
        case NODE_BLOCK: printf("BLOCK\n"); break;
        default: printf("UNKNOWN\n");
    }
}

void debug_node_creation(ASTNode* node) {
    printf("=== Creating New Node ===\n");
    print_node_type(node);
    printf("Node address: %p\n", (void*)node);
    printf("=====================\n");
}

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    debug_node_creation(node); 
    return node;
}

ASTNode* create_binary_op(TokenType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_BINARY_OP);
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

ASTNode* create_literal_int(int value) {
    ASTNode* node = create_node(NODE_LITERAL);
    if (!node) return NULL;
    
    snprintf(node->token.lexeme, sizeof(node->token.lexeme), "%d", value);
    node->data.literal.literal_type = INT_CONST;
    node->data.literal.literal.int_value = value;
    return node;
}

ASTNode* create_literal_float(float value) {
    ASTNode* node = create_node(NODE_LITERAL);
    if (!node) return NULL;
    
    snprintf(node->token.lexeme, sizeof(node->token.lexeme), "%f", value);
    node->data.literal.literal_type = FLOAT_CONST;
    node->data.literal.literal.float_value = value;
    return node;
}

ASTNode* create_identifier(Token token) {
    ASTNode* node = create_node(NODE_IDENTIFIER);
    node->token = token;
    return node;
}
ASTNode* create_literal_string(char* value) {
    ASTNode* node = create_node(NODE_LITERAL);
    node->data.literal.literal_type = STRING_CONST;
    node->data.literal.literal.string_value = value;
    return node;
}

ASTNode* create_var_decl(TokenType type, ASTNode* id, ASTNode* dims) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    node->data.var_decl.var_type = type;
    node->data.var_decl.id = id;
    node->data.var_decl.dims = dims;
    return node;
}

ASTNode* create_function(TokenType return_type, ASTNode* id, ASTNode* params, ASTNode* body) {
    ASTNode* node = create_node(NODE_FUNCTION);
    node->data.function.return_type = return_type;
    node->data.function.id = id;
    node->data.function.params = params;
    node->data.function.body = body;
    return node;
}

ASTNode* create_if(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = create_node(NODE_IF);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* create_for(ASTNode* init, ASTNode* condition, ASTNode* increment, ASTNode* body) {
    ASTNode* node = create_node(NODE_FOR);
    node->data.for_stmt.init = init;
    node->data.for_stmt.condition = condition;
    node->data.for_stmt.increment = increment;
    node->data.for_stmt.body = body;
    return node;
}

ASTNode* create_return(ASTNode* expr) {
    ASTNode* node = create_node(NODE_RETURN);
    node->data.return_stmt.expr = expr;
    return node;
}

ASTNode* create_call(ASTNode* function, ASTNode* args) {
    ASTNode* node = create_node(NODE_CALL);
    node->data.call.function = function;
    node->data.call.args = args;
    return node;
}

ASTNode* create_block(ASTNode** statements, int count) {
    ASTNode* node = create_node(NODE_BLOCK);
    node->data.block.statements = statements;
    node->data.block.count = count;
    return node;
}

ASTNode* create_assign(ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_ASSIGN);
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

ASTNode* create_unary_op(TokenType op, ASTNode* operand) {
    ASTNode* node = create_node(NODE_UNARY_OP);
    node->data.binary.op = op;
    node->data.binary.left = operand;
    node->data.binary.right = NULL;
    return node;
}

