#include "../include/gen_x64.h"

String curr_scope;
bool rax, rcx, rdx;

int if_stack[20000], while_stack[20000];
size_t if_ptr, wh_ptr, stack_size;

char *logops[] = {
	"setl", "setg", 
	"setle", "setge",
	"sete", "setne",
};

bool is_value(expr_node *ep) {
	if(ep && ep->expr_type == VALUE_T) return true;
	return false;
}

void push_stack(int a, bool is_if) { 
	if(is_if) if_stack[if_ptr++] = a; 
	else while_stack[wh_ptr++] = a;
}

int pop_stack(bool is_if) {
	int to_ret = is_if ? if_stack[--if_ptr] : while_stack[--wh_ptr];

	if(is_if) if_stack[if_ptr] = 0;
	else while_stack[wh_ptr] = 0;

	return to_ret;
}

expr_node *optimize_expr(expr_node *tree) {

	if(!tree) return NULL;

	if(tree->lBranch) tree->lBranch = optimize_expr(tree->lBranch);
	if(tree->rBranch) tree->rBranch = optimize_expr(tree->rBranch);

	switch(tree->expr_type) {
	case OPERATION:
		switch(tree->operation) {
		case ADD_OP:
			if(tree->rBranch && tree->rBranch->expr_type == VALUE_T && !tree->rBranch->value)
				tree = tree->lBranch;
			else if(tree->lBranch && tree->lBranch->expr_type == VALUE_T && !tree->lBranch->value) 
				tree = tree->rBranch;

			break;
		case MULT_OP:
			if(tree->rBranch && tree->rBranch->expr_type == VALUE_T && tree->rBranch->value == 1)
				tree = tree->lBranch;
			else if(tree->lBranch && tree->lBranch->expr_type == VALUE_T && tree->lBranch->value == 1)
				tree = tree->rBranch;

			break;
		}
		break;
	}

	return tree;
}

void emit_and_or(FILE *out, int oper, char *reg) {
	static int label = 0;

	switch(oper) {
	case OR_OP:
		emit(out, "cmp rax, 0");
		emit(out, "jne or%d", label++);
		emit(out, "cmp %s, 0", reg);
		emit(out, "je or%d", label);
		

		emit(out, "or%d:", label - 1);
		emit(out, "mov rax, 1");
		emit(out, "jmp rest%d", label - 1);
		emit(out, "or%d:", label);
		emit(out, "mov rax, 0");
		emit(out, "rest%d:", label - 1);
		break;
	case AND_OP:
		emit(out, "cmp rax, 0");
		emit(out, "je add%d", label);
		emit(out, "cmp %s, 0", reg);
		emit(out, "je add%d", label);
		
		emit(out, "mov rax, 1");
		emit(out, "jmp rest%d", label);
		emit(out, "add%d:", label);
		emit(out, "mov rax, 0");
		emit(out, "rest%d:", label);
		break;
	}
}

lint trsl_static_expr(expr_node *tree) {
	if(!tree) return 0;

	switch(tree->expr_type) {
	case OPERATION:
		switch(tree->operation) {
		case ADD_OP: return trsl_static_expr(tree->lBranch) + trsl_static_expr(tree->rBranch);
		case MULT_OP: return trsl_static_expr(tree->lBranch) * trsl_static_expr(tree->rBranch);
		case DIV_OP: return trsl_static_expr(tree->lBranch) / trsl_static_expr(tree->rBranch);
		case NEG_OP: return -trsl_static_expr(tree->lBranch);
		case OR_OP: return trsl_static_expr(tree->lBranch) || trsl_static_expr(tree->rBranch);
		case AND_OP: return trsl_static_expr(tree->lBranch) && trsl_static_expr(tree->rBranch);
		case GEQ_OP: return trsl_static_expr(tree->lBranch) >= trsl_static_expr(tree->rBranch);
		case LEQ_OP: return trsl_static_expr(tree->lBranch) <= trsl_static_expr(tree->rBranch);
		case LT_OP: return trsl_static_expr(tree->lBranch) < trsl_static_expr(tree->rBranch);
		case GT_OP: return trsl_static_expr(tree->lBranch) > trsl_static_expr(tree->rBranch);
		case EQ_OP: return trsl_static_expr(tree->lBranch) == trsl_static_expr(tree->rBranch);
		case NEQ_OP: return trsl_static_expr(tree->lBranch) != trsl_static_expr(tree->rBranch);
		}
		break;
	case VALUE_T:
		return tree->value;
		break;
	}
}

