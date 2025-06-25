/* $Id: exstate.h,v 1.26 1994/05/10 13:47:02 jm Exp $ */

#include "unify.h"

#define NextTask(W,T) {T = (--(W)->task.current);}
#define NextRecall(W,T)  {T = (--(W)->recall.current)->chb;}
#define NextWake(W,T)  {T = (--(W)->wake.current)->andb;}

#define EmptyTask(W) ((W)->task.current == (W)->context.current->task)
#define EmptyWake(W) ((W)->wake.current == (W)->context.current->wake)
#define EmptyRecall(W) ((W)->recall.current == (W)->context.current->recall)

#define EmptyTrail(W) ((W)->context.current->trail == (W)->trail.current)


#define TrailValue(W,V,X)\
{\
   (W)->trail.current++;\
   if((W)->trail.current == (W)->trail.end){\
     reinit_trail(W);\
   }\
  (W)->trail.current->var = (V);\
  (W)->trail.current->val = (X);\
}

#define TrailRef(W,V)	TrailValue(W,TagRef(V),RefTerm(V))

#define UndoTrail(W) \
{\
    register trailentry *tr, *stop;\
    stop = (W)->context.current->trail;\
    tr = (W)->trail.current;\
    for(; tr != stop ; tr--) {\
	Bind(Ref(tr->var),tr->val);\
    }\
    (W)->trail.current = stop;\
}

#define MakeBigVar(R)\
{\
    svainfo *new;\
    NewSvainfo(new);\
    new->method = NULL;\
    new->env = Env(RefTerm(R));\
    new->susp = NULL;\
    RefTerm(R) = TagGva(new);\
}

#define SuspendTrail(W,A) suspend_trail(W,A);

#define PromoteConstraints(W,P,A) promote_constraints(W,P,A);
    
#define PromoteContext(W,A) promote_context(W,A);


#define BindVariable(W,A,V,T,BindFail)\
{\
  if(VarIsUVA(V)) {\
    if(IsLocalUVA(V,A)) {\
      BindCheck(V,T,BindFail);\
    } else if (!bind_external_uva(W,A,V,T)) {\
      BindFail;\
    }\
  } else {\
    if(GvaIsSva(RefGva(V))) {\
      if(IsLocalGVA(V,A)) {\
	WakeAll(W,V);\
	BindCheck(V,T,BindFail);\
      } else if (!bind_external_sva(W,A,V,T)) {\
	BindFail;\
      }\
    } else if (!(RefGva(V)->method->unify(TagRef(V),T,A,W))) {\
      BindFail;\
    }\
  }\
}




#define Promote(W,A)\
{\
   if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = PROMOTE;\
  (W)->task.current->value.andbox = (A);\
  (W)->task.current++;\
}




#define Split(W,A)\
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = SPLIT;\
  (W)->task.current->value.andbox = (A);\
  (W)->task.current++;\
}



#define Root(W)\
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = ROOT;\
  (W)->task.current->value.code = NULL;\
  (W)->task.current++;\
}

#define Start(W)\
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = START;\
  (W)->task.current->value.code = NULL;\
  (W)->task.current++;\
}




#define Wake(W,A) \
{\
  if((W)->wake.current == (W)->wake.end){\
     reinit_wake(W);\
  }\
  assert((A) != NULL);\
  (W)->wake.current->andb = (A);\
  (W)->wake.current++;\
}

#define Recall(W,C) \
{\
  if((W)->recall.current == (W)->recall.end){\
     reinit_recall(W);\
  }\
  assert((C) != NULL);\
  (W)->recall.current->chb = (C);\
  (W)->recall.current++;\
}

#define Examine(W,V,A) \
{\
  if((A)->tried != NULL) {\
    examine(V,A,W);\
  }\
}


#define WakeAll(W,V)\
{\
  gvainfo *Gv = RefGva(V);\
  if (GvaIsSva(Gv)) {\
    suspension *S;\
    for(S = SvaSusp(GvaSva(Gv)); S != NULL; S = S->next){\
      switch(S->type) {\
       case CHB:\
       assert(S->suspended.chb != NULL);\
       Recall(W,S->suspended.chb);\
       break;\
      case ANDB:\
       assert(S->suspended.andb != NULL);\
       Wake(W,S->suspended.andb);\
       break;\
      }\
    }\
    SvaSusp(GvaSva(Gv)) = NULL;\
  } else {\
    if (Gv->method->wakeall != NULL) {\
      Gv->method->wakeall(Gv,W);\
    }\
  }\
}

#define WakeLocal(W, V, A)\
{\
  if(VarIsGVA(V))\
    Examine(W,V,A);\
}

#define WakeLocalGVA(W, V, A)\
  Examine(W,V,A);

#define WakeLocalUVA(W, V, A) ;


/* This is the inteface to builtin agents:

   IfVarSuspend(X) is used by builtin agents to suspend
   on a variable X. it is pushed on the suspension stack.
   The variables andb and exs are implicit.

   MakeSuspensions(W,Chb) is used by the engine to make
   suspensions on the stacked variables. The choice-box
   is the suspended builtin agent.

*/


#define IfVarSuspend(X) \
{ \
  if(IsREF(X)) { \
    *(++((exs)->suspend.current)) = Ref(X);\
    if((exs)->suspend.current == (exs)->suspend.end){\
       reinit_suspend(exs);\
    }\
    return SUSPEND; \
  } \
}

