#include "../include/parser.h"

Token *tokens;
size_t len, ix;

void program(program_node *prog) {
	prog->funcs = malloc(AST_AMOUNT_INIT * sizeof(function_node));
	size_t func_ix = 0, ptr_size = AST_AMOUNT_INIT;

	//puts("In program...");
	while(ix < len-1) {
		if(func_ix >= ptr_size) {
			ptr_size += REALLOC_AST;
			prog->funcs = realloc(prog->funcs, ptr_size * sizeof(function_node));
		}

		function_decl(&prog->funcs[func_ix++]);
	}

	prog->sz = func_ix;
}

void function_decl(function_node *func) {
	size_t args_ix = 0, ptr_size = AST_AMOUNT_INIT;
	func->args = malloc(AST_AMOUNT_INIT * sizeof(assign_node));

	//puts("In function declaration...");
	if(ix < len && tokens[ix].kword == DECLARE) {
		ix++;
		
		if(tokens[ix].type == IDENT) 
			func->ident = copy_string(tokens[ix].ident);
		else issue_error("expected function identifier.");

		ix++;

		if(tokens[ix].aop == LPAR) {
			ix++;
			while(tokens[ix].aop != RPAR) {
				if(ix == len - 1)
					issue_error("unmatched parentheses for function '%s'.", func->ident.ptr);

				if(args_ix >= ptr_size) {
					ptr_size += REALLOC_AST;
					func->args = realloc(func->args, ptr_size * sizeof(def_node));
				}

				if(tokens[ix].kword != NONE) {
					func->args[args_ix++].var_ident = copy_string(tokens[ix].ident);
				}/* else {
					free(func->args);
					func->args = NULL;
				}*/

				ix++;
			}
			func->arg_num = args_ix;
		} else issue_error("expected parentheses for function '%s'", func->ident.ptr);

		func->stack_sz = 0;

		ix++;
		block(&func->scope);
		ix++;
		
	} else issue_error("expected function declaration.");
}

void block(block_node *block) {
	block->stms = malloc(AST_AMOUNT_INIT * 5 * sizeof(statement_node));
	size_t block_ix = 0, ptr_size = AST_AMOUNT_INIT * 5;

	//puts("In block...");
	if(tokens[ix].kword == BEGIN) {
		ix++;
		while(tokens[ix].kword != END) {
			/*if(ix == len - 1 || expect_symbol(DECLARE))
				issue_error("expected end of block.");*/

			if(block_ix >= ptr_size) {
				ptr_size += REALLOC_AST;
				block->stms = realloc(block->stms, ptr_size * sizeof(statement_node));
			}
			
			statement(&block->stms[block_ix++]);
		}
		block->stm_num = block_ix;
	} else issue_error("expected block opening.");
}

void statement(statement_node *stm) {
	//puts("In statement...");

	if(tokens[ix].type == IDENT) {
		assign(&stm->asg, &stm->type);
		return;
	}
	
	switch(tokens[ix].kword) {
	case IF:
		if_stm(&stm->cond_stm, &stm->type);
		break;
	case WHILE:
		while_stm(&stm->cond_stm, &stm->type);
		break;
	case PLOT:
		plot(&stm->plot, &stm->type);
		break;
	case RET:
		ret(&stm->ret, &stm->type);
		break;
	case BREAK:
		break_stm(&stm->type);
		break;
	}
}

void if_stm(cond_node *cond, stm_type *tp) {
	*tp = COND_STM;

	//puts("In if...");
	if(tokens[ix].kword == IF) {
		cond->cond_type = IF_T;

		ix++;
		expr(&cond->condition);
		//ix++;

		block(&cond->scope);
		ix++;
	}
}

void while_stm(cond_node *cond, stm_type *tp) {
	*tp = COND_STM;

	//puts("In while...");
	if(tokens[ix].kword == WHILE) {
		cond->cond_type = WHILE_T;

		ix++;
		expr(&cond->condition);
		//ix++;

		block(&cond->scope);
		ix++;
	}
}

void break_stm(stm_type *tp) {
	//puts("In break...");
	if(tokens[ix].kword == BREAK) {
		*tp = BREAK_STM;
		ix++;
	}
}

void plot(plot_node *pl, stm_type *tp) {
	//puts("In plot...");
	if(tokens[ix].kword == PLOT) {
		*tp = PLOT_STM;
		ix++;

		expr(&pl->value1);

		//ix++;
		expr(&pl->value2);
	}
	return;
}

void ret(ret_node *rt, stm_type *tp) {
	//puts("In ret...");
	if(tokens[ix].kword == RET) {
		*tp = RET_STM;

		ix++;
		expr(&rt->ret_val);
	}

	return;
}

void assign(assign_node *asg, stm_type *tp) {
	//puts("In assign...");
	if(tokens[ix].type == IDENT) {
		asg->var_ident = tokens[ix++].ident;
		if(tokens[ix].lop == ASG) {
			*tp = ASSIGN_STM;

			ix++;
			expr(&asg->value);

		}
	}
	return;
}

