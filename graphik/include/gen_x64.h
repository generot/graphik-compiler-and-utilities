#ifndef __GEN_X64__
#define __GEN_X64__

#include "semantics.h"

#define STACK_ALIGN 32

#define STARTUP_PROC {								\
	emit(out, "extern printf, exit");					\
	emit(out, "section .data\nfmt db \"%%d %%d\", 10, 0");			\
	emit(out, "section .text");						\
}

typedef enum {false, true} bool;

bool is_value(expr_node*);
expr_node *optimize_expr(expr_node*);

void emit_and_or(FILE*, int, char*);

lint trsl_static_expr(expr_node*);
void trsl_dynamic_expr(FILE*, expr_node*, map);
void trsl_assign(FILE*, assign_node*, map);
void trsl_plot(FILE*, plot_node*, map);
void trsl_cond(FILE*, cond_node*, map);
void trsl_ret(FILE*, ret_node*, map);
void trsl_statement(FILE*, statement_node*, map);
void trsl_block(FILE*, block_node*, map);
void trsl_function(FILE*, function_node*, map);
void trsl_program(FILE*, program_node*, map);
void trsl_break(FILE*);

void generate_asm(FILE*, program_node*, analysis_res);

#endif
