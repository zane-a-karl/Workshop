#ifndef _ENCRYPT_STR_INPUT_H_
#define _ENCRYPT_STR_INPUT_H_

#include "utils-all-modes.h"
#include "utils-str-input.h"

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
                     (t2.tv_nsec - t1.tv_nsec) / 1000000000.0; \
               fprintf(stderr,"Line:%5d, Time = %f\n",__LINE__,sec);


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

void
encode (bool ***output_blocks, // O
	bool **input_blocks, // O
	bool *key,
	int block,
	int block_len,
	int key_len);

void *
threaded_encode (void *thread_args);


#endif // _ENCRYPT_STR_INPUT_H_
