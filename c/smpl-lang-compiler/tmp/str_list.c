#include "../hdr/str_list.h"

StrList *
init_str_list () {

	StrList *sl = (StrList *)calloc(1, sizeof(StrList));
	sl->head = NULL;
	return sl;
}

StrListNode *
init_str_list_node (char *data) {//calloc

	StrListNode *sln;
	sln = (StrListNode *)calloc(1, sizeof(StrListNode));
	sln->data = (char *)calloc(MAX_VAR_NAME_LEN,
														 sizeof(char));
	for (int i = 0; i < MAX_VAR_NAME_LEN; ++i) {
		sln->data[i] = data[i];
	}
	sln->next = NULL;
	return sln;
}

void
next_str_list_node (StrListNode **sln) {

	(*sln) = (*sln)->next;
}

void
push_str_list (StrList **sl,
							 StrList *new_sl) {

	StrListNode *i = (*sl)->head;
	while (i->next != NULL) {
		i = i->next;
	}
	i = new_sl->head;
	while (i->next != NULL) {
		push_str_list_data(sl, i->data);
		i = i->next;
	}
}

// assume new_node already calloc'd
void
push_str_list_node (StrList **sl,
										StrListNode *new_node) {

	StrListNode *i = (*sl)->head;
	if ( i == NULL ) {
		(*sl)->head = new_node;
	} else {
		while ( i->next != NULL ) {
			i = i->next;
		}
		i->next = new_node;
	}
}

void
push_str_list_data (StrList **sl,
										char *new_data) {

	StrListNode *new_node = init_list_node(new_data);
	push_str_list_node(sl, new_node);
}

void
print_str_list (StrList *sl) {

	int idx = 0;
  for (StrListNode *i = sl->head; i != NULL; i=i->next) {
    printf("node %d: data=\"%s\"\n", idx++, i->data);
  }
}

StrList *
deep_copy_str_list (StrList *src_sl) {//calloc

	StrList *dst_sl = init_str_list();
	for (StrListNode *i = src_sl->head; i != NULL; i=i->next) {
		push_str_list_data(&dst_sl, i->data);
	}
}

void
free_str_list (StrList **sl) {

	StrListNode *cur = (*sl)->head;
	StrListNode *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur=cur->next;
		free(prv->data);
		free(prv);
	}
}
