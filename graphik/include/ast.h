#ifndef __AST_H__
#define __AST_H__

#define AST_AMOUNT_INIT 10
#define REALLOC_AST 4

#include "my_string.h"

typedef long long int lint;

struct tagStm;

typedef enum {
	COND_STM,
	RET_STM,
	BREAK_STM,
	PLOT_STM,
	ASSIGN_STM
} stm_type;

typedef enum {
	ADD_OP,
	SUB_OP,
	MULT_OP,
	DIV_OP,
	NEG_OP,
	OR_OP,
	AND_OP,
	LT_OP,
	GT_OP,
	LEQ_OP,
	GEQ_OP,
	EQ_OP,
	NEQ_OP
} oper;

typedef struct exnode {
	enum {OPERATION, VALUE_T, VARIABLE_T, FUNCCALL} expr_type;
	enum {STATIC, DYNAMIC} resolution_type;

	lint value;
	String var_ident;

	int operation;

	struct exnode *lBranch, *rBranch, *func_args;
	size_t arg_num;
} expr_node;

typedef struct {
	String var_ident;
	expr_node value;
} assign_node, def_node;

typedef struct {
	expr_node value1, value2;
} plot_node;

typedef struct {
	expr_node ret_val;
} ret_node;

typedef struct {
	int s1, s2;
	//...//
} break_node;

typedef struct tagBlock {
	struct tagStm *stms;
	size_t stm_num;
} block_node;

typedef struct {
	enum {WHILE_T, IF_T} cond_type;
	expr_node condition;
	block_node scope;
} cond_node;

typedef struct tagStm {
	stm_type	type;

	cond_node	cond_stm;
	break_node 	brk_stm;
	assign_node 	asg;
	plot_node	plot;
	ret_node	ret;
} statement_node;

typedef struct {
	String ident;
	assign_node *args;
	block_node scope;

	size_t arg_num, stack_sz;
} function_node;

typedef struct {
	function_node *funcs;
	size_t sz;
} program_node;

#endif
