#include "../include/util.h"
#include <stdlib.h>

char *token_ptr = NULL;
double param_value = 0;

void init_parser(char *ptr, double p_val) {
	token_ptr = ptr;
	param_value = p_val;
}

/////////////////////////////////////
//////////////PARSER/////////////////
/////////////////////////////////////
double parse_term() {
	double t1 = parse_factor();
	while(*token_ptr == '+' || *token_ptr == '-') {
		char oper = *token_ptr;
		token_ptr++;

		double t2 = parse_factor();

		switch(oper) {
		case '+': t1 += t2; break;
		case '-': t1 -= t2; break;
		}
	}

	return t1;
}

double parse_factor() {
	double f1 = parse_exp();
	while(*token_ptr == '*' || *token_ptr == '/') {
		char oper = *token_ptr;
		token_ptr++;

		double f2 = parse_exp();

		switch(oper) {
		case '*': f1 *= f2; break;
		case '/': f1 /= f2; break;
		}
	}

	return f1;
}

double parse_exp() {
	double t1 = parse_number();
	while(*token_ptr == '^') {
		token_ptr++;
		double t2 = parse_number();

		t1 = pow(t1, t2);
	}
	
	return t1;
}

double parse_number() {
	double num = 0, dec = 10;
	while(*token_ptr >= '0' && *token_ptr <= '9') {
		num = num * 10 + (*token_ptr - '0');
		token_ptr++;
		if(*token_ptr == '.') {
			token_ptr++;
			while(*token_ptr >= '0' && *token_ptr <= '9') {
				num = num + (*token_ptr - '0') / dec;
				token_ptr++;
				dec *= 10;
			}
		}
	}

	if(*token_ptr >= 'a' && *token_ptr <= 'z') {
		num = param_value;
		token_ptr++;
	}
	
	if(*token_ptr == '(') {
		token_ptr++;
		num = parse_term();
		token_ptr++;
	}

	return num;
}
/////////////////////////////////////
//////////////PARSER/////////////////
/////////////////////////////////////

/////////////////////////////////////
//////////////LEXER/////////////////
/////////////////////////////////////
//3 * x + 43
//x++3
char *tokenize(char *stream) {
	char *tokens = malloc(MAX_TOKENS * sizeof(char));
	int ix = 0, seen_oper = 0;
	
	while((tokens[ix] = *stream++)) {
		if(tokens[ix] == ' ')
			continue;

		if(IS_OP(tokens[ix])) seen_oper++;
		else seen_oper = 0;

		if(seen_oper > 1) {
			MessageBoxA(NULL, "Invalid expression.", "Parse error", MB_ICONERROR);
			return NULL;
		}

		ix++;
	}
	
	return tokens;
}	
/////////////////////////////////////
//////////////LEXER/////////////////
/////////////////////////////////////