void invoke(expr_node *func) {
	//puts("In invoke...");
	func->expr_type = FUNCCALL;
	func->resolution_type = DYNAMIC;

	func->lBranch = func->rBranch = NULL;

	func->func_args = malloc(AST_AMOUNT_INIT * sizeof(expr_node));
	size_t args_ix = 0, args_sz = AST_AMOUNT_INIT;

	if(tokens[ix].type == IDENT) func->var_ident = copy_string(tokens[ix].ident);
	else issue_error("expected identifier after 'invoke'");
	
	ix++;
	if(tokens[ix].aop == LPAR) {
		ix++;

		//if(expect_symbol(RPAR)) free(func->func_args);

		while(tokens[ix].aop != RPAR) {
			if(args_ix >= args_sz)
				func->func_args = realloc(
					func->func_args, 
					(args_sz += REALLOC_AST) * sizeof(expr_node)
				);

			expr(&func->func_args[args_ix++]);
		}
		func->arg_num = args_ix;
	} else issue_error("expected parentheses after identifier");
}

void expr(expr_node *tree) {
	tree->expr_type = OPERATION;
	tree->resolution_type = STATIC;

	//puts("In expr...");
	tree->lBranch = calloc(1, sizeof(expr_node));
	tree->rBranch = calloc(1, sizeof(expr_node));

	term(tree->lBranch);
	tree->operation = ADD_OP;
	tree->rBranch->expr_type = VALUE_T;
	tree->rBranch->value = 0;

	while(tokens[ix].lop == OR || tokens[ix].lop == AND
	     || tokens[ix].lop == GEQ || tokens[ix].lop == LEQ
	     || tokens[ix].lop == EQ || tokens[ix].lop == NEQ
	     || tokens[ix].lop == LT || tokens[ix].lop == GT)
	{
		switch(tokens[ix].lop) {
		case OR: tree->operation = OR_OP; break;
		case AND: tree->operation = AND_OP; break;
		case EQ: tree->operation = EQ_OP; break;
		case NEQ: tree->operation = NEQ_OP; break;
		case GEQ: tree->operation = GEQ_OP; break;
		case LEQ: tree->operation = LEQ_OP; break;
		case LT: tree->operation = LT_OP; break;
		case GT: tree->operation = GT_OP; break;
		}
	
		ix++;
		expr(tree->rBranch);
	}
}

void term(expr_node *tree) {
	tree->expr_type = OPERATION;
	tree->resolution_type = STATIC;

	//puts("In term...");
	tree->lBranch = calloc(1, sizeof(expr_node));
	tree->rBranch = calloc(1, sizeof(expr_node));

	factor(tree->lBranch);
	tree->operation = ADD_OP;
	tree->rBranch->expr_type = VALUE_T;
	tree->rBranch->value = 0;

	while(tokens[ix].aop == ADD || tokens[ix].aop == SUB) {
		if(tokens[ix].aop == ADD) ix++;
		term(tree->rBranch);
	}
}

void factor(expr_node *tree) {
	tree->expr_type = OPERATION;
	tree->resolution_type = STATIC;

	//puts("In factor");
	tree->lBranch = calloc(1, sizeof(expr_node));
	tree->rBranch = calloc(1, sizeof(expr_node));

	negate(tree->lBranch);
	tree->operation = MULT_OP;
	tree->rBranch->expr_type = VALUE_T;
	tree->rBranch->value = 1;

	while(tokens[ix].aop == MULT || tokens[ix].aop == DIV) {
		switch(tokens[ix].aop) {
		case MULT:
			tree->operation = MULT_OP;
			break;
		case DIV:
			tree->operation = DIV_OP;
			break;
		}

		ix++;
		factor(tree->rBranch);
	}
}

void negate(expr_node *tree) {
	tree->expr_type = OPERATION;
	tree->resolution_type = STATIC;
	tree->operation = NEG_OP;

	//puts("In negate...");
	if(tokens[ix].aop != SUB) {
		value(tree);
		return;
	} else {
		tree->lBranch = calloc(1, sizeof(expr_node));

		ix++;
		negate(tree->lBranch);
	}
}

void value(expr_node *tree) {
	//puts("In value...");

	//printf("Type: %d\n", tokens[ix].type);
	tree->expr_type = VALUE_T;
	tree->resolution_type = STATIC;
	tree->value = 0;

	if(tokens[ix].aop == LPAR) {
		ix++;
		expr(tree);
		ix++;
	} else if(tokens[ix].kword == INVOKE) {
		ix++;
		invoke(tree);
		ix++;
	} else if(tokens[ix].type == IDENT) {
		tree->expr_type = VARIABLE_T;
		tree->resolution_type = DYNAMIC;

		tree->var_ident = copy_string(tokens[ix].ident);
		ix++;
	} else if(tokens[ix].type == VALUE) {
		tree->expr_type = VALUE_T;
		tree->value = tokens[ix].value;

		ix++;
	}
}

program_node *parse_program(TokenArray arr) {
	ix = 0;
	tokens = arr.ptr;
	len = arr.len;

	program_node *to_ret = malloc(sizeof(program_node));
	to_ret->funcs = NULL;

	program(to_ret);

	return to_ret;
}
