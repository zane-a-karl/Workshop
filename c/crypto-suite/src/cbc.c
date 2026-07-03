#include "ecb.h"
#include <math.h> // ceil()
#include <pthread.h>
#include <stdio.h>


/**
 * Notes:
 *  - Notes on CBC
 *  - code CBC
 */

/** CBC OVERVIEW
    Assume the BLOCK_LENGTH = 2

    ENCRYPTION:
              ----------------------------------------------
    PTXT ---> |    P0    |    P1    |     P2    |    P3    |
              ----------------------------------------------
                   |          |           |          |
                   |          |           |          |
                 -----      -----       -----      -----
                 |XOR|      |XOR|       |XOR|      |XOR|
                 -----      -----       -----      -----
                   |          |           |          |
                   |          |           |          |
                   V          V           V          V
              ---------------------------------------------
    KEY  ---> |    K0    |    K1    |    K0    |    K1    |
              ---------------------------------------------
                   |          |           |          |
                   |          |           |          |
                   V          V           V          V
              ----------------------------------------------
    CTXT ---> |    C0    |    C1    |     C2    |    C3    |
              ----------------------------------------------
                   |          |           |          |
    DECRYPTION:    |          |           |          |
                 -----      -----       -----      -----
                 |XOR|      |XOR|       |XOR|      |XOR|
                 -----      -----       -----      -----
                   |          |           |          |
                   |          |           |          |
                   V          V           V          V
              ---------------------------------------------
    KEY  ---> |    K0    |    K1    |    K0    |    K1    |
              ---------------------------------------------
                   |          |           |          |
                   |          |           |          |
                   V          V           V          V
              ----------------------------------------------
    PTXT ---> |    P0    |    P1    |     P2    |    P3    |
              ----------------------------------------------
          
    ECB ADVANTAGES:
    - It is very fast because it can be easily parallelized.
    - It is incredibly simple to implement.

    ECB DISADVANTAGES:
    - The same block of plaintext always maps to the same
    block of ciphertext, so it is easily broken by a
    frequency analysis attack.
*/


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
    split_into_blocks_for_thd(&(thd[t].i_block), padded_input,
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
pad_input(bool **padded_input,
	  bool *input,
	  int input_len,
	  int pad_len) {
  
  memcpy((*padded_input), input, input_len * sizeof *input);
  memset((*padded_input) + input_len, 0,
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
