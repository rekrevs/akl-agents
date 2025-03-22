/* $Id: storage.h,v 1.45 1994/05/06 13:55:37 secher Exp $ */

#include "functor.h"

#define WordAlign(x) (((x) + (WORDALIGNMENT-1)) & ~((uword) (WORDALIGNMENT-1)))

#ifndef OptionalWordAlign
#define OptionalWordAlign(x)	x
#endif

#define IsCnst(I)	((((heap *) I) < constend) && \
			 (((heap *) I) > constspace))

/* allocates in the heap area */

#define heapcurrent	glob_heapcurrent
#define heapend		glob_heapend
#define SaveHeapRegisters()
#define RestoreHeapRegisters()

#define SaveHeap(B,H) {\
    (H) = heapcurrent; \
    (B) = current_block; \
}

#define RestoreHeap(B,H) {\
    if (current_block != (B)) {\
      restore_heap_block(B); \
      heapend = current_block->end; \
    }\
    heapcurrent = (H); \
}
      
#define SMALL_SIZE	((size_t)0)

#define NEW(ret, type) \
{ \
    (ret) = (type *) heapcurrent; \
    heapcurrent += sizeof(type); \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),sizeof(type)); \
	RestoreHeapRegisters(); \
    	(ret) = (type *) heapcurrent; \
    	heapcurrent += sizeof(type); \
    }\
}


#define NEWX(ret, type, size) \
{ \
    (ret) = (type *) heapcurrent; \
    heapcurrent += (sizeof(type) * (size)); \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),sizeof(type) * (size)); \
	RestoreHeapRegisters(); \
    	(ret) = (type *) heapcurrent; \
    	heapcurrent += (sizeof(type) * (size)); \
    }\
}

#define NewReference(ret)\
{\
    (ret) = (Reference) heapcurrent; \
    heapcurrent += RefSize; \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (Reference) heapcurrent; \
    	heapcurrent += RefSize; \
    }\
}

#define NewSvainfo(ret)\
{\
    (ret) = (svainfo *) heapcurrent; \
    heapcurrent += SvaSize; \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (svainfo *) heapcurrent; \
    	heapcurrent += SvaSize; \
    }\
}

#define NewFloat(ret)\
{\
    if((uword)heapcurrent % sizeof(double) != 0) {\
       heapcurrent += sizeof(double) - (uword)heapcurrent % sizeof(double);\
    }\
    (ret) = (Fp) heapcurrent; \
    heapcurrent += FltSize; \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (Fp) heapcurrent; \
    	heapcurrent += FltSize; \
    }\
}

#define NewList(ret)\
{\
    (ret) = (list *) (heapcurrent + ENV_OFFSET); \
    heapcurrent += LstSize; \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (list *) (heapcurrent + ENV_OFFSET); \
    	heapcurrent += LstSize; \
    }\
}

#define NewStructure(ret,arity)\
{\
    (ret) = (structure *) (heapcurrent + ENV_OFFSET); \
    heapcurrent += StrSize(arity);\
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),StrSize(arity)); \
	RestoreHeapRegisters(); \
    	(ret) = (structure *) (heapcurrent + ENV_OFFSET); \
    	heapcurrent += StrSize(arity); \
    }\
}

#define NewGeneric(PTR,TYPE) \
{ \
  NEW(PTR,TYPE); \
}

#define NewXGeneric(PTR,TYPE,SIZE) \
{ \
  NEWX(PTR,TYPE,SIZE); \
}

#define NewAndbox(ret)\
{\
    (ret) = (andbox *) heapcurrent; \
    heapcurrent += sizeof(andbox); \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (andbox *) heapcurrent; \
    	heapcurrent += sizeof(andbox); \
    }\
}


#define NewChoicebox(ret)\
{\
    (ret) = (choicebox *) heapcurrent; \
      heapcurrent += sizeof(choicebox);\
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (choicebox*) heapcurrent; \
    	heapcurrent += sizeof(choicebox); \
    }\
}

#define NewChoiceCont(ret,arity)\
{\
    (ret) = (choicecont *) heapcurrent; \
    heapcurrent += ChoiceContSize(arity);\
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),ChoiceContSize(arity)); \
	RestoreHeapRegisters(); \
    	(ret) = (choicecont*) heapcurrent; \
    	heapcurrent += ChoiceContSize(arity);\
    }\
}

#define NewAndCont(ret,arity)\
{\
    (ret) = (andcont *) heapcurrent; \
    heapcurrent += AndContSize(arity);\
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),AndContSize(arity)); \
	RestoreHeapRegisters(); \
    	(ret) = (andcont *) heapcurrent; \
    	heapcurrent += AndContSize(arity);\
    }\
}

