/* $Id: config.h,v 1.29 1994/03/28 09:08:24 jm Exp $ */

/* Macros used to allocate and initialize terms on the heap */

#define MakeFloat(I, value) \
{ \
  NewFloat(I); \
  (I)->val = (value); \
  (I)->tag2 = FLT_TAG2; \
}

#define MakeList(L, Ab)\
{\
  NewList((L)); \
  InitStructEnv(L, (envid*)&((Ab)->env)); \
}

#define MakeStruct(S, F, Ab)\
{\
  NewStructure(S, FnkArity(F));\
  InitStructEnv(S, (envid*)&((Ab)->env)); \
  (S)->functor = (F); \
}

#define MakeVariable(V,Ab) \
{\
   NewReference(V); \
   InitReference(V,Ab);\
}

#define MakeGvainfo(PTR,TYPE,METHODS,ANDBOX) \
{\
  register gvainfo *ZZZZ; \
  NewGeneric(PTR,TYPE); \
  ZZZZ = (gvainfo *)(PTR); \
  (ZZZZ)->method = (METHODS); \
  (ZZZZ)->env = (envid *)&((ANDBOX)->env); \
}

#define MakeSvainfo(S,Ab) \
{\
   NewSvainfo(S); \
   InitSvainfo(S,Ab);\
}

#define MakeGeneric(PTR,TYPE,METHODS) \
{\
  register Generic ZZZZ; \
  NewGeneric(PTR,TYPE); \
  ZZZZ = (Generic)(PTR); \
  (ZZZZ)->method = (METHODS); \
}


#define MakeGenericX(PTR,TYPE,SIZE,METHODS) \
{\
  register generic *ZZZZ; \
  NewXGeneric(PTR,TYPE,SIZE); \
  ZZZZ = (generic *)(PTR); \
  (ZZZZ)->method = (METHODS); \
}

#define MakeBignum(I, value) \
{ \
  SaveHeapRegisters(); \
  (I) = make_bignum(value); \
  RestoreHeapRegisters(); \
}


/* Versions of the above that make tagged versions (Terms) */

#define MakeSmallIntegerTerm(X,value) \
{\
  (X) = MakeSmallNum(value); \
}

#define MakeIntegerTerm(X,value) \
{\
   register sword NNN = (value);\
   if (IntIsSmall(NNN)) {\
     (X) = MakeSmallNum(NNN);\
   } else {\
     register Bignum ZZZ;\
     MakeBignum(ZZZ,NNN);\
     (X) = TagBig(ZZZ);\
   }\
}

#define MakeFloatTerm(X,value) \
{\
   register Fp ZZZ;		\
   MakeFloat(ZZZ,value);	\
   (X) = TagFlt(ZZZ);		\
}

#define MakeListTerm(X,Ab) \
{\
   register List ZZZ;		\
   MakeList(ZZZ,Ab);		\
   (X) = TagLst(ZZZ);		\
}

#define MakeStructTerm(X,F,Ab) \
{\
   register Structure ZZZ;	\
     MakeStruct(ZZZ,F,Ab);\
   (X) = TagStr(ZZZ);		\
}



#define MakeVariableTerm(X,Ab) \
{\
   register Reference ZZZ;	\
   MakeVariable(ZZZ,Ab);	\
   (X) = TagRef(ZZZ);		\
}


#define MakeCvaTerm(X,G) \
{\
   register Reference ZZZ;	\
   NewReference(ZZZ);		\
   RefTerm(ZZZ) = TagGva(G);	\
   (X) = TagRef(ZZZ);		\
}


#define MakeGenericTerm(PTR,TYPE,METHODS) \
{\
  register TYPE *ZZZ;			\
  MakeGeneric(ZZZ,TYPE,METHODS);	\
  (PTR) = TagGen(ZZZ);			\
}

/* To make a new variable while building a structure or list */

#ifdef  INDIRECTVARS

#define InitVariable(Trmp,Ab)\
{\
  register Reference Z;\
  MakeVariable(Z,Ab);\
  *(Trmp) = TagRef(Z);\
  }

#else

#define InitVariable(Trmp, Ab) InitReference(Trmp,Ab);

#endif


