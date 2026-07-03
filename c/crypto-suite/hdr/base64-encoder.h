#ifndef _BASE64_ENCODER_H_
#define _BASE64_ENCODER_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* base64_char_set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

typedef struct Base64 {
  char *str;
  int len;
  int pad;
} Base64;

void
ascii_to_base64 (Base64 *b64, // O
								 char *asc_str);

void
get_char_set_idx (int *char_set_idx, // O
									int b64_char);

void
base64_to_bits (bool **bit_str, // O
								Base64 b64,
								int bit_str_len);

void
bits_to_base64 (Base64 *b64, // O
								bool *bit_str,
								int bit_str_len);

void
base64_to_ascii (char **asc, // O
								 Base64 b64,
								 int asc_str_len);

void
init_base64 (Base64 *b64,
						 int asc_len);

int
get_base64_char_len (int asc_len);

int
get_base64_bit_len (Base64 b64);

#endif /* _BASE64_ENCODER_H_ */
