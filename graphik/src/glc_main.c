#include <stdio.h>
#include <stdlib.h>

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantics.h"
#include "../include/gen_x64.h"

#define INTERNAL_ERROR(...) { 		\
	fprintf(stderr, __VA_ARGS__);	\
	exit(EXIT_FAILURE);		\
}

#define USG_ERROR "Incorrect output specification.\n"

#define MAX_CMD 50

#define COMPILE  0x02
#define ASSEMBLE 0x04
#define LINK	 0x08

char *change_extension(char *dir, char *new_ex) {
	char *no_ex = substring(dir, '.');
	strcat(no_ex, new_ex);

	return no_ex;
}

int compiler_entry(FILE *in, FILE *out, int flags, char *output_name, char *asm_ext) {
	TokenArray tokens = lex(in);
	program_node *ast = parse_program(tokens);
	analysis_res semantic_results = perform_smc_analysis(ast);

	char nasm_cmd[MAX_CMD] = {0}, ld_cmd[MAX_CMD] = {0},
	     *object_out = change_extension(output_name, "o"),
	     *exe_out = change_extension(output_name, "exe");

	sprintf(nasm_cmd, "nasm -f win64 -o %s %s", object_out, asm_ext);
	puts(nasm_cmd);

	sprintf(ld_cmd, "ld -o %s %s -L. -lmsvcrt -e main", exe_out, object_out);
	puts(ld_cmd);

	if(flags & COMPILE) generate_asm(out, ast, semantic_results);
	if(flags & ASSEMBLE) {
		fclose(out);
		system(nasm_cmd);
	}
	if(flags & LINK) {
		system(ld_cmd);
	}

	return flags;
}

int main(int argc, char **argv) {
	FILE *input = NULL, *output = NULL;
	char *out_name = NULL, *out_name2 = NULL;

	const char *options[] = {
		"-help", "-o",
		"-c", "-s", "-credits"
	};

	size_t opt_sz = sizeof(options) / sizeof(char*);
	int flags = 0;

	if(argc > 1) {
		for(int j = 0; j < argc; j++) {
			for(int i = 0; i < opt_sz; i++) {
				if(!strcmp(argv[j], options[i])) {
					switch(i) {
					case 0:
						printf( "Graphik Lightweight Compiler - version 0.1.0\n"
						"-o -> Specify output characteristics(name, extension, etc.)\n"
						"-s -> Compile to x64 Assembly\n"
						"-c -> Compile and assemble(via NASM)\n"
						"-credits -> Information about the developer/development of GLC"
						"glc <source_file> -> Compile, assemble and link into an executable" );
						return EXIT_SUCCESS;
					case 1:
						//Handle output
						if(argv[j + 1] && argv[j + 2]) {
							out_name = strdup(change_extension(argv[j + 1], "s"));
							out_name2 = strdup(argv[j + 1]);

							input = fopen(argv[j + 2], "r");
							output = fopen(out_name, "w");

							flags = COMPILE | ASSEMBLE | LINK;
						} else INTERNAL_ERROR(USG_ERROR "Usage: glc -o <output> <src>");

						break;
					case 2:
						//Handle object output
						if(argv[j + 1]) {
							out_name = strdup(change_extension(argv[j + 1], "s"));
							out_name2 = strdup(argv[j + 1]);

							input = fopen(argv[j + 1], "r");
							output = fopen(out_name, "w");

							flags = COMPILE | ASSEMBLE;
						} else INTERNAL_ERROR(USG_ERROR "Usage: glc -c <src>");

						break;
					case 3: 
						//Handle assembly output
						if(argv[j + 1]) {
							out_name = strdup(change_extension(argv[j + 1], "s"));
							out_name2 = strdup(argv[j + 1]);

							input = fopen(argv[j + 1], "r");
							output = fopen(out_name, "w");

							flags = COMPILE;
						} else INTERNAL_ERROR(USG_ERROR "Usage: glc -s <src>");
						break;
					case 4:
						printf("---------------------------------------------------------------------------------------------------------------\n"
						"The Graphik Lightweight Compiler and all Graphik utilities,\n"
						"coming with it, have been solely built by Martin Nakov('generot' in GitHub).\n"
						"---------------------------------------------------------------------------------------------------------------\n"
						"The graphical calculator and compiler have been written entirely from scratch,\n"
						"and no third party libraries/dependencies have been used for their creation.\n"
						"---------------------------------------------------------------------------------------------------------------\n"
						"Only the Windows Native API has been utilized for the production of the graphical calculator's user interface.\n"
						"---------------------------------------------------------------------------------------------------------------\n"
						"The entire utility package serves as my 1st semester programming project, nevertheless, development will\n"
						"most likely continue in the future, as long as I have the time and the nerve to work on it.\n"
						"---------------------------------------------------------------------------------------------------------------\n");
						return EXIT_SUCCESS;
					}
				}
			}
		}
	} else INTERNAL_ERROR("No arguments provided.\nUse \"glc -help\" for details\n");

	compiler_entry(input, output, flags, out_name2, out_name);

	if(out_name) free(out_name);
	if(out_name2) free(out_name2);

	return 0;
}