/* Macros used to create and initialize tree structures */

#ifdef TRACE
#define MakeAndbox(Andb,Chb)\
{\
  NewAndbox(Andb);\
  Andb->env = NULL;\
  Andb->status = STABLE;\
  Andb->tried = NULL;\
  Andb->cont = NULL;\
  Andb->constr = NULL;\
  Andb->unify = NULL;\
  Andb->father = Chb;\
  Andb->previous = NULL;\
  Andb->next = NULL;\
  Andb->id = identity++;\
  Andb->trace = NOTRACE;\
}
#else
#define MakeAndbox(Andb,Chb)\
{\
  NewAndbox(Andb);\
  Andb->env = NULL;\
  Andb->status = STABLE;\
  Andb->tried = NULL;\
  Andb->cont = NULL;\
  Andb->constr = NULL;\
  Andb->unify = NULL;\
  Andb->father = Chb;\
  Andb->previous = NULL;\
  Andb->next = NULL;\
}
#endif

#define InsertAndbox(Andb,Prev,Chb)\
{\
  Andb->previous = Prev;\
  if(Prev != NULL)\
     Prev->next = Andb;\
  else\
     (Chb)->tried = Andb;\
}

#ifdef TRACE
#define MakeAndCont(Andb,Cont,Next,Ysize) \
{\
  int i;\
  andcont *cont;\
  NewAndCont(cont, Ysize);\
  cont->ysize = (Ysize);\
  cont->trace= NOTRACE;\
  cont->choice_cont=Andb->father->cont ;\
  cont->def=Andb->father->def;\
  cont->clno=Andb->clno;\
  cont->next = (Next);\
  cont->label = (Cont);\
  for(i = 0; i != (Ysize); i ++) {\
    cont->yreg[i] = NullTerm;\
  }\
  (Andb)->cont = cont;\
}
#else
#define MakeAndCont(Andb,Cont,Next,Ysize) \
{\
  int i;\
  andcont *cont;\
  NewAndCont(cont, Ysize);\
  cont->ysize = (Ysize);\
  cont->next = (Next);\
  cont->label = (Cont);\
  for(i = 0; i != (Ysize); i ++) {\
    cont->yreg[i] = NullTerm;\
  }\
  (Andb)->cont = cont;\
}
#endif


#ifdef TRACE
#define MakeChoicebox(Chb,Def,Andb)\
{\
  NewChoicebox(Chb);\
  Chb->def = Def;\
  Chb->father = Andb;\
  Chb->tried = NULL;\
  Chb->next_clno = 1;\
  Chb->trace = NOTRACE;\
  Chb->id = identity++;\
  Chb->type = 0;\
}
#else
#define MakeChoicebox(Chb,Def,Andb)\
{\
  NewChoicebox(Chb);\
  Chb->def = Def;\
  Chb->father = Andb;\
  Chb->tried = NULL;\
  Chb->type = 0;\
}
#endif


#ifdef TRACE
#define MakeRootChoicebox(Chb,Def,Andb)\
{\
  NewChoicebox(Chb);\
  Chb->def = Def;\
  Chb->father = Andb;\
  Chb->tried = NULL;\
  Chb->trace=NOTRACE;\
  Chb->next_clno = 1;\
  Chb->type = 0;\
}
#else
#define MakeRootChoicebox(Chb,Def,Andb) MakeChoicebox(Chb,Def,Andb)
#endif

#define MakeChoiceCont(Chb,Label,Areg,Arity)\
{\
  int i;\
  choicecont *cont;\
  NewChoiceCont(cont,Arity);\
  (Chb)->cont = cont;\
  cont->arity = (Arity);\
  cont->instr.label = (Label);\
  for(i = 0; i < (int)(Arity); i++) cont->arg[i] = (Areg)[i];\
}


#define MakeChoiceCinfoCont(Chb,Cinfo,Areg,Arity)\
{\
  int i;\
  choicecont *cont;\
  NewChoiceCont(cont,Arity);\
  (Chb)->cont = cont;\
  cont->arity = (Arity);\
  cont->instr.cinfo = Cinfo;\
  for(i = 0; i < (int)(Arity); i++) cont->arg[i] = (Areg)[i];\
}

