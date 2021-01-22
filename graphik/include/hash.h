#ifndef __HASH_H__
#define __HASH_H__

#include "my_string.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	FUNCTION,
	VARIABLE,
	ARGUMENT
} type;

typedef struct {
	type symbol_type;

	size_t num;
	char *scope;

	String ident;
	size_t *arg_offsets, arg_num;
} var, symbol;

typedef struct tag_list {
	var node;
	struct tag_list *next;
} list;

typedef struct {
	list **ptr;
	size_t sz;
} map;

map make_map(size_t);

void insert(map*, var);
size_t hash(String, size_t);

var take(map, String);
var take_from_scope(map, String, char*);
var find(list*, String);

list *make_list(var);
list *add_node(list*, var);

void delete_map(map*);
void delete_list(list*);
#endif
