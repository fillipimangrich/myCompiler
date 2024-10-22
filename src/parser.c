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

void program() {
    printf("program\n");
    if (cs.current_token.type == DEF) {
        funclist();
    } else {
        statement();
    }
}

void funclist() {
    printf("funclist\n");
    if (cs.current_token.type == DEF) {
        funcdef();
        funclist(); 
    }
}

void funcdef() {
    printf("funcdef\n");
    match(DEF);
    type();  
    match(FUNC_IDENT);  
    match(LPAREN);  
    paramlist();  
    match(RPAREN);  
    match(LBRACE);  
    statelist();  
    match(RBRACE); 
}

void paramlist() {
    printf("paramlist\n");
    if (cs.current_token.type == INT || cs.current_token.type == FLOAT || cs.current_token.type == STRING) {
        type(); 
        match(IDENT); 
        optional_paramlist();  
    }
}

void optional_paramlist() {
    printf("optional_paramlist\n");
    if (cs.current_token.type == COMMA) {
        match(COMMA);
        type();
        match(IDENT);
        optional_paramlist();
    }
}

void type() {
    printf("type\n");
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
}

void statement() {
    printf("statement\n");
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
}

void vardecl(){
    printf("vardecl\n");
    type();
    match(IDENT);
    int_const();
}

void int_const(){
    printf("int_const\n");
    if(cs.current_token.type == LBRACK){
        match(LBRACK);
        match(INT_CONST);
        match(RBRACK);
        int_const();
    }
}

void atribstat(){
    printf("atribstat\n");
    lvalue();
    match(ASSIGN);
    stat();
}

void stat(){
    printf("stat\n");
    if(cs.current_token.type == FUNC_IDENT){
        funccall();
    }else{
        expression();
    }
}

void funccall(){
    printf("funccall\n");
    match(FUNC_IDENT);
    match(LPAREN);
    paramlistcall();
    match(RPAREN);
}

void paramlistcall(){
    printf("paramlistcall\n");
    if(cs.current_token.type == IDENT){
        match(IDENT);
        optional_paramlistcall();
    }
}

void optional_paramlistcall(){
    printf("optional_paramlistcall\n");
    match(COMMA);
    match(IDENT);
    optional_paramlistcall();
}

void returnstat(){
    printf("returnstat\n");
    match(RETURN);
    optional_ident();
}

void optional_ident(){
    printf("optional_ident\n");
    if(cs.current_token.type == IDENT){
        match(IDENT);
    }
}

void ifstat(){
    printf("ifstat\n");
    match(IF);
    match(LPAREN);
    expression();
    match(RPAREN);
    statement();
    optional_else();
}

void optional_else(){
    printf("optional_else\n");
    if(cs.current_token.type == ELSE){
        match(ELSE);
        statement();
    }
}

void forstat(){
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

void statelist(){
    printf("statelist\n");
    statement();
    optional_statelist();
}

void optional_statelist(){
    printf("optional_statelist\n");
    if(cs.current_token.type == RBRACE){
        return;   
    }else{
        statelist();
    }
}

void expression(){
    printf("expression\n");
    if(cs.current_token.type == NOT){
        match(NOT);
        expression();
    }else{
        numexpression();
        optional_comparator();
    }
}

void optional_comparator(){
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

void least_precedence_comparator(){
    printf("least_precedence_comparator\n");
    match(OR);
}

void comparator(){
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

void numexpression(){
    printf("numexpression\n");
    term();
    numexpressionaux();
}

void numexpressionaux(){
    printf("numexpressionaux\n");
    if(cs.current_token.type == PLUS || cs.current_token.type == MINUS){
        operator();
        term();
        numexpressionaux();
    }
}

void operator(){
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

void term(){
    printf("term\n");
    unaryexpr();
    term_aux();
}

void term_aux(){
    printf("term_Aux\n");
    if(cs.current_token.type == MOD || cs.current_token.type == DIV || 
    cs.current_token.type == MULT){
        high_precedence_operator();
        unaryexpr();
        term_aux();
    }
}

void high_precedence_operator(){
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

void unaryexpr(){
    printf("unaryexpr\n");
    if(cs.current_token.type == PLUS || cs.current_token.type == MINUS){
        operator();
        factor();
    }else{
        factor();
    }
}

void factor(){
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

void lvalue(){
    printf("lvalue\n");
    if(cs.current_token.type == IDENT){
        match(IDENT);
        lvalueaux();
    }else{
        printf("Erro de sintaxe: identificador esperado\n");
        exit(1);
    }
}

void lvalueaux(){
    printf("lvalueaux\n");
    if(cs.current_token.type == LBRACK){
        match(LBRACK);
        numexpression();
        match(RBRACK);
        lvalueaux();
    }
}