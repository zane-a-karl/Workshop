#ifndef _STR_LIST_H_
#define _STR_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_VAR_NAME_LEN
#define MAX_VAR_NAME_LEN 1<<4 //16
#endif//MAX_VAR_NAME_LEN

typedef struct StrListNode {
	char *data;
	struct StrListNode *next;
} StrListNode;

typedef struct StrList {
	StrListNode *head;
} StrList;

StrList *
init_str_list ();//calloc

StrListNode *
init_str_list_node (void *data);//calloc

void
next_str_list_node (StrListNode **sln);

void
push_str_list (StrList **sl,
							 StrList *new_sl);

void
push_str_list_node (StrList **sl,
										StrListNode *new_node);

void
push_str_list_data (StrList **sl,
										char *new_data);

void
print_str_list (StrList *sl);

StrList *
deep_copy_str_list (StrList *src_sl);

void
free_str_list (StrList **sl);

#endif//_STR_LIST_H_
