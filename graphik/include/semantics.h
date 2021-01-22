#ifndef __SANALYSIS_H__
#define __SANALYSIS_H__

#include "hash.h"
#include "ast.h"
#include "parse_util.h"

#define SYMTABLE_SZ 40
#define VAR_SIZE 8
#define STFRAME_OFFSET (VAR_SIZE * 2)

typedef struct {
	map sym_table;
	size_t final_stack_offset;
} analysis_res;

analysis_res perform_smc_analysis(program_node*);
void parse_block(block_node*, map*);

#endif
