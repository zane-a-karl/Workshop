#inlcude "utils-str-input.h"


typedef struct thread_data {
  bool *o_block;
  bool *i_block;
  bool *key;
  int block;
  int block_len;
  int key_len;
} thread_data;

void
pad_input(bool **padded_input,
	  bool *input,
	  int input_len,
	  int pad_len) {
  
  memcpy((*padded_input), input, input_len * sizeof *input);
  memset((*padded_input) + input_len,
	 0,
	 pad_len % input_len * sizeof *input);
}

void
split_into_blocks(bool ***input_blocks,
		  bool *padded_input,
		  int block,
		  int block_len) {

  for (int j = 0; j < block_len; ++j) {
    (*input_blocks)[block][j] =
      padded_input[block_len*block + j];
  }
}

void
split_into_blocks_for_thd (bool **input_block,
			   bool *padded_input,
			   int block,
			   int block_len) {

  for (int j = 0; j < block_len; ++j) {
    (*input_block)[j] =
      padded_input[block_len*block + j];
  }
}

void
copy_blocks_to_output(bool **output,
		      bool **output_blocks,
		      int block_len,
		      int n_blocks,
		      int input_len) {

  memset(*output, 0, input_len * sizeof **output);
  for (int i = 0; i < n_blocks; ++i) {
    for (int j = 0; j < block_len; ++j) {
      if (block_len*i + j >= input_len) {
	break;
      }
      (*output)[block_len*i + j] = output_blocks[i][j];
    }
  }
}

void
copy_thd_blocks_to_output(bool **output,
			  thread_data thd[],
			  int n_blocks,
			  int input_len) {
  
  memset(*output, 0, input_len * sizeof **output);
  for (int i = 0; i < n_blocks; ++i) {
    for (int j = 0; j < thd[i].block_len; ++j) {
      if (thd[i].block_len*i + j >= input_len) {
	break;
      }
      (*output)[thd[i].block_len*i + j] = thd[i].o_block[j];
    }
  }
}

void
free_mem (bool ***output_blocks,
	  bool ***input_blocks,
	  bool **padded_input,
	  int n_blocks) {
  
  for (int i = 0; i < n_blocks; ++i) {
    free((*input_blocks)[i]);
    free((*output_blocks)[i]);
  }
  free((*input_blocks));
  free((*output_blocks));
  free((*padded_input));
}
