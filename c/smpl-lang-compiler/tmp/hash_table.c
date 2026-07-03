#include "../hdr/hash_table.h"

unsigned int
hash (char *name) {
	int length = strnlen(name, MAX_NAME_LEN);
	unsigned int hash_value = 0;
	for (int i = 0; i < length; ++i) {
		hash_value += name[i];
		hash_value = (hash_value * name[i]) % MAX_NUM_VARS; 
	}
	return hash_value;
}

void
init_hash_table (Variable *(*vt[MAX_NUM_VARS])) {
	for (int i = 0; i < MAX_NUM_VARS; ++i) {
		vt[i] = NULL;
	}
}

bool
hash_table_insert (Variable *vt[MAX_NUM_VARS],
									 Variable *v) {
	if (v == NULL) {
		return false;
	}
	int index = hash(v->name);
	v->next = vt[index];
	vt[index] = v;
	return true;
}

Variable *
lookup (Variable **vt,
				char *name) {
	int index = hash(name);
	Variable *tmp = vt[index];
	while (tmp != NULL && strncmp(tmp->name, name, MAX_NAME_LEN) != 0) {
		tmp = tmp->next;
	}
	return tmp;
}

Variable *
hash_table_delete (Variable *(*vt[MAX_NUM_VARS]),
									 char *name) {
	int index = hash(name);
	Variable *tmp = (*vt)[index];
	Variable *prv = NULL;
	while (tmp != NULL && strncmp(tmp->name, name, MAX_NAME_LEN) != 0) {
		prv = tmp;
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		return NULL;
	}
	if (prv == NULL) {
		(*vt)[index] = tmp->next; // delete the current head
	} else {
		prv->next = tmp->next; // delete from the middle
	}
	return tmp;
}

/* void */
/* print_table () { */
/* 	printf("START\n"); */
/* 	for (int i = 0; i < MAX_NUM_VARS; ++i) { */
/* 		if (hash_table[i] == NULL) { */
/* 			printf("\t%i\t---\n", i); */
/* 		} else { */
/* 			printf("\t%i\t ", i); */
/* 			Variable *tmp = hash_table[i]; */
/* 			while (tmp != NULL) { */
/* 				printf("%s - ", tmp->name); */
/* 				tmp = tmp->next; */
/* 			} */
/* 			printf("\n"); */
/* 		} */
/* 	} */
/* 	printf("END\n"); */
/* } */
