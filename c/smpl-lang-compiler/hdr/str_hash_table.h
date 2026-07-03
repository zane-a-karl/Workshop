#ifndef _STR_HASH_TABLE_H_
#define _STR_HASH_TABLE_H_

#include "../hdr/constants.h"
#include "../hdr/utils.h"
#include "../hdr/operand.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum StrHashEntryType {DATA, NODE, OP, INST, BB};

struct StrHashEntry {
	char                 *name;
	enum StrHashEntryType type;
	union {
		int                *data;
		struct AstNode     *node;
		struct Operand     *operand;
		struct Instruction *instruction;
		struct BasicBlock  *basic_block;
	};
	int                   data_len;
	struct StrHashEntry  *next;
};

struct StrHashTable {
	struct StrHashEntry *entries[MAX_NUM_VARS];
};

struct StrHashTable *
new_str_hash_table ();

struct StrHashEntry *
new_str_hash_entry (char *name,
										enum StrHashEntryType type);

unsigned int
sht_hash (char *name);

bool
sht_insert (struct StrHashTable *sht,
						struct StrHashEntry *she);

struct StrHashEntry *
sht_lookup (struct StrHashTable *sht,
						char                *name);

struct StrHashEntry *
sht_delete (struct StrHashTable *sht,
						char                *name);

struct StrHashTable *
deep_copy_sht (struct StrHashTable *src);

void
free_she (struct StrHashEntry **she);

void
free_sht (struct StrHashTable **sht);

void
print_table (struct StrHashTable *sht);

#endif//_STR_HASH_TABLE_H_
