#include "encrypt-file-input.h"


bool
encrypt_using_ecb (char *ptxt_filename,
									 char *key_filename,
									 int b_len) {

  bool status = false;

  char *asc_key;
  bool *bit_key;
  int asc_key_len;
  int bit_key_len;
  int num_input_blocks;

  printf("########### KEY ###########\n");
	// TODO: At some point don't read the file twice
	// dynamically resize the char array as necessary to fit
	// the given key (will be good realloc practice).
  get_ascii_key_length(&asc_key_len, key_filename);
  asc_key = (char *)calloc(asc_key_len, sizeof *asc_key);
  get_ascii_key(&asc_key, key_filename);
	printf("The asc key len is %d\n", asc_key_len);
  bit_key_len = get_bit_key_len(asc_key_len);
  bit_key = (bool *)calloc(bit_key_len, sizeof *bit_key);
  for (int i = 0; i < bit_key_len; ++i) {
    asc_key[i] -= 48;
  }
  ascii_to_bits(&bit_key, asc_key, asc_key_len);
  print_bits(bit_key, bit_key_len);
  printf("###########################\n");

  printf("####### BLOCK WIDTH #######\n");
  printf("%14d\n", b_len);
  printf("###########################\n");

  get_num_input_blocks(&num_input_blocks,
											 ptxt_filename,
											 b_len);
  CipherInfo CI;
  CI.key = bit_key;
  CI.p_file = ptxt_filename;
  CI.c_file = "Files/Ctxts/ECB.txt";
	CI.r_file = "Files/Recons/ECB.txt";
  CI.b_len = b_len;
  CI.p_len = 0;
	CI.c_len = 0;
  CI.k_len = bit_key_len;
  CI.n_blocks = num_input_blocks;

  encrypt(CI);
  /* threaded_encrypt(CI); */

	decrypt(CI);
	/* threaded_decrypt(CI); */

  status = true;

  free(asc_key);
  free(bit_key);

  return status;
}

void
encrypt (CipherInfo CI) {

	//TODO: post-process the output ciphertext into b64 to
	// make it readable.
	// You just won't care if there is any "=" padding b/c
	// you will remove this padding upon decryption. Note
	// that this is a per-block encryption so the cipher-
	// text will look like <block>[=][=] over and over
	// again.
	//
	// This will require you write a separate
	// decryption function that pre-processes the
	// ciphertext input by decoding from b64 to asc as
	// opposed to the encryption function that
	// post-processes by encoding the ciphertext output
	// from asc to b64.
	//
	// You might be able to get away with just a single
	// encryption function if you always pre-and post process
	// But then you'd have to post process again on output
	// so it might not be worth it.
	//
	// NOTE: this will require you make some new functions
	// for utilities and require cleanup which you needed
	// anyway. You ought to also improve your Makefile at
	// the end and make a macro to make error handling
	// easier for files.
  FILE *fin = fopen(CI.p_file, "r");
  handle_return_FILE_error("Error fopen p_file",
													 fin);
  FILE *fout = fopen(CI.c_file, "w");
  handle_return_FILE_error("Error fopen o_file",
													 fout);
  bool *ptxt_block_bits = (bool *)calloc(CI.b_len,
																				 sizeof (bool));
  bool *ctxt_block_bits = (bool *)calloc(CI.b_len,
																				 sizeof (bool));
  char *ptxt_block_asc = (char *)calloc(CI.b_len/8,
																				sizeof (char));
	char *ctxt_block_asc = (char *)calloc(CI.b_len/8,
																				sizeof (char));
	Base64 ctxt_block_b64;
	ctxt_block_b64.len = get_base64_char_len(CI.b_len/8);
  ctxt_block_b64.str = (char *)calloc(ctxt_block_b64.len,
																			sizeof (char));
	ctxt_block_b64.pad = 0;

  int n_read, n_write, ret_val;
  do {
    n_read = fread(ptxt_block_asc,
									 sizeof (char),
									 CI.b_len/8,
									 fin);
    if ( n_read == 0 && feof(fin) != 0 ) {
      break;
    }
    ret_val = ferror(fin);
    handle_return_int_error("Error ferror p_file",
														ret_val);
    CI.p_len += n_read;
		// TODO: THINK ABOUT THE LOGIC OF THIS. IT SEEMS LIKE
		// A LOT OF CONVERSIONS. IS IT POSSIBLE TO SKIP A
		// FEW OF THESE STEPS? MAYBE GO DIRECTLY FROM THE
		// ENCRYPT_BLOCK FN AND IT'S OUTPUT BITS TO BASE_64?
    ascii_to_bits(&ptxt_block_bits,
									ptxt_block_asc,
									CI.b_len/8);
    encrypt_block(&ctxt_block_bits,
									ptxt_block_bits,
									CI);
		bits_to_ascii(&ctxt_block_asc,
									ctxt_block_bits,
									CI.b_len);
		ascii_to_base64(&ctxt_block_b64,
										ctxt_block_asc);

    n_write = fwrite(ctxt_block_b64.str,
										 sizeof (char),
										 strlen(ctxt_block_b64.str),
										 fout);
    if ( feof(fout) != 0 ) {
      break;
    }
    ret_val = ferror(fout);
    handle_return_int_error("Error ferror o_file",
														ret_val);
    memset(ptxt_block_bits,    0,
					 CI.b_len * (sizeof *ptxt_block_bits));
    memset(ctxt_block_bits,    0,
					 CI.b_len * (sizeof *ctxt_block_bits));
    memset(ptxt_block_asc,     0,
					 CI.b_len/8 * (sizeof *ptxt_block_asc));
		memset(ctxt_block_asc,     0,
					 CI.b_len/8 * (sizeof *ctxt_block_asc));
    memset(ctxt_block_b64.str, 0,
					 ctxt_block_b64.len * sizeof (char));
  } while (1);

  ret_val = fclose(fin);
  handle_return_int_error("Error fclose p_file",
													ret_val);
  ret_val = fclose(fout);
  handle_return_int_error("Error fclose c_file",
													ret_val);
  free(ptxt_block_bits);
  free(ctxt_block_bits);
  free(ptxt_block_asc);
	free(ctxt_block_asc);
  free(ctxt_block_b64.str);
}

