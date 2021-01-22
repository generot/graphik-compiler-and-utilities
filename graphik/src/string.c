#include <stdlib.h>

#include "../include/my_string.h"

String make_string(const char *ptr) {
	size_t len = strlen(ptr);

	char *init_mem = calloc(len + 1, sizeof(char));
	memcpy(init_mem, ptr, len + 1);

	String to_ret = {
		init_mem,
		len,
		len
	};

	return to_ret;
}

String make_string_char(char ch) {
	char *init_mem = calloc(DEFAULT_SZ, sizeof(char));
	init_mem[0] = ch;

	String to_ret = {
		init_mem,
		1,
		DEFAULT_SZ
	};

	return to_ret;
}

String copy_string(String to_copy) {
	String new_string = {
		strdup(to_copy.ptr),
		to_copy.len,
		to_copy.max_size
	};

	return new_string;
}

StringVector make_vector(size_t reserved_size) {
	StringVector to_ret = {
		calloc(reserved_size, sizeof(String)),
		reserved_size, 0
	};
	
	return to_ret;
}

String pop(StringVector *vec) {
	if(vec->ix <= 0) return INVALID_STRING;
	return vec->ptr[--vec->ix];
}

int is_alpha(String to_check) {
	for(int i = 0; i < to_check.len; i++)
		if(!IS_ALPHA(to_check.ptr[i]))
			return 0;

	return 1;
}

string_type string_dettype(String to_check) {
	int alp = 0, num = 0, ansi = 0;
	for(int i = 0; i < to_check.len; i++) {
		if(IS_ALPHA(to_check.ptr[i]))
			alp++;
		else if(IS_NUM(to_check.ptr[i]))
			num++;
		else ansi++;
	}

	if(ansi) return SPECIAL_ANSI;

	if(alp && !num) return ALPHA;
	else if(!alp && num) return NUM;
	
	return ALPHANUM;
}

int is_empty(String to_check) {
	return to_check.len == 0;
}

void push_back(StringVector *vec, String to_push) {
	if(vec->ix >= vec->size) {
		vec->ptr = realloc(vec->ptr, (vec->size + REALLOC_INTERVAL) * sizeof(String));
		vec->size += REALLOC_INTERVAL;
	}
	
	vec->ptr[vec->ix++] = copy_string(to_push);
}

void delete_string(String *str) {
	free(str->ptr);

	str->ptr = NULL;
	str->len = 0;

	return;
}

void delete_vector(StringVector *to_delete) {
	for(size_t i = 0; i < to_delete->ix; i++)
		delete_string(&to_delete->ptr[i]);

	free(to_delete->ptr);

	to_delete->ptr = NULL;
	to_delete->size = to_delete->ix = 0;
}

void string_push_back(String *str, char to_push) {
	if(str->len >= str->max_size) {
		str->ptr = realloc(str->ptr, str->max_size + REALLOC_INTERVAL + 1);
		str->max_size += REALLOC_INTERVAL;
	}

	str->ptr[str->len++] = to_push;
	str->ptr[str->len] = '\0';
}

char string_pop(String* str) {
	char popped_char = str->ptr[--str->len];
	str->ptr[str->len] = '\0';

	return popped_char;
}

char *substring(char *str, char ch) {
	char substr[DEFAULT_SZ] = {0};
	int ix = 0;

	while(*str) {
		substr[ix++] = *str;
		if(*str++ == ch) break;
	}

	return strdup(substr);
}

