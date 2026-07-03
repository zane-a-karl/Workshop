#ifndef _INT_LIST_H_
#define _INT_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct IntListNode {
	int data;
	struct IntListNode *next;
} IntListNode;

typedef struct IntList {
	IntListNode *head;
} IntList;

IntList *
init_int_list ();

IntListNode *
init_int_list_node (int data);

void
next_int_list_node (IntListNode **iln);

void
push_int_list (IntList **il,
							 IntList *new_il);

void
push_int_list_node (IntList **il,
										IntListNode *new_node);

void
push_int_list_data (IntList **il,
										int new_data);

IntList *
deep_copy_int_list (IntList *src_il);//calloc

void
print_int_list (IntList *il);

void
free_int_list (IntList **il);

#endif//_INT_LIST_H_
