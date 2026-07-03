#include "base64-encoder.h"

/** Converts the ascii str into a b64 str of length =
 * 4 * ceil(asc_str_len/3), b/c 3 ascii chars will
 * yield 24 bytes of info (at 8 bits per ascii char).
 * But 24 bytes maps to 4 b64 chars (at 6 bits per b64
 * char). The inclusion of ceil() is to account for
 * padding at the end of the b64 str should the length
 * of the ascii string not be a multiple of 3.
 * Assume b64_str has already been calloc'd.
 */
void
ascii_to_base64 (Base64 *b64, // O
								 char *asc_str) {

  int i, j;
  int asc_str_len = strlen(asc_str);
  int triple; // Bits of 3 chars concat together.
  int char_cnt = 0;
  int triple_bit_len = 0;
  int padding = 0; // i.e. # of '='s.
  int char_set_idx = 0;
  int b64_idx = 0;
  for (i = 0; i < asc_str_len; i+=3) {

    triple = 0;
    char_cnt = 0;
    for (j = i; j < asc_str_len && j < i+3; ++j) {

      triple <<= 8;
      triple |= asc_str[j];
      ++char_cnt;
    }

    triple_bit_len = char_cnt * 8;
    padding = triple_bit_len  % 3;
    for (j = triple_bit_len; j > 0; j-=6) {

      /* 63 base2 is 111111. */
      if (j >= 6) {
				char_set_idx = (triple >> (j-6)) & 63;
      } else {
				char_set_idx = (triple << (6-j)) & 63;
      }
      b64->str[b64_idx++] = base64_char_set[char_set_idx];
    }
  }
  for (i = 0; i < padding; ++i) {
    b64->str[b64_idx++] = '=';
  }
  b64->pad = padding;
}

/** Returns the idx of the char b64_char within
 * the global variable base64_char_set.
 */
void
get_char_set_idx (int *char_set_idx, // O
									int b64_char) {

  for (int i = 0; i < 64; ++i) {
    if (b64_char == base64_char_set[i]) {
      *char_set_idx = i;
      break;
    }
  }
}

/**
 * We will always assume that a bit representation encodes
 * b64 chars and not ascii chars. This will simplify the
 * analysis and the fn names.
 */
void
base64_to_bits (bool **bit_str, // O
								Base64 b64,
								int bit_str_len) {

  int bits_per_char = 6;
  int idx = 0, char_set_idx = 0;
  for (int i = 0; i < b64.len; ++i) {

    if (b64.str[i] == '=') {
      break;
    }
    get_char_set_idx(&char_set_idx, b64.str[i]);
    for (int j = 0; j < bits_per_char; ++j) {

      idx = bits_per_char*i + j;
      if (idx >= bit_str_len) {
				perror("Trying to write past the end of the bit str\n");
				exit(0);
      }
			// TODO: this fails for bit_str_len != n*24 forall n
			// need to test in isolation in /tests folder
      (*bit_str)[idx] = (char_set_idx>>(abs(5-j))) & 1;
    }
  }
}


/**
 * We will always assume that a bit representation encodes b64 chars
 * and not ascii chars. This will simplify the analysis and the fn
 * names.
 */
void
bits_to_base64 (Base64 *b64, // O
								bool *bit_str,
								int bit_str_len) {

  int char_set_idx = 0;
  int char_cnt = 0;
  int b64_idx = 0;
  for (int i = 0; i < bit_str_len; i += 6) {

    char_set_idx = 0;
    char_cnt = 0;
    for (int j = i; j < bit_str_len && j < i+6; ++j) {

      char_set_idx <<= 1;
      char_set_idx |= bit_str[j];
      ++char_cnt;
    }
    b64->str[b64_idx++] = base64_char_set[char_set_idx];
  }
  for (int i = 0; i < b64->pad; ++i) {
    b64->str[b64_idx++] = '=';
  }
}

/**
 * converts a base64 encoded string into its ascii
 * equivalent.
 */
void
base64_to_ascii (char **asc, // O
								 Base64 b64,
								 int asc_str_len) {

  int i, j;
  int quartet = 0;
  int char_cnt = 0;
  int quartet_bit_len = 0;
  int asc_idx = 0;
  int char_set_idx = 0;
  int asc_char = 0;
  for (i = 0; i < b64.len-b64.pad; i+=4) {

    quartet = 0;
    char_cnt = 0;
    for (j = i; j < b64.len-b64.pad && j < i+4; ++j) {

      quartet <<= 8;
      get_char_set_idx(&char_set_idx, b64.str[j]);
      quartet |= (char_set_idx<<2);
      ++char_cnt;
      quartet >>= 2;
    }

    quartet_bit_len = char_cnt * 6;
    for (j = quartet_bit_len; j > 0; j-=8) {

      /* 255 base2 is 11111111. */
      if (j >= 8) {
				asc_char = (quartet >> (j-8)) & 255;
      } else {
				break;
      }
      if (asc_idx < asc_str_len) {
				(*asc)[asc_idx++] = asc_char;
      } else {
				perror("We tried to write past the end of the asc str");
				exit(0);
      }
    }
  }
}

void
init_base64 (Base64 *b64,
						 int asc_len) {

  b64->len = get_base64_char_len(asc_len);
  b64->str = (char *)calloc(b64->len, sizeof *(b64->str));
  b64->pad = 0;
}

int
get_base64_char_len (int asc_len) {

  return 4*ceil((double)asc_len/3);
}

int
get_base64_bit_len (Base64 b64) {

  return 6 * (b64.len - b64.pad);
}