void trsl_dynamic_expr(FILE *out, expr_node *tree, map sym_table) {
	if(!tree) return;

	bool lbr = false, rbr = false;
	var symb;

	switch(tree->expr_type) {
	case OPERATION:
		switch(tree->operation) {
		case ADD_OP:
			lbr = is_value(tree->lBranch);
			rbr = is_value(tree->rBranch);

			if(lbr) trsl_dynamic_expr(out, tree->rBranch, sym_table);
			else trsl_dynamic_expr(out, tree->lBranch, sym_table);

			if(!lbr && !rbr && !rdx) {
				emit(out, "mov rdx, rax");

				rdx = true;
				rax = rcx = false;
			}


			if(lbr) trsl_dynamic_expr(out, tree->lBranch, sym_table);
			else trsl_dynamic_expr(out, tree->rBranch, sym_table);
	
			if(rdx && !lbr && !rbr) {
				emit(out, "add rax, rdx");
				rdx = false;
			} else {
				emit(out, "add rax, rcx");
				rcx = false;
			}

			break;
		case MULT_OP:
			lbr = is_value(tree->lBranch);
			rbr = is_value(tree->rBranch);

			if(lbr) trsl_dynamic_expr(out, tree->rBranch, sym_table);
			else trsl_dynamic_expr(out, tree->lBranch, sym_table);

			if(!lbr && !rbr && !rdx) {
				emit(out, "mov rdx, rax");

				rdx = true;
				rax = rcx = false;
			}

			if(lbr) trsl_dynamic_expr(out, tree->lBranch, sym_table);
			else trsl_dynamic_expr(out, tree->rBranch, sym_table);

			if(rdx && !lbr && !rbr) {
				emit(out, "imul rdx");
				rdx = false;
			} else {
				emit(out, "imul rcx");
				rcx = false;
			}

			break;
		case DIV_OP:
			lbr = is_value(tree->lBranch);
			rbr = is_value(tree->rBranch);

			if(rbr) trsl_dynamic_expr(out, tree->lBranch, sym_table);
			else trsl_dynamic_expr(out, tree->rBranch, sym_table);

			if((!lbr || !rbr) && !rdx) {
				emit(out, "mov rdx, rax");

				rdx = true;
				rax = rcx = false;
			}

			if(rbr) trsl_dynamic_expr(out, tree->rBranch, sym_table);
			else trsl_dynamic_expr(out, tree->lBranch, sym_table);

			if(rdx && (!lbr || !rbr)) {
				emit(out, "idiv rdx");
				rdx = false;
			} else {
				emit(out, "idiv rcx");
				rcx = false;
			}

			break;
		case NEG_OP:
			lbr = is_value(tree->lBranch);
			trsl_dynamic_expr(out, tree->lBranch, sym_table);
			
			if(rax && !rcx) emit(out, "neg rax");
			else if(rax && rcx) emit(out, "neg rcx");
			else if(!lbr) emit(out, "neg rdx");
			
			break;
		case AND_OP:
		case OR_OP:
			lbr = is_value(tree->lBranch);
			rbr = is_value(tree->rBranch);

			if(lbr) trsl_dynamic_expr(out, tree->rBranch, sym_table);
			else trsl_dynamic_expr(out, tree->lBranch, sym_table);

			if(!lbr && !rbr && !rdx) {
				emit(out, "mov rdx, rax");

				rdx = true;
				rax = rcx = false;
			}


			if(lbr) trsl_dynamic_expr(out, tree->lBranch, sym_table);
			else trsl_dynamic_expr(out, tree->rBranch, sym_table);
	
			if(rdx && !lbr && !rbr) {
				emit_and_or(out, tree->operation, "rdx");
				rdx = false;
			} else {
				emit_and_or(out, tree->operation, "rcx");
				rcx = false;
			}
			break;
		case EQ_OP:
		case NEQ_OP:
		case GEQ_OP:
		case LEQ_OP:
		case GT_OP:
		case LT_OP:
			lbr = is_value(tree->lBranch);
			rbr = is_value(tree->rBranch);

			if(lbr) trsl_dynamic_expr(out, tree->rBranch, sym_table);
			else trsl_dynamic_expr(out, tree->lBranch, sym_table);

			if(!lbr && !rbr && !rdx) {
				emit(out, "mov rdx, rax");

				rdx = true;
				rax = rcx = false;
			}


			if(lbr) trsl_dynamic_expr(out, tree->lBranch, sym_table);
			else trsl_dynamic_expr(out, tree->rBranch, sym_table);
	
			if(rdx && !rcx && !lbr && !rbr) {
				emit(out, "cmp rdx, rax");
				emit(out, "%s al", logops[tree->operation - LT_OP]);
				emit(out, "movzx rax, al");
				rdx = false;
			} else {
				emit(out, "cmp rax, rcx");
				emit(out, "%s al", logops[tree->operation - LT_OP]);
				emit(out, "movzx rax, al");
				rcx = false;
			}

			break;
		}

		break;
	case VALUE_T:
		if(!rax) {
			emit(out, "mov rax, %lli", tree->value);
			rax = true;
		} else if(!rcx) {
			emit(out, "mov rcx, %lli", tree->value);
			rcx = true;
		}
		break;
	case VARIABLE_T:
		symb = take_from_scope(sym_table, tree->var_ident, curr_scope.ptr);
		char ptr_oper = symb.symbol_type == ARGUMENT ? '+' : '-';

		printf("%s - %zu\n", symb.ident.ptr, symb.num);

		if(!rax) {
			emit(out, "mov rax, [rbp %c %zu]", ptr_oper, symb.num);
			rax = true;
		} else if(!rcx) {
			emit(out, "mov rcx, [rbp %c %zu]", ptr_oper, symb.num);
			rcx = true;
		}

		break;
	case FUNCCALL:
		symb = take_from_scope(sym_table, tree->var_ident, "global");
		
		if(rax && !rcx) {
			emit(out, "mov rcx, rax");
			rcx = true;
			rax = false;
		}

		for(int i = (int)tree->arg_num - 1; i >= 0; i--) {
			if(tree->func_args[i].resolution_type == DYNAMIC) {
				trsl_dynamic_expr(out, optimize_expr(&tree->func_args[i]), sym_table);

				if(rax && !rcx) {
					emit(out, "push rax");
					rax = false;
				}
				else if(rax && rcx) {
					emit(out, "push rcx");
					rcx = false;
				}

			} else {
				lint res = trsl_static_expr(optimize_expr(&tree->func_args[i]));
				emit(out, "push %lli", res);
			}
		}

		emit(out, "call %s", symb.ident.ptr);
		rax = true;
		break;
	}

}

