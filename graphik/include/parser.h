#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>

#include "lexer.h"
#include "ast.h"
#include "parse_util.h"

#define MAX_STR 100

//To be relocated to 'x64 assembly generation'

//////////PARSER////////////
program_node *parse_program(TokenArray);

void program(program_node*);
void function_decl(function_node*);
void block(block_node*);
void statement(statement_node*);

//Statements
void assign(assign_node*, stm_type*);
void if_stm(cond_node*, stm_type*);
void while_stm(cond_node*, stm_type*);
void break_stm(stm_type*);
void plot(plot_node*, stm_type*);
void ret(ret_node*, stm_type*);

//Expressions
void invoke(expr_node*);
void expr(expr_node*);
void term(expr_node*);
void factor(expr_node*);
void negate(expr_node*);
void value(expr_node*);
//////////PARSER////////////

#endif
