/* $Id: trace.h,v 1.26 1994/04/05 12:18:06 jm Exp $ */

typedef enum {
  TRACE_CALL_GOAL=1,
  TRACE_EXIT_GOAL=2,
  TRACE_SUSPEND_GOAL=4,
  TRACE_REENTER_GOAL=8,
  TRACE_FAIL_GOAL=16,

  TRACE_CREATE_GUARD=32,
  TRACE_FAIL_GUARD=64,
  TRACE_SUSPEND_GUARD=128,
  TRACE_WAKE_GUARD=256,

  TRACE_UP_GUARD=512,
  TRACE_DOWN_GUARD=1024,

  TRACE_NOISY_PRUNING_GUARD=2048,          
  TRACE_PROMOTE_GUARD=4096,

  TRACE_NON_DET_PRE=8192,        
  TRACE_NON_DET_POST=16384
} tracemode;



typedef enum {
  NOTRACE = 0,
  CREEP = 1,
  SKIP = 2,
  LEAP = 4,
  MLEAP = 8,
  TLEAP = 16,
  MONITOR= 32,
  MONITOR_CHILDREN=64,
  TRACE_INIT=128
} tracef;

typedef enum {
  NOSPY_MARKER
} spyf;

#define NOSPY	((predicate *)NOSPY_MARKER)

#ifdef TRACE

#define AndContName(ACONT) (AtmPname(ACONT->def->name))
#define AndContNo(ACONT) (ACONT->clno)

#define Top_Level(ANDB)  ((ANDB->father->father==NULL))

#define Stop_Spy(DEF) (((tracing & LEAP) && (DEF != NULL && DEF->spypoint)))

#define Stop_Monitor_Chb(CHB) ((tracing & MLEAP) && (CHB->trace))
#define Call_Monitor(ANDB) ((tracing & MLEAP) && (ANDB->trace & (MONITOR|MONITOR_CHILDREN)))

#define Stop_Children_Call(ANDB)\
((tracing & MLEAP) && (ANDB->cont->trace & MONITOR_CHILDREN))

#define Stop_Skip_This(ANDB) ((tracing & SKIP) && (ANDB->trace & SKIP))
#define Stop_Skip_Fail(ANDB) ((tracing & SKIP) && ((ANDB->trace & (SKIP|CREEP))))

#define Stop_Skip_Exit_Call(ANDB) ((tracing & SKIP) && (ANDB->trace & SKIP))
#define Stop_Skip_Suspend(CHB) ((tracing & SKIP) && (CHB->father->trace & SKIP))
#define Stop_Skip_Exit_Goal(CHB) ((tracing & SKIP) && (CHB->father->trace & SKIP))

#define TRACE_Begin(ANDB) {if(tracing_init) trace_begin(ANDB);}


/* C (no stop on call/simple exit even if child of monitor-children */

#define TRACE_Call_C(DEF,AREG,ANDB,INS)\
{if(((tracing & CREEP) || Stop_Spy(DEF)) && (filter & TRACE_CALL_GOAL))\
       trace_call(TRACE_CALL_GOAL,DEF, AREG, ANDB,FALSE,INS);}

#define TRACE_Exit_Call_C(DEF,AREG,ANDB,INS)\
{if((Stop_Skip_This(ANDB)) || (Call_Monitor(ANDB) || \
     (((tracing & (CREEP)) || Stop_Spy(DEF))\
  && (filter & TRACE_EXIT_GOAL))))\
    trace_call(TRACE_EXIT_GOAL,DEF, AREG, ANDB,FALSE,INS);}

#define TRACE_Exit_Goal_C(CHB)\
{ if((Stop_Skip_This(CHB->father) || (((tracing & CREEP) || Stop_Spy(CHB->def)\
	  || Stop_Monitor_Chb(CHB)) && (filter & TRACE_EXIT_GOAL))))\
      trace_goal(TRACE_EXIT_GOAL, CHB,FALSE);}

#define TRACE_Suspend_Goal_C(CHB)\
{ if(tracing & SKIP) trace_suspend++;\
  if(Stop_Children_Call(CHB->father)) CHB->trace |=MONITOR;\
  if(Call_Monitor(CHB->father)) \
  { CHB->trace |= MONITOR;\
    CHB->father->trace &= ~(MONITOR|MONITOR_CHILDREN); }\
  if((Stop_Skip_This(CHB->father) || (((tracing & CREEP) || Stop_Spy(CHB->def)\
	  || Stop_Monitor_Chb(CHB)) && (filter & TRACE_SUSPEND_GOAL))))\
      trace_goal(TRACE_SUSPEND_GOAL, CHB,FALSE);}

#define TRACE_Fail_Call_C(DEF,AREG,ANDB,INS)\
{if((!tracing_init) && (Top_Level(ANDB) || tracing))\
       trace_call(TRACE_FAIL_GOAL,DEF, AREG, ANDB, FALSE,INS);}

