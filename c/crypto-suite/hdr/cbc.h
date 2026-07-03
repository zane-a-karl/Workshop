#ifndef _CBC_H_
#define _CBC_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h> // memset()

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <time.h>

static struct timespec t1,t2;
static double sec;

#define TSTART clock_gettime(CLOCK_MONOTONIC, &t1);
#define TSTAMP clock_gettime(CLOCK_MONOTONIC, &t2); \
              sec = (t2.tv_sec - t1.tv_sec) + \
		(t2.tv_nsec - t1.tv_nsec) / \
		1000000000.0; \
	      fprintf(stderr,"Line:%5d, Time = %f\n",__LINE__,sec);


typedef struct thread_data {
  bool *o_block;
  bool *i_block;
  bool *key;
  int block;
  int block_len;
  int key_len;
} thread_data;

void
ecb (bool **output, // O
     bool *input,
     bool *key,
     int block_len,
     int input_len,
     int key_len);

void
threaded_ecb (bool **output, // O
	      bool *input,
	      bool *key,
	      int block_len,
	      int input_len,
	      int key_len);

// ####################### Util functions #######################
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
encode (bool ***output_blocks, // O
	bool **input_blocks, // O
	bool *key,
	int block,
	int block_len,
	int key_len);

void *
threaded_encode (void *thread_args);

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

#endif // _CBC_H_
