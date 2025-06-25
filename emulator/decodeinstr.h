/* $Id: decodeinstr.h,v 1.3 1994/03/22 13:29:41 jm Exp $ */

/* These macros turn the instruction definitions into decoding macros */

/* Basic hooks: */

#define CodeWord(n,x)		(x) = GetCodeWord(n)
#define BeginCodeWord(n)	{ code codeword = *(pc+(n))
#define CodeField(x,i,n)	(x) = GetCodeField(codeword,i,n)
#define EndCodeWord(dummy)	}
#define StartInstruction(dummy)
#define EndInstruction(n)	FetchOpCode(n); pc += (n)

/* Special operand types: */

#define LabelWord(n,x)		(x) = (code*) GetCodeWord(n)
#define IndexWord(n,x)		(x) = (indx) GetCodeWord(n)
#define IndexField(x,i)		CodeField(x,i*INDEX_BITS,INDEX_BITS)
#define NumberField(x,i)	CodeField(x,i*INDEX_BITS,INDEX_BITS)
#define FunctorWord(n,f)	(f) = (Functor) GetCodeWord(n)
#define TermWord(n,x)		(x) = code_to_term(GetCodeWord(n))
#define PredWord(n,x)		(x) = (predicate*) GetCodeWord(n)

/* Auxiliary macros: */

#define GetCodeField(w,i,n)	(((w) >> (i)) & ((1L << (n))-1))
#define GetCodeWord(n)		(*(pc+(n)))
#define GetOpCode(ptr)		((opcode) (*(ptr)))
#define FetchOpCode(n)		op = GetOpCode(pc+(n))
