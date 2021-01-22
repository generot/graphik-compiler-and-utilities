#include "../include/parse_util.h"
#include <stdarg.h>

int emit(FILE *stream, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);

	int bytes = vfprintf(stream, format, arglist);
	fputc('\n', stream);

	va_end(arglist);
	return bytes + 1;
}

void issue_error(const char *err_msg, ...) {
	va_list arglist;
	va_start(arglist, err_msg);

	fprintf(stderr, "GLC: ");
	vfprintf(stderr, err_msg, arglist);
	va_end(arglist);

	exit(-1);
}
