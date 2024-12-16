#include "parser.h"


void advance() {
    cs.current_token = getNextToken();
    printf("Token: %s , Type: %s\n", cs.current_token.lexeme, TokenString[cs.current_token.type]);
}

bool match(TokenType expected_type) {
    if (cs.current_token.type == expected_type) {
        advance();
        return true;
    } else {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), 
                "Unexpected token '%s'", cs.current_token.lexeme);
        syntax_error(error_msg, &cs.current_token, TokenString[expected_type]);
        return false;
    }
}

ASTNode* program() {
    printf("Entering program()\n");

    ASTNode *node = create_node(NODE_PROGRAM);
    if (!node) {
        semantic_error("Failed to create program node", &cs.current_token);
        return NULL;
    }
    printf("Created program node: %p\n", (void*)node);

    node->data.block.statements = NULL;
    node->data.block.count = 0;

    node->data.block.statements = malloc(sizeof(ASTNode*));
    if (node->data.block.statements == NULL) {
        semantic_error("Memory allocation failed for program statements", &cs.current_token);
        free(node);
        return NULL;
    }
    printf("Allocated statements array\n");

    node->data.block.count = 1;
    printf("Current token type: %d\n", cs.current_token.type);

    if (cs.current_token.type == DEF) {
        printf("Calling funclist()\n");
        node->data.block.statements[0] = funclist();
        if (!node->data.block.statements[0]) {
            semantic_error("Failed to parse function list", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
    } else {
        printf("Calling statement()\n");
        node->data.block.statements[0] = statement();
        if (!node->data.block.statements[0]) {
            semantic_error("Failed to parse statement", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
    }

    printf("Exiting program()\n");
    return node;
}

ASTNode* funclist() {
    printf("funclist\n");

    ASTNode *node = create_node(NODE_BLOCK);
    if (!node) {
        semantic_error("Failed to create block node for function list", &cs.current_token);
        return NULL;
    }

    node->data.block.statements = malloc(sizeof(ASTNode*));
    if (!node->data.block.statements) {
        semantic_error("Memory allocation failed for function list statements", &cs.current_token);
        free(node);
        return NULL;
    }
    node->data.block.count = 0;

    if (cs.current_token.type == DEF) {
        // Reallocate to make room for new function
        node->data.block.count++;
        ASTNode **temp = realloc(node->data.block.statements, 
                                node->data.block.count * sizeof(ASTNode*));
        if (!temp) {
            semantic_error("Memory reallocation failed for function list", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
        node->data.block.statements = temp;
        
        // Add the function definition to statements
        node->data.block.statements[node->data.block.count - 1] = funcdef();
        if (!node->data.block.statements[node->data.block.count - 1]) {
            semantic_error("Failed to parse function definition", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
        
        // Recursively process remaining functions
        ASTNode *rest = funclist();
        if (!rest) {
            semantic_error("Failed to parse remaining functions", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }

        if (rest->data.block.count > 0) {
            // Add statements from recursive call
            int old_count = node->data.block.count;
            node->data.block.count += rest->data.block.count;
            temp = realloc(node->data.block.statements,
                          node->data.block.count * sizeof(ASTNode*));
            if (!temp) {
                semantic_error("Memory reallocation failed when merging function lists", &cs.current_token);
                free(node->data.block.statements);
                free(node);
                free(rest->data.block.statements);
                free(rest);
                return NULL;
            }
            node->data.block.statements = temp;
            
            for (int i = 0; i < rest->data.block.count; i++) {
                node->data.block.statements[old_count + i] = rest->data.block.statements[i];
            }
            free(rest->data.block.statements);
            free(rest);
        }
    }

    return node;
}

ASTNode* funcdef() {
    printf("funcdef\n");

    if (!match(DEF)) {
        syntax_error("Expected 'def' keyword", &cs.current_token, "def");
        return NULL;
    }
    
    // Get return type
    TokenType return_type = cs.current_token.type;
    if (!type()) {
        syntax_error("Expected return type", &cs.current_token, "type");
        return NULL;
    }
    
    // Get function name
    Token func_name = cs.current_token;
    if (!match(FUNC_IDENT)) {
        syntax_error("Expected function identifier", &cs.current_token, "function identifier");
        return NULL;
    }
    
    if (!match(LPAREN)) {
        syntax_error("Expected '('", &cs.current_token, "(");
        return NULL;
    }

    ASTNode *params = paramlist();
    if (!params) {
        return NULL;
    }

    if (!match(RPAREN)) {
        syntax_error("Expected ')'", &cs.current_token, ")");
        free_ast_node(params);
        return NULL;
    }
    
    if (!match(LBRACE)) {
        syntax_error("Expected '{'", &cs.current_token, "{");
        free_ast_node(params);
        return NULL;
    }

    ASTNode *body = statelist();
    if (!body) {
        free_ast_node(params);
        return NULL;
    }

    if (!match(RBRACE)) {
        syntax_error("Expected '}'", &cs.current_token, "}");
        free_ast_node(params);
        free_ast_node(body);
        return NULL;
    }

    // Create function node with collected info
    ASTNode *node = create_function(return_type, 
                                  create_identifier(func_name),
                                  params,
                                  body);

    if (!node) {
        free_ast_node(params);
        free_ast_node(body);
        return NULL;
    }

    return node;
}

ASTNode* paramlist() {
    printf("paramlist\n");

    ASTNode *node = create_node(NODE_BLOCK);
    if (!node) {
        semantic_error("Failed to create block node for parameter list", &cs.current_token);
        return NULL;
    }

    node->data.block.statements = malloc(sizeof(ASTNode*));
    if (!node->data.block.statements) {
        semantic_error("Failed to allocate memory for parameter list", &cs.current_token);
        free(node);
        return NULL;
    }
    node->data.block.count = 0;

    if (cs.current_token.type == INT || cs.current_token.type == FLOAT || cs.current_token.type == STRING) {
        TokenType param_type = cs.current_token.type;
        if (!type()) {
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
        
        Token param_name = cs.current_token;
        if (!match(IDENT)) {
            free(node->data.block.statements);
            free(node);
            return NULL;
        }

        // Create and add parameter node
        node->data.block.count++;
        ASTNode **new_statements = realloc(node->data.block.statements, 
                                         node->data.block.count * sizeof(ASTNode*));
        if (!new_statements) {
            semantic_error("Failed to reallocate memory for parameter", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
        node->data.block.statements = new_statements;

        node->data.block.statements[node->data.block.count - 1] = create_var_decl(param_type,
                                                                       create_identifier(param_name),
                                                                       NULL);
        if (!node->data.block.statements[node->data.block.count - 1]) {
            semantic_error("Failed to create parameter node", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }

        // Process additional parameters
        ASTNode *rest = optional_paramlist();
        if (rest) {
            if (rest->data.block.count > 0) {
                int old_count = node->data.block.count;
                node->data.block.count += rest->data.block.count;
                new_statements = realloc(node->data.block.statements,
                                       node->data.block.count * sizeof(ASTNode*));
                if (!new_statements) {
                    semantic_error("Failed to reallocate memory for additional parameters", &cs.current_token);
                    free(node->data.block.statements);
                    free(node);
                    free(rest->data.block.statements);
                    free(rest);
                    return NULL;
                }
                node->data.block.statements = new_statements;
                
                for (int i = 0; i < rest->data.block.count; i++) {
                    node->data.block.statements[old_count + i] = rest->data.block.statements[i];
                }
                free(rest->data.block.statements);
                free(rest);
            } else {
                free(rest->data.block.statements);
                free(rest);
            }
        }
    }

    return node;
}

ASTNode* optional_paramlist() {
    printf("optional_paramlist\n");

    ASTNode *node = create_node(NODE_BLOCK);
    if (!node) {
        semantic_error("Failed to create block node", &cs.current_token);
        return NULL;
    }

    node->data.block.statements = malloc(sizeof(ASTNode*));
    if (!node->data.block.statements) {
        semantic_error("Failed to allocate memory for parameter list", &cs.current_token);
        free(node);
        return NULL;
    }
    node->data.block.count = 0;

    if (cs.current_token.type == COMMA) {
        match(COMMA);
        TokenType param_type = cs.current_token.type;
        type();
        
        Token param_name = cs.current_token;
        match(IDENT);

        // Create and add parameter node
        node->data.block.count++;
        ASTNode **new_statements = realloc(node->data.block.statements, 
                                         node->data.block.count * sizeof(ASTNode*));
        if (!new_statements) {
            semantic_error("Failed to reallocate memory for parameter", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }
        node->data.block.statements = new_statements;

        node->data.block.statements[node->data.block.count - 1] = create_var_decl(param_type,
                                                                       create_identifier(param_name),
                                                                       NULL);
        if (!node->data.block.statements[node->data.block.count - 1]) {
            semantic_error("Failed to create parameter node", &cs.current_token);
            free(node->data.block.statements);
            free(node);
            return NULL;
        }

        // Process additional parameters recursively
        ASTNode *rest = optional_paramlist();
        if (rest && rest->data.block.count > 0) {
            int old_count = node->data.block.count;
            node->data.block.count += rest->data.block.count;
            new_statements = realloc(node->data.block.statements,
                                   node->data.block.count * sizeof(ASTNode*));
            if (!new_statements) {
                semantic_error("Failed to reallocate memory for additional parameters", &cs.current_token);
                free(node->data.block.statements);
                free(node);
                free(rest->data.block.statements);
                free(rest);
                return NULL;
            }
            node->data.block.statements = new_statements;
            
            for (int i = 0; i < rest->data.block.count; i++) {
                node->data.block.statements[old_count + i] = rest->data.block.statements[i];
            }
            free(rest->data.block.statements);
            free(rest);
        }
    }

    return node;
}

ASTNode* type() {
    printf("type\n");

    ASTNode *node = create_node(NODE_LITERAL);
    if (!node) {
        semantic_error("Failed to create node for type", &cs.current_token);
        return NULL;
    }

    node->data.literal.literal_type = cs.current_token.type;

    if (cs.current_token.type == INT) {
        if (!match(INT)) {
            free(node);
            return NULL;
        }
    } else if (cs.current_token.type == FLOAT) {
        if (!match(FLOAT)) {
            free(node);
            return NULL;
        }
    } else if (cs.current_token.type == STRING) {
        if (!match(STRING)) {
            free(node);
            return NULL;
        }
    } else {
        syntax_error("Invalid type", &cs.current_token, "int, float or string");
        free(node);
        return NULL;
    }

    return node;
}

ASTNode* statement() {
    printf("statement\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == INT || cs.current_token.type == FLOAT || cs.current_token.type == STRING) {
        node = vardecl();
        if (!node) {
            semantic_error("Failed to parse variable declaration", &cs.current_token);
            return NULL;
        }
        if (!match(SEMICOLON)) {
            free_ast_node(node);
            return NULL;
        }
    } else if (cs.current_token.type == IDENT) {
        node = atribstat();
        if (!node) {
            semantic_error("Failed to parse assignment statement", &cs.current_token);
            return NULL;
        }
        if (!match(SEMICOLON)) {
            free_ast_node(node);
            return NULL;
        }
    } else if (cs.current_token.type == RETURN) {
        node = returnstat();
        if (!node) {
            semantic_error("Failed to parse return statement", &cs.current_token);
            return NULL;
        }
        if (!match(SEMICOLON)) {
            free_ast_node(node);
            return NULL;
        }
    } else if (cs.current_token.type == IF) {
        node = ifstat();
        if (!node) {
            semantic_error("Failed to parse if statement", &cs.current_token);
            return NULL;
        }
    } else if (cs.current_token.type == FOR) {
        node = forstat();
        if (!node) {
            semantic_error("Failed to parse for statement", &cs.current_token);
            return NULL;
        }
    } else if (cs.current_token.type == LBRACE) {
        if (!match(LBRACE)) {
            return NULL;
        }
        node = statelist();
        if (!node) {
            semantic_error("Failed to parse statement list", &cs.current_token);
            return NULL;
        }
        if (!match(RBRACE)) {
            free_ast_node(node);
            return NULL;
        }
    } else if (cs.current_token.type == SEMICOLON) {
        if (!match(SEMICOLON)) {
            return NULL;
        }
        node = create_node(NODE_BLOCK); // Empty statement
        if (!node) {
            semantic_error("Failed to create empty statement node", &cs.current_token);
            return NULL;
        }
        node->data.block.statements = NULL;
        node->data.block.count = 0;
    } else {
        syntax_error("Invalid statement", &cs.current_token, "declaration, assignment, return, if, for, or block");
        return NULL;
    }

    return node;
}

ASTNode* vardecl() {
    printf("vardecl\n");

    TokenType var_type = cs.current_token.type;
    if (!type()) {
        syntax_error("Expected type declaration", &cs.current_token, "type");
        return NULL;
    }

    Token id_token = cs.current_token;
    ASTNode* id_node = create_identifier(id_token);
    if (!id_node) {
        semantic_error("Failed to create identifier node", &cs.current_token);
        return NULL;
    }

    if (!match(IDENT)) {
        free_ast_node(id_node);
        return NULL;
    }

    ASTNode* dims = int_const();
    // int_const() can return NULL for non-array variables, so no error check needed

    ASTNode* node = create_var_decl(var_type, id_node, dims);
    if (!node) {
        semantic_error("Failed to create variable declaration node", &cs.current_token);
        free_ast_node(id_node);
        free_ast_node(dims);
        return NULL;
    }

    return node;
}

ASTNode* int_const() {
    printf("int_const\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == LBRACK) {
        if (!match(LBRACK)) {
            return NULL;
        }

        if (cs.current_token.type != INT_CONST) {
            syntax_error("Expected integer constant", &cs.current_token, "integer constant");
            return NULL;
        }

        int value = atoi(cs.current_token.lexeme);
        ASTNode* literal = create_literal_int(value);
        if (!literal) {
            semantic_error("Failed to create integer literal node", &cs.current_token);
            return NULL;
        }

        if (!match(INT_CONST)) {
            free_ast_node(literal);
            return NULL;
        }

        if (!match(RBRACK)) {
            free_ast_node(literal);
            return NULL;
        }

        literal->next = int_const();
        // int_const() can return NULL for non-array dimensions, so no error check needed
        node = literal;
    }

    return node;
}

ASTNode* atribstat(){
    printf("atribstat\n");
    
    ASTNode* left = lvalue();
    if (!left) {
        return NULL;
    }

    if (!match(ASSIGN)) {
        free_ast_node(left);
        return NULL;
    }

    ASTNode* right = stat();
    if (!right) {
        free_ast_node(left);
        return NULL;
    }

    ASTNode* node = create_assign(left, right);
    if (!node) {
        semantic_error("Failed to create assignment node", &cs.current_token);
        free_ast_node(left);
        free_ast_node(right);
        return NULL;
    }

    return node;
}

ASTNode* stat(){
    printf("stat\n");
    
    if(cs.current_token.type == FUNC_IDENT){
        return funccall();
    } else {
        return expression();
    }
}

ASTNode* funccall(){
    printf("funccall\n");
    
    Token func_token = cs.current_token;
    ASTNode* func_node = create_identifier(func_token);
    if (!func_node) {
        semantic_error("Failed to create function identifier node", &cs.current_token);
        return NULL;
    }

    if (!match(FUNC_IDENT)) {
        free_ast_node(func_node);
        return NULL;
    }

    if (!match(LPAREN)) {
        free_ast_node(func_node);
        return NULL;
    }

    ASTNode* args = paramlistcall();
    // args can be NULL for no parameters, so no error check needed

    if (!match(RPAREN)) {
        free_ast_node(func_node);
        free_ast_node(args);
        return NULL;
    }

    ASTNode* node = create_call(func_node, args);
    if (!node) {
        semantic_error("Failed to create function call node", &cs.current_token);
        free_ast_node(func_node);
        free_ast_node(args);
        return NULL;
    }

    return node;
}

ASTNode* paramlistcall(){
    printf("paramlistcall\n");
    if(cs.current_token.type == IDENT){
        Token id_token = cs.current_token;
        ASTNode* id_node = create_identifier(id_token);
        if (!id_node) {
            semantic_error("Failed to create identifier node", &cs.current_token);
            return NULL;
        }

        if (!match(IDENT)) {
            free_ast_node(id_node);
            return NULL;
        }

        id_node->next = optional_paramlistcall();
        return id_node;
    }
    return NULL;
}

ASTNode* optional_paramlistcall(){
    printf("optional_paramlistcall\n");
    if (cs.current_token.type == COMMA) {
        if (!match(COMMA)) {
            return NULL;
        }

        if (cs.current_token.type != IDENT) {
            syntax_error("Expected identifier after comma", &cs.current_token, "identifier");
            return NULL;
        }

        Token id_token = cs.current_token;
        ASTNode* id_node = create_identifier(id_token);
        if (!id_node) {
            semantic_error("Failed to create identifier node", &cs.current_token);
            return NULL;
        }

        if (!match(IDENT)) {
            free_ast_node(id_node);
            return NULL;
        }

        id_node->next = optional_paramlistcall();
        return id_node;
    }
    return NULL;
}

ASTNode* returnstat(){
    printf("returnstat\n");
    if (!match(RETURN)) {
        return NULL;
    }

    ASTNode* expr = NULL;
    if (cs.current_token.type == IDENT) {
        Token id_token = cs.current_token;
        expr = create_identifier(id_token);
        if (!expr) {
            semantic_error("Failed to create identifier node", &cs.current_token);
            return NULL;
        }

        if (!match(IDENT)) {
            free_ast_node(expr);
            return NULL;
        }
    }

    ASTNode* return_node = create_return(expr);
    if (!return_node) {
        semantic_error("Failed to create return node", &cs.current_token);
        free_ast_node(expr);
        return NULL;
    }

    return return_node;
}
ASTNode* optional_ident() {
    printf("optional_ident\n");
    if (cs.current_token.type == IDENT) {
        Token id_token = cs.current_token;
        ASTNode* id_node = create_identifier(id_token);
        if (!id_node) {
            semantic_error("Failed to create identifier node", &cs.current_token);
            return NULL;
        }
        if (!match(IDENT)) {
            free_ast_node(id_node);
            return NULL;
        }
        return id_node;
    }
    return NULL;
}

ASTNode* ifstat() {
    printf("ifstat\n");
    if (!match(IF)) {
        return NULL;
    }
    if (!match(LPAREN)) {
        return NULL;
    }
    
    ASTNode* condition = expression();
    if (!condition) {
        return NULL;
    }
    
    if (!match(RPAREN)) {
        free_ast_node(condition);
        return NULL;
    }
    
    ASTNode* then_branch = statement();
    if (!then_branch) {
        free_ast_node(condition);
        return NULL;
    }
    
    ASTNode* else_branch = optional_else();
    // optional_else() can return NULL, that's ok
    
    ASTNode* if_node = create_if(condition, then_branch, else_branch);
    if (!if_node) {
        semantic_error("Failed to create if node", &cs.current_token);
        free_ast_node(condition);
        free_ast_node(then_branch);
        if (else_branch) free_ast_node(else_branch);
        return NULL;
    }
    
    return if_node;
}

ASTNode* optional_else() {
    printf("optional_else\n");
    if (cs.current_token.type == ELSE) {
        if (!match(ELSE)) {
            syntax_error("Expected 'else' keyword", &cs.current_token, "else");
            return NULL;
        }
        ASTNode* else_branch = statement();
        if (!else_branch) {
            semantic_error("Failed to parse else branch", &cs.current_token);
            return NULL;
        }
        return else_branch;
    }
    return NULL;
}

ASTNode* forstat() {
    printf("forstat\n");
    if (!match(FOR)) {
        syntax_error("Expected 'for' keyword", &cs.current_token, "for");
        return NULL;
    }
    if (!match(LPAREN)) {
        syntax_error("Expected '('", &cs.current_token, "(");
        return NULL;
    }

    ASTNode* init = atribstat();
    if (!init) {
        semantic_error("Failed to parse for loop initialization", &cs.current_token);
        return NULL;
    }

    if (!match(SEMICOLON)) {
        syntax_error("Expected ';'", &cs.current_token, ";");
        free_ast_node(init);
        return NULL;
    }

    ASTNode* condition = expression();
    if (!condition) {
        semantic_error("Failed to parse for loop condition", &cs.current_token);
        free_ast_node(init);
        return NULL;
    }

    if (!match(SEMICOLON)) {
        syntax_error("Expected ';'", &cs.current_token, ";");
        free_ast_node(init);
        free_ast_node(condition);
        return NULL;
    }

    ASTNode* increment = atribstat();
    if (!increment) {
        semantic_error("Failed to parse for loop increment", &cs.current_token);
        free_ast_node(init);
        free_ast_node(condition);
        return NULL;
    }

    if (!match(RPAREN)) {
        syntax_error("Expected ')'", &cs.current_token, ")");
        free_ast_node(init);
        free_ast_node(condition);
        free_ast_node(increment);
        return NULL;
    }

    ASTNode* body = statement();
    if (!body) {
        semantic_error("Failed to parse for loop body", &cs.current_token);
        free_ast_node(init);
        free_ast_node(condition);
        free_ast_node(increment);
        return NULL;
    }

    ASTNode* for_node = create_for(init, condition, increment, body);
    if (!for_node) {
        semantic_error("Failed to create for loop node", &cs.current_token);
        free_ast_node(init);
        free_ast_node(condition);
        free_ast_node(increment);
        free_ast_node(body);
        return NULL;
    }

    return for_node;
}

ASTNode* statelist() {
    printf("statelist\n");
    ASTNode* first_statement = statement();
    if (!first_statement) {
        semantic_error("Failed to parse first statement", &cs.current_token);
        return NULL;
    }

    ASTNode* rest = optional_statelist();
    // rest can be NULL, that's ok
    
    // Count total statements
    int count = 1;
    ASTNode* curr = rest;
    while (curr) {
        count++;
        curr = curr->next;
    }
    
    // Create array of statements
    ASTNode** statements = malloc(sizeof(ASTNode*) * count);
    if (!statements) {
        semantic_error("Memory allocation failed for statement list", &cs.current_token);
        free_ast_node(first_statement);
        if (rest) free_ast_node(rest);
        return NULL;
    }
    
    statements[0] = first_statement;
    
    int i = 1;
    curr = rest;
    while (curr) {
        statements[i++] = curr;
        curr = curr->next;
    }
    
    ASTNode* block = create_block(statements, count);
    if (!block) {
        semantic_error("Failed to create block node", &cs.current_token);
        free(statements);
        free_ast_node(first_statement);
        if (rest) free_ast_node(rest);
        return NULL;
    }
    
    return block;
}

ASTNode* optional_statelist() {
    printf("optional_statelist\n");
    if (cs.current_token.type == RBRACE) {
        return NULL;   
    } else {
        ASTNode* result = statelist();
        if (!result) {
            semantic_error("Failed to parse statement list", &cs.current_token);
            return NULL;
        }
        return result;
    }
}

ASTNode* expression() {
    printf("expression\n");
    if (cs.current_token.type == NOT) {
        match(NOT);
        ASTNode* expr = expression();
        if (!expr) {
            semantic_error("Failed to parse expression after NOT", &cs.current_token);
            return NULL;
        }
        ASTNode* result = create_unary_op(NOT, expr);
        if (!result) {
            semantic_error("Failed to create NOT operation node", &cs.current_token);
            free_ast_node(expr);
            return NULL;
        }
        return result;
    } else {
        ASTNode* left = numexpression();
        if (!left) {
            return NULL;
        }
        ASTNode* right = optional_comparator();
        if (right) {
            right->data.binary.left = left;
            return right;
        }
        return left;
    }
}

ASTNode* optional_comparator() {
    printf("optional_comparator\n");
    if (cs.current_token.type == LT || cs.current_token.type == GT || 
        cs.current_token.type == LEQ || cs.current_token.type == GEQ ||
        cs.current_token.type == EQ || cs.current_token.type == NEQ ||
        cs.current_token.type == AND) {
        TokenType op = cs.current_token.type;
        ASTNode* comp = comparator();
        if (!comp) {
            return NULL;
        }
        ASTNode* right = numexpression();
        if (!right) {
            free_ast_node(comp);
            return NULL;
        }
        ASTNode* result = create_binary_op(op, NULL, right);
        if (!result) {
            semantic_error("Failed to create binary operation node", &cs.current_token);
            free_ast_node(right);
            free_ast_node(comp);
            return NULL;
        }
        return result;
    } else if (cs.current_token.type == OR) {
        TokenType op = cs.current_token.type;
        ASTNode* comp = least_precedence_comparator();
        if (!comp) {
            return NULL;
        }
        ASTNode* right = numexpression();
        if (!right) {
            free_ast_node(comp);
            return NULL;
        }
        ASTNode* result = create_binary_op(op, NULL, right);
        if (!result) {
            semantic_error("Failed to create OR operation node", &cs.current_token);
            free_ast_node(right);
            free_ast_node(comp);
            return NULL;
        }
        return result;
    }
    return NULL;
}

ASTNode* least_precedence_comparator() {
    printf("least_precedence_comparator\n");
    if (!match(OR)) {
        syntax_error("Expected 'OR' operator", &cs.current_token, "OR");
        return NULL;
    }
    ASTNode* node = create_binary_op(OR, NULL, NULL);
    if (!node) {
        semantic_error("Failed to create OR operation node", &cs.current_token);
        return NULL;
    }
    return node;
}

ASTNode* comparator() {
    printf("comparator\n");
    TokenType op;
    if (cs.current_token.type == LT) {
        op = LT;
        if (!match(LT)) return NULL;
    } else if (cs.current_token.type == GT) {
        op = GT;
        if (!match(GT)) return NULL;
    } else if (cs.current_token.type == LEQ) {
        op = LEQ;
        if (!match(LEQ)) return NULL;
    } else if (cs.current_token.type == GEQ) {
        op = GEQ;
        if (!match(GEQ)) return NULL;
    } else if (cs.current_token.type == EQ) {
        op = EQ;
        if (!match(EQ)) return NULL;
    } else if (cs.current_token.type == NEQ) {
        op = NEQ;
        if (!match(NEQ)) return NULL;
    } else if (cs.current_token.type == AND) {
        op = AND;
        if (!match(AND)) return NULL;
    } else {
        syntax_error("Invalid comparison operator", &cs.current_token, "comparison operator");
        return NULL;
    }

    ASTNode* node = create_binary_op(op, NULL, NULL);
    if (!node) {
        semantic_error("Failed to create comparison node", &cs.current_token);
        return NULL;
    }
    return node;
}

ASTNode* numexpression() {
    printf("numexpression\n");
    ASTNode* left = term();
    if (!left) {
        return NULL;
    }

    ASTNode* result = numexpressionaux();
    if (result) {
        result->data.binary.left = left;
        return result;
    }
    return left;
}

ASTNode* numexpressionaux() {
    printf("numexpressionaux\n");
    if (cs.current_token.type == PLUS || cs.current_token.type == MINUS) {
        TokenType op = cs.current_token.type;
        ASTNode* op_node = operator();
        if (!op_node) {
            return NULL;
        }
        free_ast_node(op_node); // We only needed the operator match

        ASTNode* right = term();
        if (!right) {
            return NULL;
        }

        ASTNode* result = create_binary_op(op, NULL, right);
        if (!result) {
            semantic_error("Failed to create binary operation node", &cs.current_token);
            free_ast_node(right);
            return NULL;
        }

        ASTNode* next = numexpressionaux();
        if (next) {
            next->data.binary.left = result;
            return next;
        }
        return result;
    }
    return NULL;
}

ASTNode* operator(){
    printf("operator\n");
    TokenType op;
    if(cs.current_token.type == PLUS){
        op = PLUS;
        match(PLUS);
    }else if(cs.current_token.type == MINUS){
        op = MINUS;
        match(MINUS);
    }else {
        printf("Erro de sintaxe: operador inválido\n");
        exit(1);
    }
    return create_binary_op(op, NULL, NULL);
}

ASTNode* term() {
    printf("term\n");
    ASTNode* left = unaryexpr();
    if (!left) {
        return NULL;
    }
    
    ASTNode* result = term_aux();
    if (result != NULL) {
        result->data.binary.left = left;
        return result;
    }
    return left;
}

ASTNode* term_aux() {
    printf("term_Aux\n");
    if (cs.current_token.type == MOD || cs.current_token.type == DIV || 
        cs.current_token.type == MULT) {
        TokenType op = cs.current_token.type;
        
        ASTNode* op_node = high_precedence_operator();
        if (!op_node) {
            return NULL;
        }
        free_ast_node(op_node); // Only needed for the match
        
        ASTNode* right = unaryexpr();
        if (!right) {
            return NULL;
        }

        ASTNode* result = create_binary_op(op, NULL, right);
        if (!result) {
            semantic_error("Failed to create binary operation node", &cs.current_token);
            free_ast_node(right);
            return NULL;
        }

        ASTNode* next = term_aux();
        if (next != NULL) {
            next->data.binary.left = result;
            return next;
        }
        return result;
    }
    return NULL;
}

ASTNode* high_precedence_operator() {
    printf("high_precedence_operator\n");
    TokenType op;
    if (cs.current_token.type == MOD) {
        op = MOD;
        if (!match(MOD)) {
            return NULL;
        }
    } else if (cs.current_token.type == DIV) {
        op = DIV;
        if (!match(DIV)) {
            return NULL;
        }
    } else if (cs.current_token.type == MULT) {
        op = MULT;
        if (!match(MULT)) {
            return NULL;
        }
    } else {
        syntax_error("Invalid high precedence operator", &cs.current_token, "*, / or %");
        return NULL;
    }

    ASTNode* node = create_binary_op(op, NULL, NULL);
    if (!node) {
        semantic_error("Failed to create operator node", &cs.current_token);
        return NULL;
    }
    return node;
}

ASTNode* unaryexpr() {
    printf("unaryexpr\n");
    if (cs.current_token.type == PLUS || cs.current_token.type == MINUS) {
        TokenType op = cs.current_token.type;
        
        ASTNode* op_node = operator();
        if (!op_node) {
            return NULL;
        }
        free_ast_node(op_node); // Only needed for the match
        
        ASTNode* right = factor();
        if (!right) {
            return NULL;
        }

        ASTNode* result = create_binary_op(op, create_literal_int(0), right);
        if (!result) {
            semantic_error("Failed to create unary operation node", &cs.current_token);
            free_ast_node(right);
            return NULL;
        }
        return result;
    } else {
        return factor();
    }
}

ASTNode* factor() {
    printf("factor\n");
    if (cs.current_token.type == INT_CONST) {
        int value = atoi(cs.current_token.lexeme);
        if (!match(INT_CONST)) {
            return NULL;
        }
        ASTNode* node = create_literal_int(value);
        if (!node) {
            semantic_error("Failed to create integer literal node", &cs.current_token);
            return NULL;
        }
        return node;
    } else if (cs.current_token.type == FLOAT_CONST) {
        float value = atof(cs.current_token.lexeme);
        if (!match(FLOAT_CONST)) {
            return NULL;
        }
        ASTNode* node = create_literal_float(value);
        if (!node) {
            semantic_error("Failed to create float literal node", &cs.current_token);
            return NULL;
        }
        return node;
    } else if (cs.current_token.type == STRING_CONST) {
        char* value = cs.current_token.lexeme;
        if (!value) {
            semantic_error("Failed to duplicate string literal", &cs.current_token);
            return NULL;
        }
        if (!match(STRING_CONST)) {
            return NULL;
        }
        ASTNode* node = create_literal_string(value);
        if (!node) {
            semantic_error("Failed to create string literal node", &cs.current_token);
            return NULL;
        }
        return node;
    } else if (cs.current_token.type == IDENT) {
        return lvalue();
    } else if (cs.current_token.type == LPAREN) {
        if (!match(LPAREN)) {
            return NULL;
        }
        ASTNode* expr = numexpression();
        if (!expr) {
            return NULL;
        }
        if (!match(RPAREN)) {
            free_ast_node(expr);
            return NULL;
        }
        return expr;
    } else {
        syntax_error("Expected factor", &cs.current_token, "number, string, identifier or '('");
        return NULL;
    }
}

ASTNode* lvalue() {
    printf("lvalue\n");
    if (cs.current_token.type == IDENT) {
        Token id_token = cs.current_token;
        if (!match(IDENT)) {
            return NULL;
        }
        ASTNode* id_node = create_identifier(id_token);
        if (!id_node) {
            semantic_error("Failed to create identifier node", &cs.current_token);
            return NULL;
        }
        return id_node;
    } else {
        syntax_error("Expected identifier", &cs.current_token, "identifier");
        return NULL;
    }
}

ASTNode* lvalueaux() {
    printf("lvalueaux\n");
    if (cs.current_token.type == LBRACK) {
        if (!match(LBRACK)) {
            return NULL;
        }
        
        ASTNode* index = numexpression();
        if (!index) {
            return NULL;
        }
        
        if (!match(RBRACK)) {
            free_ast_node(index);
            return NULL;
        }
        
        ASTNode* next = lvalueaux();
        if (!next && cs.current_token.type == LBRACK) {
            free_ast_node(index);
            return NULL;
        }

        ASTNode* result = create_binary_op(LBRACK, index, next);
        if (!result) {
            semantic_error("Failed to create array access node", &cs.current_token);
            free_ast_node(index);
            free_ast_node(next);
            return NULL;
        }
        return result;
    }
    return NULL;
}


void free_ast_node(ASTNode* node) {
    if (!node) return;

    // Free any child nodes based on node type
    switch (node->type) {
        case NODE_BLOCK:
            if (node->data.block.statements) {
                for (int i = 0; i < node->data.block.count; i++) {
                    free_ast_node(node->data.block.statements[i]);
                }
                free(node->data.block.statements);
            }
            break;
            
        case NODE_FUNCTION:
            free_ast_node(node->data.function.id);
            free_ast_node(node->data.function.params);
            free_ast_node(node->data.function.body);
            break;
            
        case NODE_BINARY_OP:
            free_ast_node(node->data.binary.left);
            free_ast_node(node->data.binary.right);
            break;
            
        case NODE_UNARY_OP:
            free_ast_node(node->data.binary.left);
            break;
            
        case NODE_VAR_DECL:
            free_ast_node(node->data.var_decl.id);
            break;
            
        case NODE_IF:
            free_ast_node(node->data.if_stmt.condition);
            free_ast_node(node->data.if_stmt.then_branch);
            free_ast_node(node->data.if_stmt.else_branch);
            break;
            
        case NODE_FOR:
            free_ast_node(node->data.for_stmt.init);
            free_ast_node(node->data.for_stmt.condition);
            free_ast_node(node->data.for_stmt.increment);
            free_ast_node(node->data.for_stmt.body);
            break;
            
        case NODE_RETURN:
            free_ast_node(node->data.return_stmt.expr);
            break;
            
        case NODE_CALL:
            free_ast_node(node->data.call.function);
            free_ast_node(node->data.call.args);
            break;
            
        case NODE_LITERAL:
            if (node->data.literal.literal_type == STRING && node->data.literal.literal.string_value) {
                free(node->data.literal.literal.string_value);  
            }
            break;
            
        default:
            break;
    }

    // Free any next node in linked lists
    if (node->next) {
        free_ast_node(node->next);
    }

    // Finally free the node itself
    free(node);
}