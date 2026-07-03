#ifndef _UTILS_H_
#define _UTILS_H_

#include "../hdr/constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
check_fopen (FILE* f);

char *
deep_copy_str (char *src);

#endif //_UTILS_H_