void trsl_assign(FILE *out, assign_node *asg, map sym_table) {
	var symb = take_from_scope(sym_table, asg->var_ident, curr_scope.ptr);

	if(asg->value.resolution_type == DYNAMIC) {
		trsl_dynamic_expr(out, optimize_expr(&asg->value), sym_table);
		emit(out, "mov [rbp - %zu], rax", symb.num);
		rax = false;
	} else {
		lint res = trsl_static_expr(optimize_expr(&asg->value));
		emit(out, "mov rax, %lli", res);
		emit(out, "mov [rbp - %zu], rax", symb.num);
		rax = false;
	}
		
}

void trsl_plot(FILE *out, plot_node *plt, map sym_table) {
	if(plt->value1.resolution_type == DYNAMIC) {
		trsl_dynamic_expr(out, optimize_expr(&plt->value1), sym_table);
		emit(out, "mov rdx, rax");
		rax = false;
	} else {
		lint res = trsl_static_expr(optimize_expr(&plt->value1));
		emit(out, "mov rdx, %lli", res);
	}

	if(plt->value2.resolution_type == DYNAMIC) {
		trsl_dynamic_expr(out, optimize_expr(&plt->value2), sym_table);
		emit(out, "mov r8, rax");
		rax = false;
	} else {
		lint res = trsl_static_expr(optimize_expr(&plt->value2));
		emit(out, "mov r8, %lli", res);
	}

	emit(out, "lea rcx, [fmt]");
	emit(out, "call printf");
	rcx = false;
}

