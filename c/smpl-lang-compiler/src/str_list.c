#include "../hdr/str_list.h"

//Assume that `data` is already calloc'd
struct StrNode *
new_str_node (char *data)
{
	struct StrNode *sn = calloc(1, sizeof(struct StrNode));
	sn->data           = data;
	sn->next           = NULL;

	return sn;
}

struct StrList *
new_str_list ()
{
	struct StrList *sl = calloc(1, sizeof(struct StrList));
	sl->head           = NULL;

	return sl;
}

struct StrNode *
deep_copy_str_node (struct StrNode *src)
{
	return new_str_node(deep_copy_str(src->data));
}

struct StrList *
deep_copy_str_list (struct StrList *src)
{
	struct StrList *dst = new_str_list();
	for (struct StrNode *i = src->head; i != NULL; i = i->next) {
		push_str_node(dst, deep_copy_str_node(i));
	}
	return dst;
}

// assume `new_node` already calloc'd
void
push_str_node (struct StrList *sl,
							 struct StrNode *new_node)
{
	struct StrNode *i = sl->head;
	if ( i == NULL ) {
		sl->head = new_node;
	} else {
		while ( i->next != NULL ) {
			i = i->next;
		}
		i->next = new_node;
	}
}

void
free_str_list (struct StrList **sl)
{
	struct StrNode *cur = (*sl)->head;
	struct StrNode *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur=cur->next;
		free(prv->data);
		free(prv);
	}
	free(*sl);
}
