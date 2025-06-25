/* $Id: encodeinstr.h,v 1.6 1994/03/22 13:29:44 jm Exp $ */

/* These macros turn the instruction definitions into encoding macros */

/* Basic hooks: */

#define CodeWord(n,x)		PutCodeWord(n,x)
#define BeginCodeWord(n)	{ code asmword = 0
#define CodeField(x,i,n)	asmword += MakeCodeField(x,i,n)
#define EndCodeWord(n)		PutCodeWord(n,asmword); \
				}
#define StartInstruction(op)	CodeWord(0,(code)EnumToCode(op));
#define EndInstruction(n)	pc += (n)

/* Special operand types: */

#define LabelWord(n,x) \
	if ((x) >= MAXLABELS) label_overflow(); \
	CodeWord(n,labelrefs[x]); \
	labelrefs[x] = (char*)(pc+(n)) - (char*)codebase

#define IndexWord(n,x)		CodeWord(n,IndexToOffset(x))
#define IndexField(x,i) \
	CodeField(IndexToOffset(x),i*INDEX_BITS,INDEX_BITS)
#define NumberField(x,i) \
	CodeField(x,i*INDEX_BITS,INDEX_BITS)

#define FunctorWord(n,f)	CodeWord(n,(code)(f))
#define TermWord(n,x)		CodeWord(n,term_to_code(x))
#define PredWord(n,p)		CodeWord(n,(code)(p))

/* Auxiliary macros: */

#define MakeCodeField(x,i,n)	(((x) & ((1L << (n))-1)) << (i))
#define PutCodeWord(n,x)	*(pc+(n)) = (x)

#define ConstFunctor(fname,arity)	store_functor(Atm(fname),arity)
#define ConstAtom(name)		TagAtm(store_atom(name))
#define ConstInt(name)		store_integer_term(name)
#define ConstFlt(name)		TagFlt(store_float(atof(name)))
#define GetPred(fname,arity)	get_predicate(Atm(fname),arity)
#define GetAbstraction(label)   get_abstraction(label)

#define pc codecurrent
