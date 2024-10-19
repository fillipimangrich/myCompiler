#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h" 
#include "types.h"
#include "compiler_state.h"

void advance();

void match(TokenType expected_type);

void program();

void funclist();

void funcdef();

void paramlist();

void optional_paramlist();

void type();

void statement();

void vardecl();

void int_const();

void atribstat();

void stat();

void funccall();

void paramlistcall();

void optional_paramlistcall();

void returnstat();

void optional_ident();

void ifstat();

void optional_else();

void forstat();

void statelist();

void optional_statelist();

void expression();

void optional_comparator();

void least_precedence_comparator();

void comparator();

void numexpression();

void numexpressionaux();

void operator();

void term();

void term_aux();

void high_precedence_operator();

void unaryexpr();

void factor();

void lvalue();

void lvalueaux();

#endif // PARSER_H