#define TRACE_Reenter_Goal_C(CHB)\
{ if(tracing & SKIP) trace_suspend--;\
  if(((tracing & CREEP) || Stop_Spy(CHB->def) || Stop_Monitor_Chb(CHB))\
   && (filter & TRACE_REENTER_GOAL)) trace_goal(TRACE_REENTER_GOAL,CHB,FALSE);}

/* Abstractions */

#define TRACE_Call_Apply(DEF,ABSTR,ARITY,AREG,ANDB,INS)\
{if(((tracing & CREEP) || Stop_Spy(DEF)) && (filter & TRACE_CALL_GOAL))\
       trace_apply(TRACE_CALL_GOAL, DEF, ABSTR, ARITY, AREG, ANDB, FALSE,INS);}

#define TRACE_Fail_Apply(DEF,ABSTR,ARITY,AREG,ANDB,INS)\
{if(((tracing & CREEP) || Stop_Spy(DEF)) && (filter & TRACE_CALL_GOAL))\
       trace_apply(TRACE_FAIL_GOAL, DEF, ABSTR, ARITY, AREG, ANDB, FALSE,INS);}

#define TRACE_Fail_Apply_List(DEF,ABSTR,LIST,ANDB,INS)\
{if(((tracing & CREEP) || Stop_Spy(DEF)) && (filter & TRACE_CALL_GOAL))\
      trace_apply_list(TRACE_FAIL_GOAL, DEF, ABSTR, LIST, ANDB, FALSE,INS);}


/* Goal-related */

#define TRACE_Call(DEF,AREG,ANDB,INS)\
{if((Stop_Children_Call(ANDB) || (tracing & CREEP) || Stop_Spy(DEF))\
     && (filter & TRACE_CALL_GOAL))\
       trace_call(TRACE_CALL_GOAL,DEF, AREG, ANDB,TRUE,INS);}

#define TRACE_Exit_Cont(ANDB,INS)\
{ if(((ANDB->cont->trace & CREEP) && (tracing & SKIP)) \
     || (((tracing & CREEP) || Stop_Spy(ANDB->cont->def)\
	  || (ANDB->cont->trace & MONITOR_CHILDREN)) && (filter & TRACE_EXIT_GOAL)))\
     trace_exit_cont(ANDB,INS);}

#define TRACE_Suspend_Goal(CHB)\
{ if(tracing & SKIP) trace_suspend++;\
  if((Stop_Skip_This(CHB->father) || (((tracing & CREEP) || Stop_Spy(CHB->def)\
	  || Stop_Monitor_Chb(CHB)) && (filter & TRACE_SUSPEND_GOAL))))\
      trace_goal(TRACE_SUSPEND_GOAL, CHB,TRUE);}

#define TRACE_Fail_Goal(CHB)\
{if(!(tracing_init) && (Top_Level(CHB->father) || tracing))\
  trace_goal(TRACE_FAIL_GOAL,CHB,TRUE);}

#define TRACE_Reenter_Goal(CHB)\
{ if(tracing & SKIP) trace_suspend--;\
  if(((tracing & CREEP) || Stop_Spy(CHB->def) || Stop_Monitor_Chb(CHB))\
   && (filter & TRACE_REENTER_GOAL)) trace_goal(TRACE_REENTER_GOAL,CHB,TRUE);}

/* Guard-related */

#define TRACE_Create_First_Guard(ANDB,CHB)\
{if(Call_Monitor(CHB->father))\
  { CHB->trace = (((CHB->father->trace) & (MONITOR|MONITOR_CHILDREN)) | MONITOR);\
    CHB->father->trace &= ~(MONITOR|MONITOR_CHILDREN); }\
if(Stop_Children_Call(CHB->father)) CHB->trace |=(MONITOR|MONITOR_CHILDREN);\
   TRACE_Create_Guard(ANDB);}

#define TRACE_Create_Guard(ANDB)\
{ANDB->clno=ANDB->father->next_clno++;\
if(((tracing & CREEP) && (filter & TRACE_CREATE_GUARD))) trace_guard(TRACE_CREATE_GUARD,ANDB);}
  
#define TRACE_Suspend_Guard(ANDB)\
{if((tracing & CREEP) && (filter & TRACE_SUSPEND_GUARD)) trace_guard(TRACE_SUSPEND_GUARD,ANDB);}

#define TRACE_Fail_Guard(ANDB)\
{if((!(tracing_init)) && ((tracing  || Top_Level(ANDB))))\
   trace_guard(TRACE_FAIL_GUARD,ANDB);}

#define TRACE_Wake_Guard(ANDB)\
{if((tracing & CREEP) && (filter & TRACE_WAKE_GUARD)) trace_guard(TRACE_WAKE_GUARD,ANDB);}  

/* Promotion-related */

