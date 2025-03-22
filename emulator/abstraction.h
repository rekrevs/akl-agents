/* $Id: abstraction.h,v 1.4 1994/03/30 19:47:25 jm Exp $ */


extern method absmethod;
extern predicate *apply_def;

#define NewAbstraction(Abs, Def, Andb)\
{\
  MakeGenericX((Abs), abstraction,\
	       (sizeof(abstraction) + (sizeof(Term) * (Def)->ext)),\
	       &absmethod);\
  (Abs)->def = (Def);\
  (Abs)->env = (envid*)&((Andb)->env); \
}
  

#define IsAbs(Abs) (IsGEN(Abs) && (Gen(Abs)->method == &absmethod))

#define GenAbs(g)	((abstraction*)(g))
#define Abs(x)		GenAbs(Gen(x))

#define AbsDef(A)	(A)->def
#define AbsArity(A)	(A)->def->arity
#define AbsExt(A)	(A)->def->ext

#define AbsArgRef(A,N)  &((A)->arg[N])

typedef struct abstraction {
  method	*method;
  envid		*env;
  predicate 	*def;
  Term		arg[ANY];
} abstraction;

extern void initialize_abstraction();


