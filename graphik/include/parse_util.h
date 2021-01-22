#ifndef __PARSE_UTIL__
#define __PARSE_UTIL__

#include <stdio.h>
#include <stdlib.h>

int emit(FILE *stream, const char *format, ...);
void issue_error(const char *err_msg, ...);

#endif