#define TRACE_Promote_Guard(ANDB)\
{ if(Stop_Skip_This(ANDB->father->father)) \
    {ANDB->cont->trace |= CREEP;ANDB->father->father->trace |=CREEP;\
     ANDB->father->father->trace &= ~SKIP;}\
      if(((tracing & CREEP) && (filter & TRACE_PROMOTE_GUARD)) ||\
	 Stop_Monitor_Chb(ANDB->father)) trace_guard(TRACE_PROMOTE_GUARD,ANDB);}

#define TRACE_Noisy_Pruning(ANDB)\
{ if(Stop_Skip_This(ANDB->father->father)) \
    {ANDB->cont->trace |= CREEP;ANDB->father->father->trace |=CREEP;\
     ANDB->father->father->trace &= ~SKIP;}\
      if(((tracing & CREEP) && (filter & TRACE_NOISY_PRUNING_GUARD)) ||\
	 (Stop_Monitor_Chb(ANDB->father))) trace_guard(TRACE_NOISY_PRUNING_GUARD,ANDB);}
   
/* Movement-related */

#define TRACE_Up_Guard(ANDB)\
{if((tracing & CREEP) && (filter & TRACE_UP_GUARD)) trace_guard(TRACE_UP_GUARD,ANDB);}

#define TRACE_Down_Guard(FANDB,TANDB) {if((tracing & (CREEP|MLEAP)) && (FANDB!=TANDB))\
 trace_down_guard(1,FANDB,TANDB);}


/* Nondeterminism-related */

#define TRACE_Non_Det_Pre(ANDB)\
{if(((tracing & CREEP) || Stop_Spy(ANDB->father->def)\
 || (Stop_Monitor_Chb(ANDB->father)))\
&& (filter & TRACE_NON_DET_PRE)) trace_nondet(TRACE_NON_DET_PRE,ANDB);}

#define TRACE_Non_Det_Post(ANDB)\
{if(((tracing & CREEP) || Stop_Spy(ANDB->father->def) || Stop_Monitor_Chb(ANDB->father))\
 && (filter & TRACE_NON_DET_PRE)) trace_nondet(TRACE_NON_DET_POST,ANDB);}

#else

#define TRACE_Begin(ANDB) {}

#define TRACE_Call_C(DEF,AREG,ANDB,INS) {}
#define TRACE_Exit_Call_C(DEF,AREG,ANDB,INS) {}
#define TRACE_Exit_Goal_C(CHB) {}
#define TRACE_Suspend_Goal_C(CHB) {}
#define TRACE_Fail_Call_C(DEF,AREG,ANDB,INS) {}
#define TRACE_Reenter_Goal_C(CHB) {}

#define TRACE_Call(DEF,AREG,ANDB,INS) {}
#define TRACE_Exit_Cont(ANDB,INS) {}

#define TRACE_Call_Apply(DEF,ABSTR,ARITY,AREG,ANDB,INS) {}
#define TRACE_Fail_Apply(DEF,ABSTR,ARITY,AREG,ANDB,INS) {}
#define TRACE_Fail_Apply_List(DEF,ABSTR,LIST,ANDB,INS) {}


#define TRACE_Execute(DEF,AREG,ANDB,INS) {}
#define TRACE_Execute_C(DEF,AREG,ANDB,INS) {}
#define TRACE_Suspend_Goal(CHB) {}
#define TRACE_Fail_Goal(CHB) {}
#define TRACE_Reenter_Goal(CHB) {}

#define TRACE_Create_First_Guard(ANDB,CHB) {}
#define TRACE_Create_Guard(ANDB) {}
#define TRACE_Suspend_Guard(ANDB) {}
#define TRACE_Fail_Guard(ANDB) {}
#define TRACE_Wake_Guard(ANDB) {}

#define TRACE_Promote_Guard(ANDB) {}
#define TRACE_Noisy_Pruning(ANDB) {}

#define TRACE_Up_Guard(ANDB) {}
#define TRACE_Down_Guard(ANDB1,ANDB2) {}

#define TRACE_Non_Det_Pre(ANDB) {}
#define TRACE_Non_Det_Post(ANDB) {}

#endif

extern int trace_call();
extern int trace_goal();     
extern int trace_guard();
extern int trace_down_guard();
extern int trace_nondet();
extern int trace_cont();
extern void trace_begin();
extern int trace_exit_cont();

extern int trace_apply();
extern int trace_apply_list();


extern unsigned int init_filter();

extern int suspend_depth;
# define SUSPEND_DEPTH_DEFAULT (-1)

extern int trace_suspend;
extern int tracing;
extern int tracing_init;
extern unsigned int filter;
extern int debugging;
extern int failcatching;
extern int trace_full_cont;

extern predicate *spypoints;

extern void initialize_trace();

#ifdef TRACE
extern int identity;
#endif



