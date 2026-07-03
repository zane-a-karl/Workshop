#include "utils-all-modes.h"


/** Converts the ascii str into a bit str of length =
 * (strlen * bitwidth) stored in big endian.
 * Assume bit_str has already been calloc'd.
 */
void
ascii_to_bits (bool **bit_str, // O
							 char *asc_str,
							 int asc_str_len) {

  int bits_per_char = 8;
  for (int i = 0; i < asc_str_len; ++i) {
    for (int j = 0; j < bits_per_char; ++j) {
      (*bit_str)[bits_per_char*i + j] =
				asc_str[i] & (1<<(bits_per_char-j-1));
    }
  }
}

/** Converts the bit str into an ascii str of
 * length = bit_str_len / bits_per_char stored in big endian.
 * Assume ascii_str has already been calloc'd.
 */
void
bits_to_ascii (char **ascii_str, // O
							 bool *bit_str,
							 int bit_str_len) {

  int bits_per_char = 8;
  for (int i = 0; i < bit_str_len; ++i) {
    (*ascii_str)[i/bits_per_char] +=
      bit_str[i] * (1<<(bits_per_char-(i%bits_per_char)-1));
  }
}

/** Prints the 8 bit per char representation of a
 * boolean array. With each row representing a single
 * character.
 */
void
print_bits (bool *arr,
						int len) {

  int rows = floor(len/8);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < 8; ++j) {
      printf("%d", arr[8*i + j]);
    }
    printf("\n");
  }
}

/** Converts a string into its CIPHERMODE enum equivalent
 */
int
set_cipher_mode (char *mode) {
  if (strcmp(mode, "ECB") == 0) {
    return ECB;
#define othermode(m) } else if (strcmp(mode, #m) == 0) { return m;
    othermode(CBC);
    othermode(CFB);
    othermode(OFB);
    othermode(CTR);
#undef othermode
  } else {
    printf("Invalid mode\n");
    assert(0);
  }
}

void
get_num_input_blocks (int *n_blocks, // O
											char *ptxt_filename,
											int block_length) {

  FILE *fin = fopen(ptxt_filename, "r");
  handle_return_FILE_error("Error fopen ptxt_file", fin);
  int ch, ret_val, ptxt_length = 0;
  do {

    ch = fgetc(fin);
    if ( feof(fin) != 0 ) {
      break;
    }
    ret_val = ferror(fin);
    handle_return_int_error("Error ferror ptxt_file",
														ret_val);
    ++ptxt_length;

  } while (1);
  ret_val = fclose(fin);
  handle_return_int_error("Error fclose key_file",
													ret_val);

  (*n_blocks) = ceil((double)ptxt_length / (block_length/8));
}

void
get_ascii_key_length (int *key_length, // O
											char *key_filename) {

  FILE *fin = fopen(key_filename, "r");
  handle_return_FILE_error("Error fopen key_file", fin);
  int ch, ret_val;
  do {

    ch = fgetc(fin);
    if ( feof(fin) != 0 ) {
      break;
    }
    ret_val = ferror(fin);
    handle_return_int_error("Error ferror key_file",
														ret_val);
		if (ch == '\n') {
			break;
		}
    ++(*key_length);

  } while (1);
  ret_val = fclose(fin);
  handle_return_int_error("Error fclose key_file",
													ret_val);
}

void
get_ascii_key (char **key, // O
							 char *key_filename) {

  FILE *fin = fopen(key_filename, "r");
  handle_return_FILE_error("Error fopen key_file", fin);
  int i = 0, ch, ret_val;
  do {

    ch = fgetc(fin);
    if ( feof(fin) != 0 ) {
      break;
    }
    ret_val = ferror(fin);
    handle_return_int_error("Error ferror key_file",
														ret_val);
    (*key)[i++] = ch;

  } while (1);
  ret_val = fclose(fin);
  handle_return_int_error("Error fclose key_file",
													ret_val);
}

int
get_bit_key_len (int asc_key_len) {

	return 8 * asc_key_len;
}

void
handle_return_FILE_error (char *error_str,
													FILE* f) {

  if ( f == NULL ) {
    printf("%s\n", error_str);
    perror(NULL);
    exit(1);
  }
}

void
handle_return_int_error (char *error_str,
												 int ret_val) {

  if ( ret_val != 0 ) {
    printf("%s\n", error_str);
    perror(NULL);
    exit(1);
  }
}
