#ifndef __LEXER_H__
#define __LEXER_H__

#define NON -1

#include <stdio.h>
#include "my_string.h"

typedef enum {
	IDENT,
	KEYWORD,
	OPERATOR,
	VALUE
} token_type;

typedef enum {
	ADD = 1,
	SUB,
	MULT,
	DIV,
	POW,
	LPAR,
	RPAR
} arthm_op;

typedef enum {
	OR = 1,
	AND,
	NEQ,
	EQ,
	LEQ,
	GEQ,
	GT,
	LT,
	ASG
} logical_op;

typedef enum {
	IF = 1,
	WHILE,
	BEGIN,
	END,
	BREAK,
	SET,
	PLOT,
	DECLARE,
	INVOKE,
	NONE,
	RET
} keyword;


typedef struct {
	token_type type;

	logical_op lop;
	arthm_op aop;

	keyword kword;

	String ident;
	long long int value;
} Token;

typedef struct {
	Token *ptr;
	size_t len;
} TokenArray;

Token make_token(token_type, logical_op, 
		arthm_op, keyword, String, double);

TokenArray lex(FILE*);

StringVector tokenize(FILE*);

keyword check_keyword(String);
arthm_op check_ar_oper(String);
logical_op check_log_op(String, String);

void print_tokens(Token*, size_t);

#endif