/** Performs an ECB (XOR) encryption on the bits in
 * "p_block" using the "CI.key" and storing it
 * in the "c_block"
 */
void
encrypt_block (bool **c_block,
							 bool *p_block,
							 CipherInfo CI) {

  for ( int i = 0; i < CI.b_len; ++i ) {
    (*c_block)[i] = p_block[i];
    if ( CI.k_len >= CI.b_len ) {
      (*c_block)[i] ^= CI.key[i % CI.b_len];
    } else {
      (*c_block)[i] ^= CI.key[i % CI.k_len];
    }
  }
}

/* TODO: WRITE A DECRYPT FN, NOT THREADED. MAYBE ADD
	 A SEPARATE FILE FOR DECRYPTION functions.*/
void
decrypt (CipherInfo CI) {

	FILE *fin = fopen(CI.c_file, "r");
  handle_return_FILE_error("Error fopen c_file",
													 fin);
  FILE *fout = fopen(CI.r_file, "w");
  handle_return_FILE_error("Error fopen r_file",
													 fout);
  bool *ctxt_block_bits = (bool *)calloc(CI.b_len,
																				 sizeof (bool));
  bool *rcns_block_bits = (bool *)calloc(CI.b_len,
																				 sizeof (bool));
  char *rcns_block_asc = (char *)calloc(CI.b_len/8,
																				sizeof (char));
	Base64 ctxt_block_b64;
	ctxt_block_b64.len = get_base64_char_len(CI.b_len/8);
  ctxt_block_b64.str = (char *)calloc(ctxt_block_b64.len,
																			sizeof (char));
	ctxt_block_b64.pad = 0;
  int n_read, n_write, ret_val;
  do {

		// Read the b64 char block length not the asc one.
    n_read = fread(ctxt_block_b64.str,
									 sizeof (char),
									 ctxt_block_b64.len,
									 fin);
    if ( n_read == 0 && feof(fin) != 0 ) {
      break;
    }
    ret_val = ferror(fin);
    handle_return_int_error("Error ferror c_file",
														ret_val);
    CI.c_len += n_read;
		// This is the preprocessing step // Keep this cmt
		// TODO: Something goes wrong here when you run
		// with a block length of 8, but 24 works just fine
		// let's see if you can figure out what happened
		base64_to_bits(&ctxt_block_bits,
									 ctxt_block_b64,
									 CI.b_len);
		// encryption is same as decryption
    encrypt_block(&rcns_block_bits,
									ctxt_block_bits,
									CI);
		bits_to_ascii(&rcns_block_asc,
									ctxt_block_bits,
									CI.b_len);
    n_write = fwrite(rcns_block_asc,
										 sizeof (char),
										 strlen(rcns_block_asc),
										 fout);
    if ( feof(fout) != 0 ) {
      break;
    }
    ret_val = ferror(fout);
    handle_return_int_error("Error ferror r_file",
														ret_val);
    memset(ctxt_block_bits,    0,
					 CI.b_len * (sizeof *ctxt_block_bits));
    memset(rcns_block_bits,    0,
					 CI.b_len * (sizeof *rcns_block_bits));
    memset(rcns_block_asc,     0,
					 CI.b_len/8 * (sizeof *rcns_block_asc));
    memset(ctxt_block_b64.str, 0,
					 ctxt_block_b64.len * sizeof (char));
  } while (1);

  ret_val = fclose(fin);
  handle_return_int_error("Error fclose c_file",
													ret_val);
  ret_val = fclose(fout);
  handle_return_int_error("Error fclose r_file",
													ret_val);
  free(ctxt_block_bits);
  free(rcns_block_bits);
  free(rcns_block_asc);
  free(ctxt_block_b64.str);
}

