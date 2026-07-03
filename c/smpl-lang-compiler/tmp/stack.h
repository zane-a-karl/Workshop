#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>

typedef struct Node {
	char *data;
	struct Node *below;
} Node;

typedef Node * Stack;

bool
push (Stack *s, char *str);

char *
pop (Stack *s);

#endif // _STACK_H_
