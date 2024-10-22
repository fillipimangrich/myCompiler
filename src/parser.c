#include "parser.h"

void advance() {
    cs.current_token = getNextToken();
    printf("Token: %s , Type: %s\n", cs.current_token.lexeme, TokenString[cs.current_token.type]);
}

void match(TokenType expected_type) {
    if (cs.current_token.type == expected_type) {
        advance();
    } else {
        printf("Erro: token inesperado '%s', token esperado: '%s'\n", TokenString[cs.current_token.type], TokenString[expected_type]);
        exit(1);
    }
}

ASTNode* program() {
    printf("program\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == DEF) {
        funclist();
    } else {
        statement();
    }

    return node;
}

ASTNode* funclist() {
    printf("funclist\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == DEF) {
        funcdef();
        funclist(); 
    }

    return node;
}

ASTNode* funcdef() {
    printf("funcdef\n");

    ASTNode *node = NULL;

    match(DEF);
    type();  
    match(FUNC_IDENT);  
    match(LPAREN);  
    paramlist();  
    match(RPAREN);  
    match(LBRACE);  
    statelist();  
    match(RBRACE); 

    return node;
}

ASTNode* paramlist() {
    printf("paramlist\n");

    ASTNode *node = NULL;

    if (cs.current_token.type == INT || cs.current_token.type == FLOAT || cs.current_token.type == STRING) {
        type(); 
        match(IDENT); 
        optional_paramlist();  
    }

    return node;
}

ASTNode* optional_paramlist() {
    printf("optional_paramlist\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == COMMA) {
        match(COMMA);
        type();
        match(IDENT);
        optional_paramlist();
    }

    return node;
}

ASTNode* type() {
    printf("type\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == INT) {
        match(INT);
    } else if (cs.current_token.type == FLOAT) {
        match(FLOAT);
    } else if (cs.current_token.type == STRING) {
        match(STRING);
    }else {
        printf("Erro: Tipo inválido\n");
        exit(1);
    }

    return node;
}

ASTNode* statement() {
    printf("statement\n");

    ASTNode *node = NULL;
    if (cs.current_token.type == INT || cs.current_token.type == FLOAT || cs.current_token.type == STRING) {
        vardecl();
        match(SEMICOLON);
    } else if (cs.current_token.type == IDENT) {
        atribstat();
        match(SEMICOLON);
    } else if (cs.current_token.type == RETURN) {
        returnstat();
        match(SEMICOLON);
    } else if (cs.current_token.type == IF) {
        ifstat();
    } else if (cs.current_token.type == FOR) {
        forstat();
    } else if (cs.current_token.type == LBRACE) {
        match(LBRACE);
        statelist();
        match(RBRACE);
    } else if (cs.current_token.type == SEMICOLON) {
        match(SEMICOLON);
    } else {
        printf("Erro de sintaxe: Declaração inválida\n");
        exit(1);
    }

    return node;
}

ASTNode* vardecl(){
    printf("vardecl\n");

    ASTNode *node = NULL;
    type();
    match(IDENT);
    int_const();

    return node;
}

ASTNode* int_const(){
    printf("int_const\n");

    ASTNode *node = NULL;
    if(cs.current_token.type == LBRACK){
        match(LBRACK);
        match(INT_CONST);
        match(RBRACK);
        int_const();
    }

    return node;
}

ASTNode* atribstat(){
    printf("atribstat\n");
    lvalue();
    match(ASSIGN);
    stat();
}

ASTNode* stat(){
    printf("stat\n");
    if(cs.current_token.type == FUNC_IDENT){
        funccall();
    }else{
        expression();
    }
}

ASTNode* funccall(){
    printf("funccall\n");
    match(FUNC_IDENT);
    match(LPAREN);
    paramlistcall();
    match(RPAREN);
}

ASTNode* paramlistcall(){
    printf("paramlistcall\n");
    if(cs.current_token.type == IDENT){
        match(IDENT);
        optional_paramlistcall();
    }
}

ASTNode* optional_paramlistcall(){
    printf("optional_paramlistcall\n");
    match(COMMA);
    match(IDENT);
    optional_paramlistcall();
}

ASTNode* returnstat(){
    printf("returnstat\n");
    match(RETURN);
    optional_ident();
}

ASTNode* optional_ident(){
    printf("optional_ident\n");
    if(cs.current_token.type == IDENT){
        match(IDENT);
    }
}

ASTNode* ifstat(){
    printf("ifstat\n");
    match(IF);
    match(LPAREN);
    expression();
    match(RPAREN);
    statement();
    optional_else();
}

ASTNode* optional_else(){
    printf("optional_else\n");
    if(cs.current_token.type == ELSE){
        match(ELSE);
        statement();
    }
}

