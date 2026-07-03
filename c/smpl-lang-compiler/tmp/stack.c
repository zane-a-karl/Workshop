#include "../hdr/stack.h"

bool
push (Stack *s, char *str) {
	Node *new_node = malloc(sizeof node);
	if (new_node == NULL) {
		return false;
	}
	new_node->data = str;
	new_node->below = *s;
	*s = new_node;
	return true;
}

char *
pop (Stack *s) {
	if (*s == NULL) {
		return "THE STACK IS EMPTY";
	}
	char *popped_data = (*s)->data;
	Node *tmp = *s;
	*s = (*s)->below;
	free(tmp);
	return popped_data;
}
