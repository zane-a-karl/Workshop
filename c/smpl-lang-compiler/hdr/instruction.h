#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "../hdr/forward_declarations.h"

#include "../hdr/operand.h"
#include "../hdr/utils.h"//for `deep_copy_str()`

#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

struct Instruction {
	char               *name;
	struct OperandList *ops;
	int                 number;//I.e. the instruction number
	bool                produces_output;
	struct Instruction *dominator;//Instr that dominates this one
	struct Instruction *next;
};

struct InstructionList {
	struct Instruction *head;
};

char *
create_label_from_instr (struct Instruction *i);

struct Instruction *
new_instruction (char   *name,
								 bool    produces_output,
								 int     n_args,
								 va_list args);

struct InstructionList *
new_instruction_list ();

void
push_instruction (struct InstructionList *il,
									struct Instruction     *new_i);

/* struct Instruction * */
/* deep_copy_instruction (struct Instruction * src); */

void
delete_instruction_from_sht (struct StrHashEntry *ent,
														 struct Instruction *to_delete);

void
delete_instruction (struct InstructionList *il,
										struct Instruction     *instr);

char *
dominance_class (struct Instruction *instr);

struct Instruction *
find_dominating_identical (struct Instruction *instr);

void
free_instruction (struct Instruction **instr);

void
free_instruction_list (struct InstructionList **il);

#endif//_INSTRUCTION_H_