#define MakeWake(V,Andb)\
{   gvainfo *ZZZg = RefGva(V);\
    if (GvaIsSva(ZZZg)) {\
	suspension *susp;\
	svainfo *ZZZs = (svainfo*)(ZZZg);\
	DerefGvaEnv(ZZZg);\
	NewSuspension(susp);\
	susp->type = ANDB;\
	susp->suspended.andb = Andb;\
	susp->next = SvaSusp(ZZZs);\
	SvaSusp(ZZZs) = susp;\
	Mark(Andb,Home(SvaEnv(ZZZs)));\
    } else {\
	if (ZZZg->method->makewake != NULL) {\
	  ZZZg->method->makewake(ZZZg,Andb);\
	}\
    }\
}

#define MakeRecall(V,Chb)\
{   gvainfo *ZZZg = RefGva(V);\
    if (GvaIsSva(ZZZg)) {\
	suspension *susp;\
	svainfo *ZZZs = (svainfo*)(ZZZg);\
	DerefGvaEnv(ZZZg);\
	NewSuspension(susp);\
	susp->type = CHB;\
	susp->suspended.chb = Chb;\
	susp->next = SvaSusp(ZZZs);\
	SvaSusp(ZZZs) = susp;\
	Mark((Chb)->father,Home(SvaEnv(ZZZs)));\
    } else {\
	if (ZZZg->method->makerecall != NULL) {\
	  ZZZg->method->makerecall(ZZZg,Chb);\
	}\
    }\
}

#define MakeUnifier(Unifier,Arg1,Arg2,Andb)\
{\
  NewUnifier(Unifier);\
  (Unifier)->next = (Andb)->unify;\
  (Andb)->unify = Unifier;\
  (Unifier)->arg1 = Arg1;\
  (Unifier)->arg2 = Arg2;\
}

#define MakeTuple(Tuple,Size) NewTuple(Tuple, Size)

/* Macros used to manipulate constraints and suspensions */


#define LinkUnifier(A,U) \
{\
   (U)->next = (A)->unify;\
   (A)->unify = (U);\
}


#define BindUnifier(U) \
{\
   Reference arg1 = Ref((U)->arg1);\
   Term arg2 = (U)->arg2;\
   (U)->arg2 = RefTerm(arg1);\
   RefTerm(arg1) = arg2;\
}

#define UnBindUnifier(U) \
{\
   Reference arg1 = Ref((U)->arg1);\
   Term arg2 = (U)->arg2;\
   (U)->arg2 = RefTerm(arg1);\
   RefTerm(arg1) = arg2;\
}

#define   UndoUnifier(Andb) \
{\
   unifier *unif;\
   for(unif = (Andb)->unify; unif != NULL; unif = unif->next) {\
	UnBindUnifier(unif);\
   }\
}

#define   UndoConstraints(Andb) \
{\
   constraint *constr;\
     for(constr = Andb->constr; constr != NULL; constr = constr->next) {\
       constrtable *meth = constr->method; \
       if (meth != NULL) \
         (meth->deinstall)(constr);\
   }\
}

#define LinkConstr(A,C) \
{\
   (C)->next = (A)->constr;\
   (A)->constr = (C);\
}


#define UnlinkSusp(V,P,S) \
{\
    if((P) == NULL)\
       SvaSusp(GvaSva(RefGva(V))) = (S)->next; \
    else\
       (P)->next = (S)->next;\
}

/* Macros used to manipulate and-boxes */

#define PromoteAndbox(P,A) (P)->env = (envid*)&((A)->env);

#define UnlinkAndb(A) \
{\
    if((A)->next != NULL)\
	(A)->next->previous = (A)->previous;\
    if((A)->previous != NULL)\
	(A)->previous->next = (A)->next;\
    else\
	(A)->father->tried = (A)->next;\
}

#define Kill(Andb)\
{\
   (Andb)->status = DEAD;\
}

#define KillLeft(Andb)\
{ andbox *sibl;\
  for(sibl = Andb->next; sibl != NULL; sibl = sibl->next)\
      sibl->status = DEAD;\
  Andb->next = NULL;\
}

