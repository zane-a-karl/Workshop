#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_NUM_VARS = 1<<8; //256
const int MAX_NAME_LEN = 1<<4; //16
typedef struct Variable {
	char name[MAX_NAME_LEN];
	struct Variable *next;
} Variable;

unsigned int
hash (char *name);

void
init_hash_table (Variable *(*vt[MAX_NUM_VARS]));

bool
hash_table_insert (Variable *vt[MAX_NUM_VARS],
									 Variable *v);

Variable *
lookup (Variable **vt,
				char *name);

Variable *
hash_table_delete (Variable *(*vt[MAX_NUM_VARS]),
									 char *name);

void
print_table ();

#endif //_HASH_TABLE_H_
