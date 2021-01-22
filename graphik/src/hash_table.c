#include "../include/hash.h"
#define NON -1

map make_map(size_t max_len) {
	map new_map = {
		calloc(max_len, sizeof(list*)),
		max_len
	};

	var def_var = {NON, 0, NULL, INVALID_STRING};
	for(size_t i = 0; i < max_len; i++) 
		new_map.ptr[i] = make_list(def_var);

	return new_map;
}

size_t hash(String ident, size_t map_size) {
	size_t ix = 0;
	for(size_t i = 0; i < ident.len; i++) {
		ix += ident.ptr[i];
		ix ^= ix << 2;
	}

	return ix % map_size;
}

void insert(map *mp, var v) {
	size_t ix_to_map = hash(v.ident, mp->sz);
	mp->ptr[ix_to_map] = add_node(mp->ptr[ix_to_map], v);
}

var take(map mp, String ident) {
	size_t ix = hash(ident, mp.sz);
	return find(mp.ptr[ix], ident);
}

var find(list *ls, String ident) {
	for(list *i = ls; i; i = i->next) {
		if(i->node.ident.ptr && !strcmp(i->node.ident.ptr, ident.ptr))
			return i->node;
	}

	return (var){NON, 0, NULL, INVALID_STRING};
}

var take_from_scope(map mp, String ident, char *superscope) {
	size_t ix = hash(ident, mp.sz);
	for(list *ls = mp.ptr[ix]; ls; ls = ls->next) {
		if(ls->node.ident.ptr) {
			char *sscope = substring(ls->node.scope, '_');

			if(!strcmp(ls->node.ident.ptr, ident.ptr)
			&& !strcmp(sscope, superscope))
				return ls->node;
		}
	}

	return (var){NON, 0, NULL, INVALID_STRING};
}

list *make_list(var v) {
	list *new_list = malloc(sizeof(list));

	new_list->node = v;
	new_list->next = NULL;

	return new_list;
}

list *add_node(list *ls, var v) {
	list *new_node = malloc(sizeof(list));

	new_node->node = v;
	new_node->next = ls;
	ls = new_node;

	return ls;
}

void delete_list(list *base) {
	if(!base) return;

	delete_list(base->next);
	free(base);
}

void delete_map(map *mp) {
	for(size_t i = 0; i < mp->sz; i++)
		delete_list(mp->ptr[i]);
}	
