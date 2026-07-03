#include "../hdr/operand.h"

struct Operand *
new_operand (enum OperandType type,
						 ...)
{
	struct Operand *op = calloc(1, sizeof(struct Operand));
	op->next           = NULL;
	va_list args;
	va_start(args, type);
	op->type = type;
	switch ( type ) {
	case IMMEDIATE:
	case INSTRUCTION:
		op->number = va_arg(args, int);
		break;
	case UNINITVAR:
	case ARGUMENT:
	case LABEL:
	case FN:
		op->name   = deep_copy_str(va_arg(args, char *));
		break;
	case POSSPHI:
		op->op     = va_arg(args, struct Operand *);
		break;
	default:
		perror("(new_operand): Unknown operand type");
		exit(1);
		break;
	}
	va_end(args);

	return op;
}

struct OperandList *
new_operand_list ()
{
	struct OperandList *ol = calloc(1, sizeof(struct OperandList));
	ol->head               = NULL;

	return ol;
}

struct OpBox
new_op_box (struct Operand *op,
						int            *dims)
{
	struct OpBox ob;
	ob.op   = op;
	ob.dims = dims;

	return ob;
}

// assume `new_op` already calloc'd
void
push_operand (struct OperandList *ol,
							struct Operand     *new_op)
{
	struct Operand *o = ol->head;
	if ( o == NULL ) {
		ol->head = new_op;
	} else {
		while ( o->next != NULL ) {
			o = o->next;
		}
		o->next = new_op;
	}
}

struct Operand *
deep_copy_operand (struct Operand *src)
{
	struct Operand *dst;
	if ( src == NULL ) {
		return NULL;
	}
	switch ( src->type ) {
	case IMMEDIATE:
	case INSTRUCTION:
		dst = new_operand(src->type, src->number);
		break;
	case UNINITVAR:
	case ARGUMENT:
	case LABEL:
	case FN:
		dst = new_operand(src->type, src->name);
		break;
	case POSSPHI:
		dst = new_operand(src->type, src->op);
		break;
	default:
		perror("(new_operand): Unknown operand type");
		exit(1);
		break;
	}
	return dst;
}

/**
 * `new_list` will not be NULL, and will have at least one operand.
 */
struct OperandList *
deep_copy_operand_list (struct OperandList *src)
{
	struct OperandList *dst = new_operand_list();
	for (struct Operand *i = src->head; i != NULL; i = i->next) {
		push_operand(dst, deep_copy_operand(i));
	}
	return dst;
}

bool
eq_operands (struct Operand *op1,
						 struct Operand *op2)
{
	if ( op1->type != op2->type ) {
		return false;
	} else {
		int n = MAX_VAR_NAME_LEN;
		switch ( op1->type ) {
		case 	IMMEDIATE:
		case INSTRUCTION:
			return op1->number == op2->number;
		case UNINITVAR:
		case ARGUMENT:
		case LABEL:
		case FN:
			return strncmp(op1->name, op2->name, n);
		case POSSPHI:
			return eq_operands(op1->op, op2->op);
		default:
			perror("(new_operand): Unknown operand type");
			exit(1);
			return false;
		}
	}
}

bool
eq_operand_lists (struct OperandList *l1,
									struct OperandList *l2)
{
	struct Operand *i1 = l1->head;
	struct Operand *i2 = l2->head;
	for (; i1 != NULL && i2 != NULL; i1 = i1->next, i2 = i2->next) {
		if ( eq_operands(i1, i2) ) {
			continue;
		} else {
			return false;
		}
	}
	return true;
}

void
free_operand (struct Operand **op)
{
	switch ( (*op)->type ) {
	case IMMEDIATE:
	case INSTRUCTION:
		break;
	case UNINITVAR:
	case ARGUMENT:
	case LABEL:
	case FN:
		free((*op)->name);
		(*op)->name = NULL;
		break;
	case POSSPHI:
		free_operand(&((*op)->op));
		(*op)->op = NULL;
		break;
	default:
		perror("(new_operand): Unknown operand type");
		exit(1);
		break;
	}
	free(*op);
	*op = NULL;
}

void
free_operand_list (struct OperandList **ol)
{
	struct Operand *cur = (*ol)->head;
	struct Operand *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur = cur->next;
		free_operand(&prv);
	}
	free(*ol);
	*ol = NULL;
}
