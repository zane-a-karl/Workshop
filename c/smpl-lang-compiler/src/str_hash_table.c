#include "../hdr/str_hash_table.h"


struct StrHashTable *
new_str_hash_table ()
{
	struct StrHashTable *sht;
	sht = calloc(1, sizeof(struct StrHashTable));
	for (int i = 0; i < MAX_NUM_VARS; ++i) {
		sht->entries[i] = NULL;
	}
	return sht;
}

// Assume `name` is already calloc'd
struct StrHashEntry *
new_str_hash_entry (char *name,
										enum StrHashEntryType type)
{
	struct StrHashEntry *she;
	she           = calloc(1, sizeof(struct StrHashEntry));
	she->name     = name;
	she->type     = type;
	she->data_len = 0;
	she->next     = NULL;

	return she;
}

unsigned int
sht_hash (char *name)
{
	int len = strnlen(name, MAX_VAR_NAME_LEN);
	unsigned int hash_value = 0;
	for (int i = 0; i < len; ++i) {
		hash_value += name[i];
		hash_value = (hash_value * name[i]) % MAX_VAR_NAME_LEN;
	}
	return hash_value;
}

bool
sht_insert (struct StrHashTable *sht,
						struct StrHashEntry *she)
{
	if ( she == NULL ) return false;
	int index = sht_hash(she->name);
	she->next = sht->entries[index];
	sht->entries[index] = she;
	/* struct StrHashEntry *itr = sht->entries[index]; */
	/* if ( itr == NULL ) { */
	/* 	sht->entries[index] = she; */
	/* 	return true; */
	/* } */
	/* while (itr->next != NULL) { */
	/* 	itr = itr->next; */
	/* } */
	/* itr->next = she; */
	/* she->next = NULL; */


	return true;
}

struct StrHashEntry *
sht_lookup (struct StrHashTable *sht,
						char                *name)
{
	int index = sht_hash(name);
	struct StrHashEntry *i = sht->entries[index];
	while ( i != NULL &&
					strncmp(i->name, name, MAX_VAR_NAME_LEN) != 0 ) {

		i = i->next;
	}
	return i;
}

struct StrHashEntry *
sht_delete (struct StrHashTable *sht,
						char                *name)
{
  int index = sht_hash(name);
	struct StrHashEntry *i = sht->entries[index];
	struct StrHashEntry *prv = NULL;
	while ( i != NULL &&
					strncmp(i->name, name, MAX_VAR_NAME_LEN) != 0 ) {
		
		prv = i;
		i = i->next;
	}
  	if ( i == NULL) {       //empty list head
		return NULL;
	} else if ( prv == NULL) {//head only list
		sht->entries[index] = i->next;
	} else {                  //str found in list
		prv->next = i->next;
	}
	return i;
}

struct StrHashTable *
deep_copy_sht (struct StrHashTable *src)
{
	struct StrHashTable *dst = new_str_hash_table();
	int i, k;
	struct StrHashEntry *j, *tmp;
	for (i = 0; i < MAX_NUM_VARS; ++i) {
		for (j = src->entries[i]; j != NULL; j = j->next) {
			tmp = new_str_hash_entry(deep_copy_str(j->name), j->type);
			switch (j->type) {
			case DATA:
				tmp->data = calloc(j->data_len, sizeof(int));
				for (k = 0; k < j->data_len; ++k) {
					tmp->data[k] = j->data[k];
				}
				break;
			case NODE:
				tmp->node = j->node;//shallow copy don't free!
				break;
			case OP:
				//tmp->operand = j->operand;
				tmp->operand = deep_copy_operand(j->operand);
				break;
			case INST:
				tmp->instruction = j->instruction;//shallow don't free!
				break;
			case BB:
				tmp->basic_block = j->basic_block;//shallow don't free!
				break;
			default:
				perror("(deep_copy_sht) Unrecognized she type");
				exit(1);
				break;
			}//switch

			sht_insert(dst, tmp);
		}//jfor
	}//ifor
	return dst;
}

// Remember if you used string literals then DO NOT free!
void
free_she (struct StrHashEntry **she)
{
	free((*she)->name);
	(*she)->name = NULL;
	switch ( (*she)->type ) {
	case DATA:
		free((*she)->data);
		(*she)->data = NULL;
		break;
	case NODE:
		//Don't free the node as it will be freed elsewhere
		break;
	case OP:
		//Don't free the op as it will be freed elsewhere
		// Nope I deep copied sometimes...
		// so free this at those time???
		//		free_operand((*she)->operand);
		break;
	case INST:
		//Don't free the instr as it will be freed elsewhere
		break;
	case BB:
		//Don't free the bb as it will be freed elsewhere
		break;		
	}
	free(*she);
	*she = NULL;
}

void
free_sht (struct StrHashTable **sht)
{
	for (int i = 0; i < MAX_NUM_VARS; ++i) {
		struct StrHashEntry *cur = (*sht)->entries[i];
		struct StrHashEntry *prv = NULL;
			while ( cur != NULL ) {
				prv = cur;
				cur = cur->next;
				free_she(&prv);
			}
	}
	free(*sht);
	*sht = NULL;
}

void
print_table (struct StrHashTable *sht)
{
	printf("START\n");
	for (int i = 0; i < MAX_NUM_VARS; ++i) {
		if ( sht->entries[i] == NULL ) {
			printf("\t%i\t---\n", i);
		} else {
			printf("\t%i\t", i);
			struct StrHashEntry *tmp = sht->entries[i];
			while ( tmp != NULL ) {
				printf("%s -> ", tmp->name);
				tmp = tmp->next;
			}
			printf("\n");
		}
	}
	printf("END\n");
}
