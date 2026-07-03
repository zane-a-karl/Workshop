#include "../hdr/basic_block.h"

BasicBlock *
alloc_basic_block (int block_number) {
	BasicBlock *bb = (BasicBlock *)calloc(1, sizeof(BasicBlock));
	bb->blk_num = block_number;
	bb->i_list = NULL;
	bb->left = NULL;
	bb->right = NULL;

	return bb;
}
