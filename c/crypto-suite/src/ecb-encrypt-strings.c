#include "encrypt-str-input.h"


/** Encode and Decode are the same function, just with the
 * inputs moved around.
 * Notice that should the key length > block_len only the first
 * block_len bits of the key will be used. Should key length <
 * block_len then the final (block_len-key_len) bits will be
 * filled by the first (block_len-key_len) bits of the key.
 */
void
ecb (bool **output, // O
     bool *input,
     bool *key,
     int block_len,
     int input_len,
     int key_len) {

  int n_blocks = ceil((double)input_len/(double)block_len);
  int pad_len = n_blocks * block_len;
  bool *padded_input = (bool *)calloc(pad_len, sizeof *padded_input);
  pad_input(&padded_input, input, input_len, pad_len);
  
  bool **input_blocks = (bool **)calloc(n_blocks,
					sizeof *input_blocks);
  for (int i = 0; i < n_blocks; ++i) {
    input_blocks[i] = (bool *)calloc(block_len,
				     sizeof **input_blocks);
    split_into_blocks(&input_blocks, padded_input, i, block_len);
  }

  bool **output_blocks = (bool **)calloc(n_blocks,
					 sizeof *output_blocks);
  TSTART;
  for (int i = 0; i < n_blocks; ++i) {
    output_blocks[i] = (bool *)calloc(block_len,
				      sizeof **output_blocks);
    encode (&output_blocks, input_blocks,key, i, block_len, key_len);
  }
  TSTAMP;

  copy_blocks_to_output(output, // O
			output_blocks,
			block_len,
			n_blocks,
			input_len);

  free_mem(&output_blocks, &input_blocks, &padded_input, n_blocks);
}

void *
threaded_encode (void *thread_args) {

  thread_data *thd = (thread_data *)thread_args;
  for (int j = 0; j < thd->block_len; ++j) {
    if (thd->key_len >= thd->block_len) {
      thd->o_block[j] = thd->i_block[j] ^
	thd->key[ (thd->block_len*thd->block + j)%thd->block_len ];
    } else {
      thd->o_block[j] = thd->i_block[j] ^
	thd->key[ (thd->block_len*thd->block + j)%thd->key_len ];
    }
  }
  return thread_args;
}

void
threaded_ecb (bool **output, // O
	      bool *input,
	      bool *key,
	      int block_len,
	      int input_len,
	      int key_len) {

  int n_blocks = ceil((double)input_len/(double)block_len);
  int rc;
  pthread_t threads[n_blocks];
  thread_data thd[n_blocks];
  for (int t = 0; t < n_blocks; ++t) {
    thd[t].key = key;
    thd[t].block_len = block_len;
    thd[t].key_len = key_len;
  }
  
  int pad_len = n_blocks * block_len;
  bool *padded_input = (bool *)calloc(pad_len, sizeof *padded_input);
  pad_input(&padded_input, input, input_len, pad_len);

  for (int t = 0; t < n_blocks; ++t) {
    thd[t].i_block = (bool *)calloc(block_len,
				    sizeof *(thd[t].i_block));
    thd[t].block = t;
    split_into_blocks_for_thd(&(thd[t].i_block),
			      padded_input,
			      t, block_len);
  }

  TSTART;
  for (int t = 0; t < n_blocks; ++t) {
    thd[t].o_block = (bool *)calloc(block_len,
				    sizeof *(thd[t].o_block));
    rc = pthread_create(&threads[t], NULL,
			threaded_encode, (void *)&thd[t]);
    if (rc != 0) {
      fprintf(stderr,
	      "Failed to create thread #%d, rc=%d\n",
	      thd[t].block, rc);
    }
  }
  TSTAMP;

  for (int t = 0; t < n_blocks; ++t) {
    pthread_join(threads[t], NULL);
  }

  for (int t = 0; t < n_blocks; ++t) {
    copy_thd_blocks_to_output(output, // O
			      thd,
			      n_blocks,
			      input_len);
  }

  for (int t = 0; t < n_blocks; ++t) {
    free(thd[t].o_block);
    free(thd[t].i_block);
  }
  free(padded_input);
}

void
encode (bool ***output_blocks,
	bool **input_blocks,
	bool *key,
	int block,
	int block_len,
	int key_len) {

  for (int j = 0; j < block_len; ++j) {
    if (key_len >= block_len) {
      (*output_blocks)[block][j] = input_blocks[block][j] ^
	key[ (block_len*block + j)%block_len ];
    } else {
      (*output_blocks)[block][j] = input_blocks[block][j] ^
	key[ (block_len*block + j)%key_len ];
    }
  }
}