void trsl_ret(FILE *out, ret_node *rt, map sym_table) {
	if(rt->ret_val.resolution_type == DYNAMIC) {
		trsl_dynamic_expr(out, optimize_expr(&rt->ret_val), sym_table);
	} else {
		lint res = trsl_static_expr(optimize_expr(&rt->ret_val));
		emit(out, "mov rax, %lli", res);
	}


	if(!strcmp(curr_scope.ptr, "main_")) {
		//Exit routine//
		emit(out, "add rsp, %lli", stack_size + STACK_ALIGN);
		emit(out, "pop rbp");
		emit(out, "push rax");
		emit(out, "call exit");
		//Exit routine//
	} else {
		char *func_sc = substring(curr_scope.ptr, '_');
		String func_ident = make_string(func_sc);
		string_pop(&func_ident);

		var symb = take_from_scope(sym_table, func_ident, "global");

		emit(out, "add rsp, %lli", stack_size * 2);
		emit(out, "pop rbp");
		emit(out, "ret %zu", symb.arg_num * VAR_SIZE);

		delete_string(&func_ident);
		free(func_sc);
	}
}

void trsl_cond(FILE *out, cond_node *cnd, map sym_table) {
	static int occs_i = 0, occs_w = 0;
	int pos = 0;

	switch(cnd->cond_type) {
	case WHILE_T:
		emit(out, "wl%d:", occs_w);

		push_stack(occs_w, true);
		occs_w++;

		trsl_block(out, &cnd->scope, sym_table);
		
		if(cnd->condition.resolution_type == DYNAMIC) {
			trsl_dynamic_expr(out, optimize_expr(&cnd->condition), sym_table);
		} else {
			lint res = trsl_static_expr(optimize_expr(&cnd->condition));
			emit(out, "mov rax, %lli", res);
		}

		pos = pop_stack(true);

		emit(out, "cmp rax, 1");
		emit(out, "je wl%d", pos);
		emit(out, "lw%d:", pos);

		rax = false;
		break;
	case IF_T:
		if(cnd->condition.resolution_type == DYNAMIC) {
			trsl_dynamic_expr(out, optimize_expr(&cnd->condition), sym_table);
		} else {
			lint res = trsl_static_expr(optimize_expr(&cnd->condition));
			emit(out, "mov rax, %lli", res);
		}

		emit(out, "cmp rax, 1");
		emit(out, "jne li%d", occs_i);
		rax = false;

		push_stack(occs_i, false);
		occs_i++;

		trsl_block(out, &cnd->scope, sym_table);

		emit(out, "li%d:", pop_stack(false));
		break;
	}
}

void trsl_break(FILE *out) {
	emit(out, "jmp lw%d", while_stack[wh_ptr - 1]);
}

void trsl_statement(FILE *out, statement_node *stm, map sym_table) {
	switch(stm->type) {
	case COND_STM: trsl_cond(out, &stm->cond_stm, sym_table); break;
	case RET_STM: trsl_ret(out, &stm->ret, sym_table); break;
	case BREAK_STM: trsl_break(out); break;
	case PLOT_STM: trsl_plot(out, &stm->plot, sym_table); break;
	case ASSIGN_STM: trsl_assign(out, &stm->asg, sym_table); break;
	}
}

void trsl_block(FILE *out, block_node *blc, map sym_table) {
	for(size_t i = 0; i < blc->stm_num; i++)
		trsl_statement(out, &blc->stms[i], sym_table);
}

void trsl_function(FILE *out, function_node *fn, map sym_table) {
	curr_scope = make_string(fn->ident.ptr);
	string_push_back(&curr_scope, '_');

	bool is_main = !strcmp(curr_scope.ptr, "main_");

	emit(out, "%s:", fn->ident.ptr);
	emit(out, "push rbp");
	emit(out, "mov rbp, rsp");
	
	stack_size = fn->stack_sz * 2;
	emit(out, "sub rsp, %lli", is_main ? STACK_ALIGN + stack_size : stack_size * 2);

	trsl_block(out, &fn->scope, sym_table);

	delete_string(&curr_scope);
}

void trsl_program(FILE *out, program_node *prog, map sym_table) {
	for(size_t i = 0; i < prog->sz; i++)
		trsl_function(out, &prog->funcs[i], sym_table);
}

void generate_asm(FILE *out, program_node *ast, analysis_res res) {
	fprintf(out, "global ");
	for(size_t i = 0; i < ast->sz; i++) {
		fprintf(out, "%s", ast->funcs[i].ident.ptr);
		if(i != ast->sz - 1) fprintf(out, ", ");
	}

	fputc('\n', out);

	stack_size = if_ptr = wh_ptr = 0;
	rax = rcx = rdx = false;

	STARTUP_PROC;
	trsl_program(out, ast, res.sym_table);
}