void
threaded_encrypt (CipherInfo CI) {

  n_enc_threads = 4;
  int ret_val = 0;
  pthread_t threads[n_enc_threads];
  pthread_t watch_thread;
  pthread_attr_t attr;
  ThreadData TD[n_enc_threads];

  FILE *fout = fopen(CI.c_file, "w");
  handle_return_FILE_error("Error fopen c_file",
													 fout);
  ret_val = pthread_mutex_init(&write_mutex, NULL);
  handle_return_int_error("Error mutex_init write_mutex",
													ret_val);
  ret_val = pthread_cond_init(&sequential_cond, NULL);
  handle_return_int_error("Error cond_init seq_cond",
													ret_val);
  curr_block_idx = 0;
  thread_is_pending = (bool *)calloc(n_enc_threads,
																		 sizeof (bool));
  thread_is_pending[0] = true;

  /* For portability, explicitly create threads in
		 joinable state */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,
															PTHREAD_CREATE_JOINABLE);

  for (int i = 0; i < n_enc_threads; ++i) {

    TD[i].ptxt_block_bits = (bool *)calloc(CI.b_len,
																			 sizeof (bool));
    TD[i].ctxt_block_bits = (bool *)calloc(CI.b_len,
																				sizeof (bool));
    TD[i].ptxt_block_asc = (char *)calloc(CI.b_len/8,
																			sizeof (char));
		TD[i].ctxt_block_b64.len =
			get_base64_char_len(CI.b_len/8);
		TD[i].ctxt_block_b64.str =
			(char *)calloc(TD[i].ctxt_block_b64.len,
										 sizeof (char));
		TD[i].ctxt_block_b64.pad = 0;
    TD[i].idx = i;
    TD[i].CI = CI;
    TD[i].fout = fout;
    ret_val = pthread_create(&threads[i],
														 &attr,
														 threaded_encrypt_block,
														 (void *)&TD[i]);
    handle_return_int_error("Error pthread_create",
														ret_val);
  }
  ret_val = pthread_create(&watch_thread,
													 &attr,
													 watch_next_block_idx,
													 (void *)&CI);
  handle_return_int_error("Error pthread_create",
													ret_val);

  for (int i = 0; i < n_enc_threads; ++i) {
    ret_val = pthread_join(threads[i], NULL);
    handle_return_int_error("Error pthread_join",
														ret_val);
  }

  ret_val = fclose(fout);
  handle_return_int_error("Error fclose o_file",
													ret_val);

  for (int i = 0; i < n_enc_threads; ++i) {
    free(TD[i].ptxt_block_bits);
    free(TD[i].ctxt_block_bits);
    free(TD[i].ptxt_block_asc);
    free(TD[i].ctxt_block_b64.str);
  }

  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&write_mutex);
  pthread_cond_destroy(&sequential_cond);
}

