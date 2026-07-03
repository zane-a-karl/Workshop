#ifndef _BASIC_BLOCK_H_
#define _BASIC_BLOCK_H_

#include "dlx.h"

#include <stdlib.h>

typedef struct BasicBlock {
	int blk_num;
	int cur_ssa_idx;
	Instruction *i_list;
	struct BasicBlock *left;
	struct BasicBlock *right;
} BasicBlock;

BasicBlock *
alloc_basic_block (int block_number);

void
reset_basic_block (BasicBlock *bb);

void
free_basic_block (BasicBlock *bb);

#endif//_BASIC_BLOCK_H_
