#include "../include/lexer.h"
#include <stdlib.h>

Token make_token(token_type tp, logical_op lop, 
		arthm_op aop, keyword kwd, String ident, double val)
{
	Token new_token = {
		tp,
		lop,
		aop,
		kwd,
		ident,
		val
	};

	return new_token;
}

StringVector tokenize(FILE *stream) {
	StringVector vec = make_vector(10);

	String token = make_string("");
	char current_char, prev_char = 0;

	while((current_char = getc(stream)) != EOF) {	
		if(IS_ALPHANUM(current_char)) {
			string_push_back(&token, current_char);
		}
		
		if(!IS_ALPHANUM(current_char) && IS_ALPHANUM(prev_char)) {
			push_back(&vec, token);
			delete_string(&token);

			token = make_string("");
		}

		if(!IS_ALPHANUM(current_char) && current_char != ' ' && current_char != '\n' && current_char != '\t')
			push_back(&vec, make_string_char(current_char));

		prev_char = current_char;
	}

	return vec;
}

keyword check_keyword(String tok) {
	const char *keywords[] = {
		"if", "while",
		"begin", "end",
		"break", "set",
		"plot", "declare",
		"invoke", "none",
		"ret"
	};

	for(int i = 0; i < sizeof(keywords)/sizeof(char*); i++) {
		if(!strcmp(tok.ptr, keywords[i]))
			switch(i) {
			case 0:  return IF;
			case 1:  return WHILE;
			case 2:  return BEGIN;
			case 3:  return END;
			case 4:  return BREAK;
			case 5:  return SET;
			case 6:  return PLOT;
			case 7:  return DECLARE;
			case 8:  return INVOKE;
			case 9:  return NONE;
			case 10: return RET;
			}
	}

	return NON;
}

arthm_op check_ar_oper(String tok) {
	const char *ops[] = {
		"+", "-",
		"*", "/",
		"^", ")",
		"("
	};

	for(int i = 0; i < sizeof(ops)/sizeof(char*); i++) {
		if(!strcmp(tok.ptr, ops[i]))
			switch(i) {
			case 0: return ADD;
			case 1: return SUB;
			case 2: return MULT;
			case 3: return DIV;
			case 4: return POW;
			case 5: return RPAR;
			case 6: return LPAR;
			}
	}

	return NON;
}

logical_op check_log_op(String tok, String next_tok) {
	if(!strcmp(tok.ptr, "<")) {
		if(!strcmp(next_tok.ptr, "="))
			return LEQ;

		return LT;
	} else if(!strcmp(tok.ptr, ">")) {
		if(!strcmp(next_tok.ptr, "="))
			return GEQ;

		return GT;
	} 
	else if(!strcmp(tok.ptr, "!") && !strcmp(next_tok.ptr, "="))  return NEQ;
	else if(!strcmp(tok.ptr, "|") && !strcmp(next_tok.ptr, "|"))  return OR;
	else if(!strcmp(tok.ptr, "&") && !strcmp(next_tok.ptr, "&"))  return AND;
	else if(!strcmp(tok.ptr, "=") && !strcmp(next_tok.ptr, "="))  return EQ;
	else if(!strcmp(tok.ptr, "=")) 	return ASG;

	return NON;
}

TokenArray lex(FILE *stream) {
	StringVector tokens = tokenize(stream);

	Token *lexed_tokens = calloc(tokens.size, sizeof(Token));
	size_t tok_ix = 0;
	
	for(size_t i = 0; i < tokens.ix; i++) {
		string_type str_tp = string_dettype(tokens.ptr[i]);

		keyword maybe_keyword = NON;
		arthm_op maybe_ar_op = NON;
		logical_op maybe_log_op = NON;

		switch(str_tp) {
		case ALPHA:
			maybe_keyword = check_keyword(tokens.ptr[i]);

			if(maybe_keyword != NON) { 
				lexed_tokens[tok_ix++] = make_token(
					KEYWORD, NON, 
					NON, maybe_keyword, 
					INVALID_STRING, 0
				);

				maybe_keyword = NON;
			} else {
				lexed_tokens[tok_ix++] = make_token(
					IDENT, NON, 
					NON, NON, 
					copy_string(tokens.ptr[i]), 0
				);
			}

			break;
		case NUM:
			lexed_tokens[tok_ix++] = make_token(
				VALUE, NON, 
				NON, NON, 
				INVALID_STRING, atof(tokens.ptr[i].ptr)
			);
			break;

		case ALPHANUM:
			lexed_tokens[tok_ix++] = make_token(
				IDENT, NON, 
				NON, NON, 
				copy_string(tokens.ptr[i]), 0
			);
			break;

		case SPECIAL_ANSI:
			maybe_ar_op = check_ar_oper(tokens.ptr[i]);
			if(maybe_ar_op != NON) {
				lexed_tokens[tok_ix++] = make_token(
					OPERATOR, NON, 
					maybe_ar_op, NON, 
					INVALID_STRING, 0
				);

				maybe_ar_op = NON;
			} else {
				maybe_log_op = check_log_op(tokens.ptr[i], tokens.ptr[i + 1]);

				if(maybe_log_op != NON) {
					lexed_tokens[tok_ix++] = make_token(
						OPERATOR, maybe_log_op, 
						NON, NON, 
						INVALID_STRING, 0
					);

					switch(maybe_log_op) {
					case GEQ: case LEQ: case NEQ: case EQ: case OR: case AND: i++;
					break;
					}

					maybe_log_op = NON;
				}
			}

			//i++;
			break;
		} 
	}
	
	lexed_tokens = realloc(lexed_tokens, tok_ix * sizeof(Token));
	delete_vector(&tokens);

	return (TokenArray){lexed_tokens, tok_ix};
}

void print_tokens(Token *arr, size_t len) {
	for(size_t i = 0; i < len; i++) {
		Token ctok = arr[i];
		printf("Type: %d\nLog. op: %d\nAr. op: %d\nKeyword: %d\nIdentifier: %s\nValue: %lli\n\n", 
				ctok.type, ctok.lop, ctok.aop, ctok.kword, ctok.ident.ptr ? 
				ctok.ident.ptr : "NULL", ctok.value);
	}
}

