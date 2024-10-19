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
    if (cs.current_token.type == DEF) {
        funclist();
    } else {
        statement();
    }
}

void funclist() {
    if (cs.current_token.type == DEF) {
        funcdef();
        funclist(); 
    }
}

void funcdef() {
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
    if (cs.current_token.type == INT || cs.current_token.type == FLOAT || cs.current_token.type == STRING) {
        type(); 
        match(IDENT); 
        optional_paramlist();  
    }
}

void optional_paramlist() {
    if (cs.current_token.type == COMMA) {
        match(COMMA);
        type();
        match(IDENT);
        optional_paramlist();
    }
}

void type() {
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
    type();
    match(IDENT);
    int_const();
}

void int_const(){
    if(cs.current_token.type == LBRACK){
        match(LBRACK);
        match(INT_CONST);
        match(RBRACK);
        int_const();
    }
}

void atribstat(){
    lvalue();
    match(ASSIGN);
    stat();
}

void stat(){
    if(cs.current_token.type == FUNC_IDENT){
        funccall();
    }else{
        expression();
    }
}

void funccall(){
    match(FUNC_IDENT);
    match(LPAREN);
    paramlistcall();
    match(RPAREN);
}

void paramlistcall(){
    if(cs.current_token.type == IDENT){
        match(IDENT);
        optional_paramlistcall();
    }
}

void optional_paramlistcall(){
    match(COMMA);
    match(IDENT);
    optional_paramlistcall();
}

void returnstat(){
    match(RETURN);
    optional_ident();
}

void optional_ident(){
    if(cs.current_token.type == IDENT){
        match(IDENT);
    }
}

void ifstat(){
    match(IF);
    match(LPAREN);
    expression();
    match(RPAREN);
    statement();
    optional_else();
}

void optional_else(){
    if(cs.current_token.type == ELSE){
        match(ELSE);
        statement();
    }
}

void forstat(){
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
    statement();
    optional_statelist();
}

void optional_statelist(){
    if(cs.current_token.type == RBRACE){
        return;   
    }else{
        statelist();
    }
}

void expression(){
    if(cs.current_token.type == NOT){
        match(NOT);
        expression();
    }else{
        numexpression();
        optional_comparator();
    }
}

void optional_comparator(){
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
    match(OR);
}

void comparator(){
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
    }
}

void numexpression(){
    term();
    numexpressionaux();
}

void numexpressionaux(){
    if(cs.current_token.type == PLUS || cs.current_token.type == MINUS){
        operator();
        term();
        numexpressionaux();
    }
}

void operator(){
    if(cs.current_token.type == PLUS){
        match(PLUS);
    }else if(cs.current_token.type == MINUS){
        match(MINUS);
    }
}

void term(){
    unaryexpr();
    term_aux();
}

void term_aux(){
    if(cs.current_token.type == MOD || cs.current_token.type == DIV || 
    cs.current_token.type == MULT){
        high_precedence_operator();
        unaryexpr();
        term_aux();
    }
}

void high_precedence_operator(){
    if(cs.current_token.type == MOD){
        match(MOD);
    }else if(cs.current_token.type == DIV){
        match(DIV);
    }else if(cs.current_token.type == MULT){
        match(MULT);
    }
}

void unaryexpr(){
    if(cs.current_token.type == PLUS || cs.current_token.type == MINUS){
        operator();
        factor();
    }else{
        factor();
    }
}

void factor(){
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
    }
}

void lvalue(){
    if(cs.current_token.type == IDENT){
        match(IDENT);
        lvalueaux();
    }
}

void lvalueaux(){
    if(cs.current_token.type == LBRACK){
        match(LBRACK);
        numexpression();
        match(RBRACK);
        lvalueaux();
    }
}