ASTNode* forstat(){
    printf("forstat\n");
    match(FOR);
    match(LPAREN);
    atribstat();
    match(SEMICOLON);
    expression();
    match(SEMICOLON);
    atribstat();
    match(RPAREN);
    statement();
}

ASTNode* statelist(){
    printf("statelist\n");
    statement();
    optional_statelist();
}

ASTNode* optional_statelist(){
    printf("optional_statelist\n");
    if(cs.current_token.type == RBRACE){
        return;   
    }else{
        statelist();
    }
}

ASTNode* expression(){
    printf("expression\n");
    if(cs.current_token.type == NOT){
        match(NOT);
        expression();
    }else{
        numexpression();
        optional_comparator();
    }
}

ASTNode* optional_comparator(){
    printf("optional_comparator\n");
    if(cs.current_token.type == LT || cs.current_token.type == GT || 
    cs.current_token.type == LEQ || cs.current_token.type == GEQ ||
    cs.current_token.type == EQ || cs.current_token.type == NEQ ||
    cs.current_token.type == AND){
        comparator();
    }else if(cs.current_token.type == OR){
        least_precedence_comparator();
    }
}

ASTNode* least_precedence_comparator(){
    printf("least_precedence_comparator\n");
    match(OR);
}

ASTNode* comparator(){
    printf("comparator\n");
    if(cs.current_token.type == LT){
        match(LT);
    }else if(cs.current_token.type == GT){
        match(GT);
    }else if(cs.current_token.type == LEQ){
        match(LEQ);
    }else if(cs.current_token.type == GEQ){
        match(GEQ);
    }else if(cs.current_token.type == EQ){
        match(EQ);
    }else if(cs.current_token.type == NEQ){
        match(NEQ);
    }else if(cs.current_token.type == AND){
        match(AND);
    }else {
        printf("Erro de sintaxe: Comparador inválido\n");
        exit(1);
    }
}

ASTNode* numexpression(){
    printf("numexpression\n");
    term();
    numexpressionaux();
}

ASTNode* numexpressionaux(){
    printf("numexpressionaux\n");
    if(cs.current_token.type == PLUS || cs.current_token.type == MINUS){
        operator();
        term();
        numexpressionaux();
    }
}

ASTNode* operator(){
    printf("operator\n");
    if(cs.current_token.type == PLUS){
        match(PLUS);
    }else if(cs.current_token.type == MINUS){
        match(MINUS);
    }else {
        printf("Erro de sintaxe: operador inválido\n");
        exit(1);
    }
}

ASTNode* term(){
    printf("term\n");
    unaryexpr();
    term_aux();
}

ASTNode* term_aux(){
    printf("term_Aux\n");
    if(cs.current_token.type == MOD || cs.current_token.type == DIV || 
    cs.current_token.type == MULT){
        high_precedence_operator();
        unaryexpr();
        term_aux();
    }
}

ASTNode* high_precedence_operator(){
    printf("high_precedence_operator\n");
    if(cs.current_token.type == MOD){
        match(MOD);
    }else if(cs.current_token.type == DIV){
        match(DIV);
    }else if(cs.current_token.type == MULT){
        match(MULT);
    }else{
        printf("Erro de sintaxe: operador de alta precedencia inválido\n");
        exit(1);
    }
}

ASTNode* unaryexpr(){
    printf("unaryexpr\n");
    if(cs.current_token.type == PLUS || cs.current_token.type == MINUS){
        operator();
        factor();
    }else{
        factor();
    }
}

ASTNode* factor(){
    printf("factor\n");
    if(cs.current_token.type == INT_CONST){
        match(INT_CONST);
    }else if(cs.current_token.type == FLOAT_CONST){
        match(FLOAT_CONST);
    }else if(cs.current_token.type == STRING_CONST){
        match(STRING_CONST);
    }else if(cs.current_token.type == IDENT){
        lvalue();
    }else if(cs.current_token.type == LPAREN){
        match(LPAREN);
        numexpression();
        match(RPAREN);
    }else{
        printf("Erro de sintaxe: factor esperado\n");
        exit(1);
    }
}

ASTNode* lvalue(){
    printf("lvalue\n");
    if(cs.current_token.type == IDENT){
        match(IDENT);
        lvalueaux();
    }else{
        printf("Erro de sintaxe: identificador esperado\n");
        exit(1);
    }
}

ASTNode* lvalueaux(){
    printf("lvalueaux\n");
    if(cs.current_token.type == LBRACK){
        match(LBRACK);
        numexpression();
        match(RBRACK);
        lvalueaux();
    }
}