#define NewUnifier(ret)\
{\
    (ret) = (unifier *) heapcurrent; \
    heapcurrent += UnifierSize;\
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (unifier *) heapcurrent; \
    	heapcurrent += UnifierSize; \
    }\
}  

#define NewConstraint(ret,arity)\
{\
    (ret) = (constraint *) heapcurrent; \
    heapcurrent += ConstraintSize(arity);\
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (constraint *) heapcurrent; \
    	heapcurrent += ConstraintSize(arity); \
    }\
}

#define NewTuple(ret,size)\
{ \
    (ret) = (Term*)heapcurrent; \
    heapcurrent += (sizeof(Term)*size); \
    if(heapcurrent >= heapend){ \
	SaveHeapRegisters(); \
        reinit_heap((ret),sizeof(Term)*size); \
	RestoreHeapRegisters(); \
        (ret) = (Term*) heapcurrent; \
        heapcurrent += (sizeof(Term)*size); \
    }\
}


#define NewSuspension(ret)\
{\
    (ret) = (suspension *) heapcurrent; \
    heapcurrent += sizeof(suspension); \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
	RestoreHeapRegisters(); \
    	(ret) = (suspension *) heapcurrent; \
    	heapcurrent += sizeof(suspension); \
    }\
}
 

/* Allocates in the constant area */

#define NewAtom(atm)\
{ \
    if(((uword)constcurrent & PTR_ATM_BIT) != 0) {\
       constcurrent += PTR_ATM_BIT;\
    }\
    (atm) = (atom*)constcurrent;\
    constcurrent += AtmSize;\
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}

#define NewFunctor(fnk)\
{ \
    (fnk) = (functor*)constcurrent;\
    constcurrent += FnkSize;\
    ((Generic)((fnk)))->method = &functormethod; \
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}

#define NewExtendedFunctor(fp,length,features) { \
  (fp) = (Functor)constcurrent; \
  constcurrent += (FnkSize + (((length) -ANY) * (FeatEntrySize)) + \
		   ((features) * sizeof(Term))); \
  ((Generic)((fp)))->method = &functormethod; \
  if(constcurrent > constend) { \
    FatalError("out of constant space"); \
  } \
}


#define NewHeapFunctor(fnk) \
{ \
    (fnk) = (functor0*)heapcurrent;\
    heapcurrent += sizeof(functor0);\
    ((Generic)((fnk)))->method = &functor0method; \
    if(heapcurrent >= heapend) {\
	SaveHeapRegisters(); \
        reinit_heap((fnk),sizeof(functor0)); \
	RestoreHeapRegisters(); \
    	(fnk) = (functor0*) heapcurrent; \
    	heapcurrent += sizeof(functor0); \
    }\
    FnkArity((fnk)) = 0;  \
}

#define NewConFlt(i)\
{ \
    if((uword)constcurrent % sizeof(double) != 0) {\
       constcurrent += sizeof(double) - (uword)constcurrent % sizeof(double);\
    }\
    (i) = (Fp)constcurrent;\
    constcurrent += FltSize;\
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}

#define NEWCONX(ret, type, size)\
{ \
    (ret) = (type *)constcurrent;\
    constcurrent += (sizeof(type) * (size));\
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}


typedef char heap;

/* "flipstate" is only for debugging GC.
 * It is sometimes useful to inspect a heapblock to see whether it's
 * in from-space or to-space.
 */

typedef enum {
  FLIP = 0,
  FLOP = 1
} flipstate;

extern flipstate currentflip;

typedef struct block {
  flipstate flipflop;
  struct block *next;
  heap  *start;
  heap  *end;
  heap  *logical_end;
} block;

typedef enum {
  OFF = 0,
  ON = 1
} gcvalue;

extern gcvalue 	gcflag;

extern block	*current_block;

extern heap 	*glob_heapend;
extern heap 	*glob_heapcurrent;

extern heap 	*constspace;
extern heap 	*constend;
extern heap 	*constcurrent;

extern int 	heapsize;
extern int 	blocksize;
extern int 	constsize;

extern heap 	*heap_start();
extern int 	heap_size();

extern void	gc_begin();
extern void	gc_end();

extern void	init_alloc();

extern void 	reinit_heap();
extern void	restore_heap_block();
extern void	force_garbage_collection();

extern char 	*salloc();
extern char 	*qalloc();


extern void initialize_storage();