#define KillAll(Chb)\
{ register andbox *atmp;\
  for(atmp = Chb->tried ; atmp != NULL ; atmp = atmp->next)\
      atmp->status = DEAD;\
}


#define Mark(From,Dest) \
{\
  andbox *To, *Tempa;\
  To = (Dest);\
  for(Tempa = (From); Tempa != To; Tempa = Tempa->father->father)\
    Tempa->status = UNSTABLE;\
}

/* Macros used to manipulate choice-boxes */

#define InsertChoicebox(Chb,Insert,Andb)\
{\
  Chb->previous = Insert;\
  if(Insert != NULL) {\
    Chb->next = Insert->next;\
    if(Insert->next != NULL)\
      Insert->next->previous = Chb;\
    insert->next = Chb;\
  } else {\
    Chb->next = Andb->tried;\
    if(Andb->tried != NULL)\
      Andb->tried->previous = Chb;\
    Andb->tried = Chb;\
  }\
}

#define RemoveChoicebox(Chb,Insert,Andb)\
{\
   if(Chb->next != NULL) \
    Chb->next->previous = Chb->previous;\
  if(Chb->previous != NULL)\
    Chb->previous->next = Chb->next;\
  else\
    Andb->tried = Chb->next;\
  Insert = Chb->previous;\
}


#define UnlinkChb(C) \
{\
    if((C)->next != NULL)\
	(C)->next->previous = (C)->previous;\
    if((C)->previous != NULL)\
	(C)->previous->next = (C)->next;\
    else\
	(C)->father->tried = (C)->next;\
}

#define RestoreAreg(Chb,Areg)\
{\
  register int i;\
  for(i = 0 ; i != (Chb)->cont->arity ; i++)\
    (Areg)[i] = (Chb)->cont->arg[i];\
}

/* In TRACE mode the choice continuation must remain.
 * The label is used to indicate an empty continuation.
 * This is done to make the original arguments available
 * for the tracer.
 */

#ifdef TRACE
#define RemoveChoiceCont(Chb)\
{\
  (Chb)->cont->instr.label = NULL;\
}

#define MakeEmptyChoiceCont(Chb,Areg,Arity)\
{\
  int i;\
  choicecont *cont;\
  NewChoiceCont(cont,(Arity));\
  (Chb)->cont = cont;\
  cont->arity = (Arity);\
  cont->instr.label = NULL;\
  for(i = 0; i < (int)(Arity); i++) cont->arg[i] = (Areg)[i];\
}

#define NonEmptyChoiceCont(C)  ((C)->cont->instr.label != NULL)

#else

#define RemoveChoiceCont(Chb)\
{\
  (Chb)->cont = NULL;\
}

#define MakeEmptyChoiceCont(Chb,Areg,Arity)\
{\
   (Chb)->cont = NULL;\
}

#define NonEmptyChoiceCont(C)  ((C)->cont != NULL)

#endif

#define SetChoiceContLabel(Chb,Label)\
{\
  (Chb)->cont->instr.label = (Label);\
}

/* This macro will be used once we move to suspended goals */

#define IsFlatGoal(suspended) \
    ((suspended.choicebox->cont->label) >= codespace && \
     (suspended.choicebox->cont->label) <= topofcodespace)


/* Functions exported from config.c to allocate and initialize
 * terms on the constant area.
 */

extern Atom 	make_atom();
extern Functor 	make_functor();
extern Fp 	make_constant_float();

extern Term	integer_copy_to_constspace();

/* Support functions for number conversions */
 
extern Term	atoi_term PROTO((char *name));
extern void	term_itoa PROTO((Term i, int base, char *buf));
extern long	integer_sizeinbase PROTO((Term i, int base));
extern double	get_float PROTO((Term x));
extern Term	integer_from_float PROTO((Term x));

/* Hmm, these are declared in term.h
 *
 *	extern sword	get_bignum_integer();
 *	extern bool	is_machine_integer();
 *	extern uword	get_bignum_unsigned();
 *	extern bool	is_machine_unsigned();
 *	extern int	bignum_compare();
 */

/* These live in bignum.c */

extern Bignum	make_bignum();
extern int	bignum_print();
