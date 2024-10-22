#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h" 
#include "types.h"
#include "compiler_state.h"

void advance();

void match(TokenType expected_type);

ASTNode* program();

ASTNode* funclist();

ASTNode* funcdef();

ASTNode* paramlist();

ASTNode* optional_paramlist();

ASTNode* type();

ASTNode* statement();

ASTNode* vardecl();

ASTNode* int_const();

ASTNode* atribstat();

ASTNode* stat();

ASTNode* funccall();

ASTNode* paramlistcall();

ASTNode* optional_paramlistcall();

ASTNode* returnstat();

ASTNode* optional_ident();

ASTNode* ifstat();

ASTNode* optional_else();

ASTNode* forstat();

ASTNode* statelist();

ASTNode* optional_statelist();

ASTNode* expression();

ASTNode* optional_comparator();

ASTNode* least_precedence_comparator();

ASTNode* comparator();

ASTNode* numexpression();

ASTNode* numexpressionaux();

ASTNode* operator();

ASTNode* term();

ASTNode* term_aux();

ASTNode* high_precedence_operator();

ASTNode* unaryexpr();

ASTNode* factor();

ASTNode* lvalue();

ASTNode* lvalueaux();

#endif // PARSER_H

