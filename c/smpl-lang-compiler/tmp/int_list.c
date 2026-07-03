#include "../hdr/int_list.h"

IntList*
init_int_list () {

	IntList *il = calloc(1, sizeof(IntList));
	il->head = NULL;
	return il;
}

IntListNode *
init_int_list_node (int data) {

	IntListNode *iln;
	iln = calloc(1, sizeof(IntListNode));
	iln->data = data;
	iln->next = NULL;
	return iln;
}

void
next_int_list_node (IntListNode **iln) {

	(*iln) = (*iln)->next;
}

void
push_int_list (IntList **il,
							 IntList *new_il) {

	IntListNode *i = (*il)->head;
	while (i->next != NULL) {
		i = i->next;
	}
	i = new_il->head;
	while (i->next != NULL) {
		push_int_list_data(il, i->data);
		i = i->next;
	}
}

// assume new_node already calloc'd
void
push_int_list_node (IntList **il,
										IntListNode *new_node) {
	
	IntListNode *i = (*il)->head;
	if ( i == NULL ) {
		(*il)->head = new_node;
	} else {
		while ( i->next != NULL ) {
			i = i->next;
		}
		i->next = new_node;
	}
}

void
push_int_list_data (IntList **il,
										int new_data) {
	
	IntListNode *new_node = init_int_list_node(new_data);
	push_int_list_node(il, new_node);
}

IntList *
deep_copy_int_list (IntList *src_il) {//calloc

	IntList *dst_il = init_int_list();
	for (IntListNode *i = src_il->head; i != NULL; i=i->next) {
		push_int_list_data(&dst_il, i->data);
	}
	return dst_il;
}

void
print_int_list (IntList *il) {

	int idx = 0;
  for (IntListNode *i = il->head; i != NULL; i=i->next) {

    printf("node %d: data=%i\n", idx++, i->data);
  }
}

void
free_int_list (IntList **il) {

	IntListNode *cur = (*il)->head;
	IntListNode *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur=cur->next;
		free(prv);
	}
}
