#ifndef _DLX_H_
#define _DLX_H_

#include <stdbool.h>

/* #define NUM_REGISTERS 1<<5 */
/* #define NUM_OPCODES 1<<6 */
/* typedef struct Register { */
/* 	int data; */
/* 	bool unassigned; */
/* } Register; */
/* Register memory[NUM_REGISTERS]; */

typedef enum InstrType {
	ADD ,// 0: f2(a b c)
	SUB ,// 1: f2(a b c)
	MUL ,// 2: f2(a b c)
	DIV ,// 3: f2(a b c)
	MOD ,// 4: f2(a b c)
	CMP ,// 5: f2(a b c)
	UNK1,
	UNK2,
	OR  ,// 8: f2(a b c)
	AND ,// 9: f2(a b c)
	BIC ,//10: f2(a b c)
	XOR ,//11: f2(a b c)
	LSH ,//12: f2(a b c)
	ASH ,//13: f2(a b c)
	CHK ,//14: f2(a   c)
	UNK3,
	ADDI,//16: f1(a b c)
	SUBI,//17: f1(a b c)
	MULI,//18: f1(a b c)
	DIVI,//19: f1(a b c)
	MODI,//20: f1(a b c)
	CMPI,//21: f1(a b c)
	UNK4,
	UNK5,
	ORI ,//24: f1(a b c)
	ANDI,//25: f1(a b c)
	BICI,//26: f1(a b c)
	XORI,//27: f1(a b c)
	LSHI,//28: f1(a b c)
	ASHI,//29: f1(a b c)
	CHKI,//30: f1(a   c)
	UNK6,
	LDW ,//32: f1(a b c)
	LDX ,//33: f2(a b c)
	POP ,//34: f1(a b c)
	UNK7,
	STW ,//36: f1(a b c)
	STX ,//37: f2(a b c)
	PSH ,//38: f1(a b c)
	UNK8,
	BEQ ,//40: f1(a   c)
	BNE ,//41: f1(a   c)
	BLT ,//42: f1(a   c)
	BGE ,//43: f1(a   c)
	BLE ,//44: f1(a   c)
	BGT ,//45: f1(a   c)
	BSR ,//46: f1(    c)
	UNK9,
	JSR ,//48: f3(    c)
	RET ,//49: f2(    c)
	RDD ,//50: f2(a    )
	WRD ,//51: f2(  b  )
	WRH ,//52: f2(  b  )
	WRL  //53: f1(     )
} InstrType;

typedef struct Instruction {
	int ssa_idx;
	char *raw_dlx_instr;
	InstrType op;
	// TODO: ADD A Result?
	// Factor struct? to return
	// from smpl_factor/term/
	// expression? Maybe have
	// the 'Result' be a
	// separate thing and then
	// you can just pillage it
	// to fill the instruction
	// out!!!
	Result a;
	Result b;
	Result c;
	struct Instruction *next;
} Instruction;

#endif//_DLX_H_
