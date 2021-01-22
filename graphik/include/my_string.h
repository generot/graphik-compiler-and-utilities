#ifndef __MY_STRING__
#define __MY_STRING__

#include <string.h>

#define DEFAULT_SZ 30
#define REALLOC_INTERVAL 6
#define INVALID_STRING (String){NULL, 0, 0}

#define IS_ALPHA(x) (((x) >= 'A' && (x) <= 'Z') || ((x) >= 'a' && (x) <= 'z'))

#define IS_NUM(x) ((x) >= '0' && (x) <= '9')
#define IS_ALNUM(x) (IS_ALPHA((x)) || IS_NUM((x)))
#define IS_ALPHANUM(x) (((x) >= 'A' && (x) <= 'Z') || ((x) >= 'a' && (x) <= 'z') || ((x) >= '0' && (x) <= '9'))
#define IS_WHITESPACE(x) ((x) != ' ' || (x) != '\n' || (x) != '\t')

typedef unsigned long long int size_t;

typedef enum {
	ALPHA,
	NUM,
	ALPHANUM,
	SPECIAL_ANSI
} string_type;

typedef struct {
	char *ptr;
	size_t len, max_size;
} String;

typedef struct {
	String *ptr;
	size_t size, ix;
} StringVector;

StringVector make_vector(size_t);

//To pop a string and push another one
//in its place, one should first save
//the popped string in a temporary variable
//and then delete it before pushing the new
//string.
String pop(StringVector*);
void push_back(StringVector*, String);

String make_string(const char*);
String copy_string(String);
String make_string_char(char);

void delete_string(String*);
void delete_vector(StringVector*);
void string_push_back(String*, char);
char string_pop(String*);
char *substring(char*, char);

int is_empty(String);
int is_alpha(String);
string_type string_dettype(String);

#endif
