#ifndef _STR_LIST_H_
#define _STR_LIST_H_

#include "../hdr/constants.h"
#include "../hdr/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct StrNode {
	char           *data;
	struct StrNode *next;
};

struct StrList {
	struct StrNode *head;
};

struct StrNode *
new_str_node (char *data);

struct StrList *
new_str_list ();

struct StrNode *
deep_copy_str_node (struct StrNode *src);

struct StrList *
deep_copy_str_list (struct StrList *src);

void
push_str_node (struct StrList *sl,
							 struct StrNode *new_node);

void
free_str_list (struct StrList **sl);

#endif//_STR_LIST_H_
