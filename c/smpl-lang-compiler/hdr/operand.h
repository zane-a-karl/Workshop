#ifndef _OPERAND_H_
#define _OPERAND_H_

#include "../hdr/constants.h"
#include "../hdr/compiler_ctx.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

enum OperandType {
	IMMEDIATE,
	UNINITVAR,
	INSTRUCTION,
	ARGUMENT,
	LABEL,
	FN,
	POSSPHI
};

struct Operand {
	enum OperandType  type;
	union {
		int             number;
		char           *name;
		struct Operand *op;
	};
	struct Operand   *next;
};

struct OperandList {
	struct Operand *head;
};

struct OpBox {
	struct Operand *op;
	int            *dims;
};

struct Operand *
new_operand (enum OperandType type,
						 ...);

struct OperandList *
new_operand_list ();

struct OpBox
new_op_box (struct Operand *op,
						int            *dims);

void
push_operand (struct OperandList *ol,
							struct Operand     *new_op);

struct Operand *
deep_copy_operand (struct Operand *src);

struct OperandList *
deep_copy_operand_list (struct OperandList *src);
												

bool
eq_operands (struct Operand *op1,
						 struct Operand *op2);

bool
eq_operand_lists (struct OperandList *l1,
									struct OperandList *l2);

void
free_operand (struct Operand **op);

void
free_operand_list (struct OperandList **ol);

#endif//_OPERAND_H_