#define SuspendBuiltin(W,Chb)\
{\
   for(; (W)->suspend.current != (W)->suspend.start; (W)->suspend.current--){\
    register Reference *zyX;\
    zyX = (W)->suspend.current;\
    if (VarIsUVA(*zyX)) {\
      MakeBigVar(*zyX);\
    }\
    MakeRecall(*zyX,Chb);\
  }\
}

#define RedoBuiltin(W,Chb) redo_builtin(W,Chb);
/*
{\
  recall *rcp, *rls;\
  rls = ((W)->context.current->recall +1);\
  for(rcp = (W)->recall.current; rcp != rls; rcp--){\
    *rcp = *(rcp -1);\
  }\
  rcp->chb = (Chb);\
  (W)->recall.current++;\
}
*/


#define RestoreSuspension(W)\
  (W)->suspend.current = (W)->suspend.start;

/* Macros for controlling the context */

#define PushContext(W)\
{    (W)->context.current++;\
     if((W)->context.current == (W)->context.end){\
	reinit_context(W);\
     }\
     (W)->context.current->task = (W)->task.current;\
     (W)->context.current->recall = (W)->recall.current;\
     (W)->context.current->wake = (W)->wake.current;\
     (W)->context.current->trail = (W)->trail.current;\
}


#define RestoreContext(W)\
  (W)->task.current =(W)->context.current->task;\
  (W)->recall.current = (W)->context.current->recall;\
  (W)->wake.current = (W)->context.current->wake;\
  (W)->trail.current = (W)->context.current->trail;\


#define RestoreTask(W)\
  (W)->task.current = (W)->context.current->task;

#define RestoreRecall(W)\
  (W)->recall.current = (W)->context.current->recall;

#define RestoreWake(W)\
  (W)->wake.current = (W)->context.current->wake;

#define RestoreTrail(W)\
  (W)->trail.current = (W)->context.current->trail;


#define PopContext(W);\
   (W)->context.current--;



typedef struct trailentry {
    Term	var;
    Term	val;
} trailentry;


typedef enum {
    PROMOTE,
    SPLIT,
    ROOT,
    START      
} tasktype;


typedef struct task {
  tasktype 	type;
  union {
    andbox 	*andbox;
    code	*code;
  } value;
}task;

typedef struct recall {
  choicebox 	*chb;
} recall;

typedef struct wake {
  andbox	*andb;
} wake;


typedef struct context {
  task 		*task;
  recall	*recall;
  wake		*wake;
  trailentry	*trail;
} context;

typedef struct taskhead {
  int  		size;
  task  	*start;
  task	 	*current;
  task 		*end;
} taskhead;


typedef struct recallhead {
  int  		size;
  recall	*start;
  recall	*current;
  recall	*end;
} recallhead;


typedef struct wakehead {
  int  		size;
  wake  	*start;
  wake	 	*current;
  wake	 	*end;
} wakehead;


typedef struct trailhead {
  int  		size;
  trailentry 	*start;
  trailentry 	*current;
  trailentry 	*end;
} trailhead;

#ifdef BAM

typedef Term pseudo_term;	/*  ATTENTION */

typedef union cons2 {
  pseudo_term pseudo_term;
  uword reg;
  code *code;
} cons2;

typedef struct cons_stack_cell {
  uword cons1;
  cons2 cons2;
} cons_stack_cell;

typedef struct try_stack_cell {
  code *code;
  cons_stack_cell *cons_stack;
} try_stack_cell;

typedef struct cons_stack_head {
    int size;
    cons_stack_cell *start;
    cons_stack_cell *current;
    cons_stack_cell *end;
} cons_stack_head;

typedef struct try_stack_head {
    int size;
    try_stack_cell *start;
    try_stack_cell *current;
    try_stack_cell *end;
} try_stack_head;

#endif
    

typedef struct contexthead {
  int  		size;
  context 	*start;
  context 	*current;
  context 	*end;
} contexthead;

typedef struct suspendhead {
  int  		size;
  Reference	*start;
  Reference 	*current;
  Reference 	*end;
} suspendhead;

enum close_obj_type {
  GENERIC_CLOSE,
  GVAINFO_CLOSE
};

typedef struct close_entry {
  struct close_entry 	*next;
  enum close_obj_type	type;
  void			*obj;
} close_entry;

typedef struct exstate {
  struct exstate 	*father;
  struct exstate    	*next;
  struct exstate	*child;
  close_entry		*close;
  code			*pc;
  uword			arity;
  predicate		*def;
  choicebox 		*insert;
  andbox 		*andb;
  choicebox		*root;
  trailhead		trail;
  taskhead		task;
  recallhead		recall;  
  wakehead		wake;
  contexthead		context;
  suspendhead		suspend;
  bool 			tracing;
  bool 			debugging;
  struct unifystate	urec;
  Term			areg[MAX_AREGS];
#ifdef BAM
  cons_stack_head      cons_stack;
  try_stack_head       try_stack;
#endif
} exstate;


extern void	reinit_context();
extern void	reinit_trail();
extern void 	reinit_suspend();
extern void	reinit_task();
extern void	reinit_recall();
extern void	reinit_wake();

extern recall 	*shift_recall();

extern void 	newcontext();
extern void	resetcontext();

extern exstate 	*init_exstate();
extern void	return_exstate();

extern void	add_generic_to_close();
extern void	add_gvainfo_to_close();

extern void 	suspend_trail();
extern void 	promote_context();
extern void     promote_constraints();

extern bool	bind_external_uva();
extern bool	bind_external_sva();

extern void	redo_builtin();
