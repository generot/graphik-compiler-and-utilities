#include "../include/semantics.h"

String current_scope;
size_t stack_ptr_offset, func_arg_offset;

void check_scope(map sym_table, String var_ident, int is_func) {
	String scope = is_func ? make_string("global") : current_scope;
	char *superscope = substring(scope.ptr, '_');

	var sym = take_from_scope(sym_table, var_ident, superscope);
	//printf("///^Comparing: %s and %s^///\n", current_scope.ptr, sym.scope);

	if(!sym.scope) {
		issue_error("Symbol '%s'(scope: %s) is not defined", 
				var_ident.ptr, scope.ptr);
	} else {
		char *sscope = substring(sym.scope, '_');
		int is_within_reach = !strcmp(superscope, sscope) && scope.len > strlen(superscope);

		//printf("///^Comparing: %s and %s^///\n", current_scope.ptr, superscope);
		if(strcmp(sym.scope, scope.ptr) != 0 && !is_within_reach)
			issue_error("Symbol '%s' is not within current scope: %s", 
					var_ident.ptr, scope.ptr);

		free(superscope);
		free(sscope);
	}

	if(is_func) delete_string(&scope);
}

int determine_expr(expr_node *expr) {
	int type = STATIC;

	if(!expr) return STATIC;

	if(expr->resolution_type == DYNAMIC)
		return DYNAMIC;

	int lside = determine_expr(expr->lBranch),
	    rside = determine_expr(expr->rBranch);

	if(lside == DYNAMIC || rside == DYNAMIC)
		type = DYNAMIC;

	return type;
}

