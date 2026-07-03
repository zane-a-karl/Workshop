#include "../hdr/var_table.h"

VarNode *
alloc_var_node () {
	VarNode *vn = (VarNode *)calloc(1, sizeof(VarNode));
	vn->key = (char *)calloc(MAX_VAR_NAME_LEN, sizeof(char));
	vn->val = -1;
	vn->next = NULL;
	return vn;
}

void
init_var_node_key (VarNode **vn,
									 char *var_name) {

	for (int i = 0; i < MAX_VAR_NAME_LEN; ++i) {
		(*vn)->key[i] = var_name[i];
	}
}

VarTable *
alloc_var_table () {

	VarTable *vt = (VarTable *)calloc(1, sizeof(VarTable));
	vt->first = NULL;
	return vt;
}

//TODO: make generic linked list to work with this and
// the TokenList
void
push_var_node (VarTable *vt,
							 VarNode *var_name) {

	VarNode *new_vn = alloc_var_node();
	init_var_node_key(&new_vn, var_name);
	VarNode *i = vt->first;
	if ( i == NULL ) {
		vt->first = new_var;
	} else {
		while ( i->next != NULL ) {
			i=i->next;
		}
		i->next = new_var;
	}
}

int
lookup (VarTable *vt,
				char *key) {

	for (VarNode *i = vt->first; i->next != NULL; i=i->next) {
		if ( strncmp(key, i->key, MAX_VAR_NAME_LEN) == 0 ) {
			return i->val;
		}
	}
	return -2;
}

void
print_var_table (VarTable *vt) {

	int idx = 0;
  for (VarNode *i = vt->first; i != NULL; i=i->next) {
    printf("Var %d: key=\"%s\", val=%i\n",
           idx++,
           i->key,
           i->val);
  }
}

void
free_var_table (VarTable *vt) {

	VarNode *cur = vt->first;
	VarNode *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur=cur->next;
		free(prv);
	}
}
