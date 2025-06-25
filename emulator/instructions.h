/* $Id: instructions.h,v 1.18 1993/11/22 10:45:38 bd Exp $ */

/* Compile with -DNOTHREAD to disable threaded code */

#ifndef NOTHREAD
#if defined(__GNUC__) && (__GNUC__ >= 2)
#define THREADED_CODE
#endif
#endif

/* The enum type that defines the opcodes is read from a file.
 * Be sure to #define Item correctly first.
 */

#ifdef Item
#undef Item
#endif

#define Item(x)	x,

typedef enum {
#include "opcodes.h"
  COMMA_HERE_NOT_ACCEPTABLE		/* Silly, but necessary */
} enum_opcode;


#ifdef THREADED_CODE
typedef void *address;

extern address *instr_label_table;
extern enum_opcode code_to_enum();

typedef address opcode;

#define EnumToCode(c)	(instr_label_table[c])
#define CodeToEnum(c)	code_to_enum(c)
#define SwitchTo(x)	goto *(address)(x);
#define LABELNAME(x)	LABEL_ ## x
#define CaseLabel(x)	LABELNAME(x)

#define NextOp()	SwitchTo(op)

#else

typedef enum_opcode opcode;

#define EnumToCode(c)	(c)
#define CodeToEnum(c)	(c)
#define SwitchTo(x)	switch(x)
#define CaseLabel(x)	case x
#define NextOp()	goto read_instr_dispatch

#endif

#define NextReadOp()	{ write_mode = 0; NextOp(); }
#define NextWriteOp()	{ write_mode = 1; NextOp(); }

#define WriteModeDispatch(write_instruction) \
{\
    if (write_mode) goto write_instruction;\
}

#define WriteModeLabel(label)	label:



typedef u32 indx;

#define Skip(offset) {\
  pc += (offset);\
  NextOp();\
}

#define Branch(offset) {\
  LabelWord(offset,pc);\
  FetchOpCode(0);\
  NextOp();\
}

