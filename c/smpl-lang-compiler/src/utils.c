#include "../hdr/utils.h"

void
check_fopen (FILE* f)
{
  if ( f == NULL ) {
    perror("Couldn't open file");
    exit(1);
  }
}

char *
deep_copy_str (char *src)
{
	char *dst = calloc(MAX_VAR_NAME_LEN, sizeof(char));
	strlcpy(dst, src, MAX_VAR_NAME_LEN);
	/* for (int i = 0; i < MAX_VAR_NAME_LEN; ++i) { */
	/* 	dst[i] = src[i]; */
	/* } */
	return dst;
}
