#ifndef _DLX_UTILS_H_
#define _DLX_UTILS_H_

#include <stdbool.h>

#define NUM_REGISTERS 1<<5
#define NUM_OPCODES 1<<6
typedef struct Register {
	int data;
	bool unassigned;
} Register;
Register memory[NUM_REGISTERS];

static const char *op_codes[] = {
	"ADD" , // F2 0
	"SUB" , // F2 1
	"MUL" , // F2 2
	"DIV" , // F2 3
	"MOD" , // F2 4
	"CMP" , // F2 5
	"UNK1", // F2 0
	"UNK2", // F2 0
	"OR"  , // F2 8
	"AND" , // F2 9
	"BIC" , // F2 10
	"XOR" , // F2 11
	"LSH" , // F2 12
	"ASH" , // F2 13
	"CHK" , // F2 14
	"UNK3", // F2 0
	"ADDI", // F1 16
	"SUBI", // F1 17
	"MULI", // F1 18
	"DIVI", // F1 19
	"MODI", // F1 20
	"CMPI", // F1 21
	"UNK4", // F1 0
	"UNK5", // F1 0
	"ORI" , // F1 24
	"ANDI", // F1 25
	"BICI", // F1 26
	"XORI", // F1 27
	"LSHI", // F1 28
	"ASHI", // F1 29
	"CHKI", // F1 30
	"UNK6",
	"LDW" , // F1 32
	"LDX" , // F2 33
	"POP" , // F1 34
	"UNK7",
	"STW" , // F1 36
	"STX" , // F2 37
	"PSH" , // F1 38
	"UNK8",
	"BEQ" , // F1 40
	"BNE" , // F1 41
	"BLT" , // F1 42
	"BGE" , // F1 43
	"BLE" , // F1 44
	"BGT" , // F1 45
	"BSR" , // F1 46
	"UNK9",
	"JSR" , // F3 48
	"RET" , // F2 49
	"RDD" , // F2 50
	"WRD" , // F2 51
	"WRH" , // F2 52
	"WRL"   // F1 53
};

#endif // _DLX_UTILS_H_