char *parse_expr(expr_node *expr, map *sym_table) {
	char exprstr[100000] = {0};

	if(!expr) return strdup("end");

	switch(expr->expr_type) {
	case OPERATION:
		switch(expr->operation) {
		case ADD_OP: sprintf(
			exprstr, "(%s + %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case SUB_OP: sprintf(
			exprstr, "(%s - %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case MULT_OP: sprintf(
			exprstr, "(%s * %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case DIV_OP: sprintf(
			exprstr, "(%s / %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case AND_OP: sprintf(
			exprstr, "(%s && %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case OR_OP: sprintf(
			exprstr, "(%s || %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;
		
		case NEG_OP: sprintf(
			exprstr, "(NEG(%s))",
			parse_expr(expr->lBranch, sym_table)
		); break;


		case EQ_OP: sprintf(
			exprstr, "(%s == %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case NEQ_OP: sprintf(
			exprstr, "(%s != %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;
		
		case GEQ_OP: sprintf(
			exprstr, "(%s >= %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;
		
		case LEQ_OP: sprintf(
			exprstr, "(%s <= %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case LT_OP: sprintf(
			exprstr, "(%s < %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		case GT_OP: sprintf(
			exprstr, "(%s > %s)",
			parse_expr(expr->lBranch, sym_table),
			parse_expr(expr->rBranch, sym_table)
		); break;

		}
		break;
	case VALUE_T:
		sprintf(exprstr, "%lli", expr->value);
		break;
	case VARIABLE_T:
		sprintf(exprstr, "%s", expr->var_ident.ptr);
		check_scope(*sym_table, expr->var_ident, 0);

		break;
	case FUNCCALL:
		sprintf(exprstr, "FUNC(%s)", expr->var_ident.ptr);

		for(size_t i = 0; i < expr->arg_num; i++) {
			int tp = determine_expr(&expr->func_args[i]);
			expr->func_args[i].resolution_type = tp;

			printf("ARG: %s\n", parse_expr(&expr->func_args[i], sym_table));
		}

		check_scope(*sym_table, expr->var_ident, 1);
		break;
	}

	return strdup(exprstr);
}

void parse_ret(ret_node *rt, map *sym_table) {
	char *expr = parse_expr(&rt->ret_val, sym_table);
	int type = determine_expr(&rt->ret_val);

	rt->ret_val.resolution_type = type;
	printf("RETURN: %s\n", expr);

	free(expr);
}

void parse_plot(plot_node *pt, map *sym_table) {
	char *expr1 = parse_expr(&pt->value1, sym_table),
	     *expr2 = parse_expr(&pt->value2, sym_table);

	int type = determine_expr(&pt->value1),
	    type2 = determine_expr(&pt->value2);

	pt->value1.resolution_type = type;
	pt->value2.resolution_type = type2;

	printf("PLOT: %s -> %s\n", expr1, expr2);

	free(expr1);
	free(expr2);
}

void parse_cond(cond_node *cnd, map *sym_table) {
	puts("TYPE: CONDITION");

	char *cond = parse_expr(&cnd->condition, sym_table);
	int type = determine_expr(&cnd->condition);

	cnd->condition.resolution_type = type;

	if(cnd->cond_type == WHILE_T) {
		string_push_back(&current_scope, 'w');
		printf("WHILE -> %s -> {\n", cond);
	} else {
		string_push_back(&current_scope, 'i');
		printf("IF -> %s -> {\n", cond);
	}

	//printf("////SCOPE: %s////\n", current_scope.ptr);
	parse_block(&cnd->scope, sym_table);
	string_pop(&current_scope);

	puts("}");

	free(cond);
}

void parse_asg(assign_node *asg, map *sym_table) {
	char *expr = parse_expr(&asg->value, sym_table);
	int type = determine_expr(&asg->value);

	asg->value.resolution_type = type;

	var sym = take(*sym_table, asg->var_ident);
	char *superscope = sym.scope ? substring(sym.scope, '_') : NULL,
	     *sscope2 = substring(current_scope.ptr, '_');

	if(!superscope || strcmp(superscope, sscope2) != 0) {
		stack_ptr_offset += sizeof(lint);
		var new_sym = { VARIABLE, stack_ptr_offset, strdup(current_scope.ptr), asg->var_ident };
		
		//printf("Var: %s -> %s\n", asg->var_ident.ptr, new_sym.scope);

		insert(sym_table, new_sym);
	}

	printf("ASSIGNMENT: %s\n", type == STATIC ? "STATIC" : "DYNAMIC");
	printf("%s = %s;\n", asg->var_ident.ptr, expr);

	free(expr);
	free(superscope);
	free(sscope2);
}

void parse_stm(statement_node *stm, map *sym_table) {
	puts("STATEMENT:");
	switch(stm->type) {
	case COND_STM:   parse_cond(&stm->cond_stm, sym_table);  break; 
	case RET_STM:    parse_ret(&stm->ret, sym_table);        break; 
	case ASSIGN_STM: parse_asg(&stm->asg, sym_table);        break; 
	case PLOT_STM:   parse_plot(&stm->plot, sym_table);      break; 
	case BREAK_STM:  puts("BREAK"); 	      		 break; 
	}
}

void parse_block(block_node *blc, map *sym_table) {
	for(size_t i = 0; i < blc->stm_num; i++)
		parse_stm(&blc->stms[i], sym_table);
}

void parse_decl(assign_node *decl, map *sym_table) {
	var new_sym = { 
		ARGUMENT, 
		func_arg_offset + STFRAME_OFFSET, 
		strdup(current_scope.ptr), 
		decl->var_ident
	};

	insert(sym_table, new_sym);
	printf("%s, ", decl->var_ident.ptr);
}

void parse_func(function_node *func, map *sym_table) {
	current_scope = make_string(func->ident.ptr);

	symbol sym_func = take(*sym_table, func->ident), new_func;
	if(!sym_func.scope) {
		new_func = (var){ 
			FUNCTION, 
			0, 
			"global", 
			func->ident, 
			malloc(func->arg_num * sizeof(size_t)),
			func->arg_num
		};

		insert(sym_table, new_func);
	} else issue_error("Function '%s' cannot be redefined.", func->ident.ptr);

	string_push_back(&current_scope, '_');
	func_arg_offset = stack_ptr_offset = 0;

	//printf("////SCOPE: %s////\n", current_scope.ptr);

	printf("FUNC - %s - (", func->ident.ptr);
	for(size_t i = 0; i < func->arg_num; i++) {
		parse_decl(&func->args[i], sym_table);
		new_func.arg_offsets[i] = func_arg_offset + STFRAME_OFFSET;

		func_arg_offset += sizeof(lint);
	}

	printf(") {\n");
	parse_block(&func->scope, sym_table);
	printf("}\n");
	
	func->stack_sz = stack_ptr_offset;
	delete_string(&current_scope);
}

void parse_ast(program_node *prog, map *sym_table) {
	puts("--PROGRAM BEGIN--");
	for(size_t i = 0; i < prog->sz; i++)
		parse_func(&prog->funcs[i], sym_table);

	puts("--PROGRAM END--");

}

analysis_res perform_smc_analysis(program_node *program) {
	map sym_table = make_map(SYMTABLE_SZ);

	stack_ptr_offset = 0;
	parse_ast(program, &sym_table); 

	return (analysis_res){ sym_table, stack_ptr_offset };
}


