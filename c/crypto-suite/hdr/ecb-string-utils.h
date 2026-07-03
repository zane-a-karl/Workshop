#ifndef _UTILS_STR_INPUT_H_
#define _UTILS_STR_INPUT_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h> // memset()

#include <math.h> // ceil()
#include <pthread.h>
#include <stdio.h>


void
pad_input (bool **padded_input, // O
	   bool *input,
	   int input_len,
	   int pad_len);

void
split_into_blocks (bool ***input_blocks, // O
		   bool *padded_input,
		   int block,
		   int block_len);

void
split_into_blocks_for_thd (bool **input_block, // O
			   bool *padded_input,
			   int block,
			   int block_len);

void
copy_blocks_to_output (bool **output, // O
		       bool **output_blocks,
		       int block_len,
		       int n_blocks,
		       int input_len);

void
copy_thd_blocks_to_output (bool **output, // O
			   thread_data thd[],
			   int n_blocks,
			   int input_len);

void
free_mem (bool ***output_blocks, // O
	  bool ***input_blocks, // O
	  bool **padded_input, // O
	  int n_blocks);

#endif // _UTILS_STR_INPUT_H_