void *
threaded_encrypt_block (void *thread_args) {

  ThreadData *TD = (ThreadData *)thread_args;

  FILE *fin = fopen(TD->CI.p_file, "r");
  handle_return_FILE_error("Error fopen p_file", fin);

  int n_read, n_write, ret_val;

  long offset =  TD->idx * TD->CI.b_len/8;
  ret_val = fseek(fin, offset, SEEK_SET);
  handle_return_int_error("Error fseek p_file SET",
													ret_val);
  do {

    n_read = fread(TD->ptxt_block_asc,
									 sizeof (char),
									 TD->CI.b_len/8,
									 fin);

    if ( n_read == 0 && feof(fin) != 0 ) {
      break;
    }
    ret_val = ferror(fin);
    handle_return_int_error("Error ferror i_file",
														ret_val);
		ascii_to_bits(&TD->ptxt_block_bits,
									TD->ptxt_block_asc,
									strlen(TD->ptxt_block_asc));
    encrypt_block(&TD->ctxt_block_bits,
									TD->ptxt_block_bits,
									TD->CI);
		//    bits_to_ascii(&TD->ctxt_block, TD->ctxt_block_bits, TD->CI.b_len);
		bits_to_base64(&TD->ctxt_block_b64,
									 TD->ctxt_block_bits,
									 TD->CI.b_len);
    thread_is_pending[TD->idx] = true;

    ret_val = pthread_mutex_lock(&write_mutex);
    handle_return_int_error("Error mutex_lock write_mtx",
														ret_val);

    while ( TD->idx != (curr_block_idx % n_enc_threads) ) {
      ret_val = pthread_cond_wait(&sequential_cond,
																	&write_mutex);
      handle_return_int_error("Error cond_wait seq_cond",
															ret_val);
    }

    /* fwrite() will write exactly the number of bytes
			 you give it. No more, no less. So if you haven't
			 filled a later portion of the array fwrite() will
			 just add gibberish to it.*/
		printf("block idx #%d is %s\n",
					 TD->idx,
					 TD->ctxt_block_b64.str);
		/* Something is going wrong with the output of the
			 base64 encoding. Need to look in to how things
			 are getting messed up. I think it has to do with
			 a mix up in conversions. */
    n_write = fwrite(TD->ctxt_block_b64.str,
										 sizeof (char),
										 strlen(TD->ctxt_block_b64.str), // Very Important
										 TD->fout);
    /* printf("Writing block %d...\n", curr_block_idx); */

    thread_is_pending[TD->idx] = false;
    ++curr_block_idx;

    ret_val = pthread_mutex_unlock(&write_mutex);
    handle_return_int_error("Error mutex_unlock write_mtx",
														ret_val);

    if ( feof(TD->fout) != 0 ) {
      break;
    }
    ret_val = ferror(TD->fout);
    handle_return_int_error("Error ferror c_file",
														ret_val);
    memset(TD->ptxt_block_bits,
					 0,
					 TD->CI.b_len * sizeof (bool));
    memset(TD->ctxt_block_bits,
					 0,
					 TD->CI.b_len * sizeof (bool));
    memset(TD->ptxt_block_asc,
					 0,
					 TD->CI.b_len * sizeof (char));
    memset(TD->ctxt_block_b64.str,
					 0,
					 TD->CI.b_len * sizeof (char));
    offset = (n_enc_threads-1) * (TD->CI.b_len/8);
    ret_val = fseek(fin, offset, SEEK_CUR);
    handle_return_int_error("Error fseek p_file CUR",
														ret_val);
  } while (1);

  /* printf("FINISHED Thread #%d!!!\n", TD->idx); */

  ret_val = fclose(fin);
  handle_return_int_error("Error fclose p_file",
													ret_val);
  return NULL;
}

void *
watch_next_block_idx (void *thread_args) {

  int ret_val = 0;
  CipherInfo *CI = (CipherInfo *)thread_args;

  while (1) {
    if (curr_block_idx == CI->n_blocks) {
      return NULL;
    }
    ret_val = pthread_mutex_lock(&write_mutex);
    handle_return_int_error("Error mutex_lock watch_mtx",
														ret_val);

    if ( thread_is_pending[curr_block_idx % n_enc_threads] ) {
      pthread_cond_broadcast(&sequential_cond);
    }
    ret_val = pthread_mutex_unlock(&write_mutex);
    handle_return_int_error("Error mutex_unlock watch_mtx",
														ret_val);
  }
}

