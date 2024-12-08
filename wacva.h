#ifndef WACVA_HEADER
#define WACVA_HEADER

#include <stdint.h>

#define unextend(c) (c)<<4
#define extend(c) (c)
#define sp(c) (c)

typedef uint32_t p_t;

#define ulng_step 4
#define ilng_step 4
#define uwrd_step 2
#define iwrd_step 2
#define ubyt_step 1
#define ibyt_step 1

enum op_t{
	MOV=unextend(0X0),
	PUS=unextend(0X1),
	POP=unextend(0X2),
	
	PLU=unextend(0X5),
	SUB=unextend(0X6),
	MUL=unextend(0X7),
	DIV=unextend(0X8),
	EQU=unextend(0X9),
	CMP=unextend(0XA),
	XOR=unextend(0xB),
	ORB=unextend(0xC),
	AND=unextend(0XD),
	NOT=unextend(0XE),
	
	JMP=extend(0XF0),
	JIF=extend(0XF1),
	JNF=extend(0XF2),
	MVB=extend(0XF3)
};

#endif
