/* $Id: configuration.h,v 1.2 1992/01/29 13:35:09 sverker Exp $ */

#define Trail(W,V)\
{\
   (W)->trail.current++;\
   if((W)->trail.current == (W)->trail.end){\
     reinit_trail(W);\
   }\
   *((W)->trail.current) = Var(V);\
}

#define EmptyTrail(W) ((W)->context.current->trail == (W)->trail.current)

#define UndoTrail(W) \
{\
    register variable **varp, **stop;\
    stop = (W)->context.current->trail;\
    varp = (W)->trail.current;\
    for(; varp != stop ; varp--) {\
        UnBind(*(varp));\
    }\
    (W)->trail.current = varp;\
}

#define SuspendTrail(W,A) \
{\
    register variable **t;\
    t = (W)->context.current->trail; \
    for(; (W)->trail.current != t; (W)->trail.current--) {\
        TAGGED *vars;\
        constraint *constr;\
        TAGGED v0,v1;\
        v0 = (TAGGED)*(W)->trail.current;\
	v1 = Var(v0)->bound_to;\
	MakeArgs(vars,2);\
	vars[0] = v0;\
	vars[1] = v1;\
	MakeConstraint(constr,unify,vars,2,A);\
        if(IsVar(v1)) {\
	  DEREF(v1, v1);\
	  if(IsUnBound(v1)){\
	    MakeSuspension(Var(v1),constr);\
	  }\
	}\
	MakeSuspension(Var(v0),constr);\
	UnBind(v0);\
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

#define RPromote(W,A)\
{\
   if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = RPROMOTE;\
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

#define RSplit(W,A)\
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = RSPLIT;\
  (W)->task.current->value.andbox = (A);\
  (W)->task.current++;\
}


#define Continue(W,C)\
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = CONTINUE;\
  (W)->task.current->value.code = (C);\
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

#define Main(W) \
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = MAIN;\
  (W)->task.current->value.constr = NULL;\
  (W)->task.current++;\
}

#define Wake(W,C) \
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = WAKE;\
  (W)->task.current->value.constr = (C);\
  (W)->task.current++;\
}

#define Examine(W,V) \
{\
  if((W)->task.current == (W)->task.end){\
     reinit_task(W);\
  }\
  (W)->task.current->type = EXAMINE;\
  (W)->task.current->value.var = (V);\
  (W)->task.current++;\
}

#define NextTask(W,T)  {T = --((W)->task.current);}

#define EmptyAndbTask(W) ((W)->task.current == (W)->context.current->andb)
#define EmptyChbTask(W) ((W)->task.current == (W)->context.current->chb)


#define WakeAll(W,V)\
{\
  suspension *s;\
  for(s = (V)->susp; s != NULL; s = s->next)\
     Wake(W,s->suspended.constraint);\
  TagOf(V) = HVA;\
  (V)->susp = NULL;\
}

#define SuspendOn(W,V)\
{\
  *(++((W)->suspend.current)) = Var(V);\
  if((W)->suspend.current == (W)->suspend.end){\
     reinit_suspend(W);\
  }\
}

#define MakeSuspensions(W,Constr)\
{\
  for((W)->suspend.current; (W)->suspend.current != (W)->suspend.start; (W)->suspend.current--){\
    MakeSuspension(*(W)->suspend.current,Constr);\
  }\
}

#define RestoreSuspension(W)\
  (W)->suspend.current = (W)->suspend.start;

#define NewContext(W)\
{    (W)->context.current++;\
     if((W)->context.current == (W)->context.end){\
	reinit_context(W);\
     }\
     (W)->context.current->chb = (W)->task.current;\
     (W)->context.current->andb = (W)->task.current;\
     (W)->context.current->trail = (W)->trail.current;\
}

#define StoreAndbTask(W)\
     (W)->context.current->andb = (W)->task.current;


#define RestoreAndbTask(W)\
    (W)->task.current = (W)->context.current->andb;


#define RestoreChbTask(W)\
    (W)->task.current = (W)->context.current->chb;


#define ResetContext(W);\
   (W)->context.current--;

#define AddClose(Gnr,W) { Gen(Gnr)->next = (W)->close; (W)->close = Gen(Gnr); }


typedef enum {
    WAKE,
    EXAMINE,
    MAIN,
    PROMOTE,
    RPROMOTE,    
    SPLIT,
    RSPLIT,    
    CONTINUE,
    ROOT,
    SKIPFAIL,
    START      
} tasktype;


typedef struct task {
    tasktype type;
    union {
        code	*code;
	struct variable *var;
	struct suspension *susp;
	struct constraint *constr;
	struct andbox *andbox;
    } value;
} task;


typedef struct context {
    task 	*chb;
    task	*andb;
    variable 	**trail;
} context;

typedef struct taskhead {
  int  		size;
  task  	*start;
  task	 	*current;
  task 		*end;
} taskhead;

typedef struct trailhead {
  int  		size;
  variable 	**start;
  variable 	**current;
  variable 	**end;
} trailhead;

typedef struct contexthead {
  int  		size;
  context 	*start;
  context 	*current;
  context 	*end;
} contexthead;

typedef struct suspendhead {
  int  		size;
  variable 	**start;
  variable 	**current;
  variable 	**end;
} suspendhead;

typedef struct configuration {
  struct configuration  *next;
  struct configuration 	*father;
  struct generic 	*close;
  variable 		*cva;
  code			*pc;
  u16			arity;
  definition*		def;
  choicebox 		*insert;
  andbox 		*andb;
  choicebox		*root;
  trailhead		trail;
  taskhead		task;
  contexthead		context;
  suspendhead		suspend;
  BOOL tracing;
  BOOL debugging;
  TAGGED		areg[MAX_AREGS];
} configuration;


extern void	reinit_context();
extern void	reinit_trail();
extern void 	reinit_suspend();
extern void	reinit_task();
extern task 	*shift_andb_tasks();

extern void 	newcontext();
extern void	resetcontext();

extern configuration 	*init_configuration();
extern void	return_configuration();
