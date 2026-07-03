#ifndef _VAR_TABLE_H_
#define _VAR_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_VAR_NAME_LEN
#define MAX_VAR_NAME_LEN 1<<4 //16
#endif//MAX_VAR_NAME_LEN

typedef struct VarNode {
	char *key; // the variable name.
	int val;   // the register location it was stored in.
	struct VarNode *next;
} VarNode;

typedef struct VarTable {
	VarNode *first;
} VarTable;

VarNode *
alloc_var_node ();

void
init_var_node_key (VarNode **vn,
									 char *var_name);

VarTable *
alloc_var_table ();

void
push_var_node (VarTable *vt,
							 VarNode *new_var);

int
lookup (VarTable *vt,
				char *key);

void
print_var_table (VarTable *vt);

void
free_var_table (VarTable *vt);

#endif//_VAR_TABLE_H_
