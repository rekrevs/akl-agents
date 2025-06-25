/* Id: trace.c,v 1.27 1992/12/12 11:22:20 perbrand Exp $ */

#include "include.h"
#include "instructions.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "initial.h"
#include "names.h"
#include "storage.h"
#include "config.h"
#include "trace.h"
#include "display.h"
#include "intrpt.h"
#include "error.h"

/* from debug.c */
extern int readstring();

void trace_path();
void spy_def();
void nospy_def();
void spy_def_debug();
void nospy_def_debug();
void nospyall();
void debugging_aux();

void trace_printcall();
void trace_printgoal();
void trace_printexit();

void full_trace_path();

int trace_fail_and();

int debug_user_interaction();

void manual_update();
void no_monitor_and_box();
unsigned int filter_menu();

Functor functor_slash_2;	/* (_ "/" _) */

#ifdef TRACE
int identity;
#endif

predicate *spypoints= (predicate *) &spypoints;

int tracing = NOTRACE;
int debugging;
int failcatching;
int trace_full_cont;
int suspend_depth = SUSPEND_DEPTH_DEFAULT;

int trace_suspend;

choicebox *trace_insert;

#define Goal_Related (TRACE_CALL_GOAL|TRACE_EXIT_GOAL|TRACE_SUSPEND_GOAL| \
		      TRACE_REENTER_GOAL|TRACE_FAIL_GOAL)
#define Guard_Related (TRACE_CREATE_GUARD|TRACE_FAIL_GUARD|TRACE_SUSPEND_GUARD|TRACE_WAKE_GUARD)
#define Movement_Related (TRACE_UP_GUARD|TRACE_DOWN_GUARD)
#define Promotion_Related (TRACE_NOISY_PRUNING_GUARD|TRACE_PROMOTE_GUARD)
#define Nondeterminism_Related (TRACE_NON_DET_PRE|TRACE_NON_DET_POST)

#define NO_TRACE_FLAGS 15

#define NO_OPTION 0
#define SPY_OPTION 1
#define SKIP_OPTION 2
#define OLD_SKIP_OPTION 4
#define MONITOR_OPTION 8
#define MONITOR_C_OPTION 16
#define MONITOR_CALL_OPTION 32
#define MONITOR_CALL_C_OPTION 64

#define NO_STOP 0
#define STOP_SKIP_THIS 1
#define STOP_SKIP_OTHER 2
#define STOP_MONITOR 4
#define STOP_SPY 8
#define STOP 16

#define Tracing_Set(SET)  {tracing &=MLEAP;tracing |=(SET);}
#define Andbox_Name(ANDB) (AtmPname(ANDB->father->def->name))
#define Choicebox_Name(CHB) (AtmPname(CHB->def->name))
#define Andbox_No(ANDB) (ANDB->clno)

#define TRACE_TSIZ trace_tsiz;

#define TRACE_OK 1
#define TRACE_FAIL 2
#define TRACE_PRINT 3

int trace_tsiz = 3;

unsigned int filter;

int tracing_init;

char tracecom[500];

bool akl_trace_set_print_depth(Arg)
     Argdecl;
{
#ifdef TRACE
  Term X0;
  Deref(X0, A(0));
  IfVarSuspend(X0);
  if(IsINT(X0)) {
    trace_tsiz = GetInteger(X0);
    return TRUE;
  }
#endif
  return FALSE;
}

bool akl_suspend_depth(Arg)
     Argdecl;
{
  Term X0;
  Deref(X0, A(0));
  IfVarSuspend(X0);
  if(IsINT(X0)) {
    suspend_depth  = GetInteger(X0);
    return TRUE;
  }
  return FALSE;
}

bool akl_trace_get_print_depth(Arg)
     Argdecl;
{
#ifdef TRACE
  Term X0, t;
  Deref(X0, A(0));
  MakeIntegerTerm(t, trace_tsiz);
  return unify(X0, t, exs->andb, exs);
#endif
  return FALSE;
}

bool akl_is_debugging(Arg)
     Argdecl;
{
#ifdef TRACE
  if (debugging)
    return TRUE;
#endif
  return FALSE;
}

bool akl_is_tracing(Arg)
     Argdecl;
{
#ifdef TRACE
  if (debugging ==CREEP)
    return TRUE;
#endif
  return FALSE;
}

bool akl_debugging(Arg)
    Argdecl;
{
#ifdef TRACE
  debugging_aux();
#endif
  return TRUE;
}

bool akl_debug(Arg)
    Argdecl;
{
#ifdef TRACE
    debugging = LEAP;

    printf("{The debugger will first leap -- showing spypoints (debug)}\n");
#endif
    return TRUE;
}

bool akl_nodebug(Arg)
    Argdecl;
{
#ifdef TRACE
    debugging = NOTRACE;

    printf("{The debugger is switched off}\n");
#endif
    return TRUE;
}


bool akl_spy(Arg)
     Argdecl;
{
  register Term X0, T1, T2;
  predicate *def;
  Atom name;
  int arity;
#ifdef TRACE
  Deref(X0, A(0));

  if (debugging == NOTRACE) {
    debugging = LEAP;
    printf("{The debugger will first leap -- showing spypoints (debug)}\n");
  }
  
  if (IsSTR(X0) && StrFunctor(Str(X0)) == functor_slash_2) {
    GetStrArg(T1, Str(X0), 0);
    Deref(T1, T1);
    GetStrArg(T2, Str(X0), 1);
    Deref(T2, T2);
    if (IsATM(T1) && IsINT(T2)) {
      name = Atm(T1);
      arity = GetInteger(T2);
      def = find_predicate(name, arity);
      if(def != NULL)
        spy_def(def);
      else
	printf("{Warning: spy - no matching predicate}\n");
      return TRUE;
    }
  }

  printf("{DOMAIN ERROR: spy expected predicate spec.}\n");
#endif
  return FALSE;
}

bool akl_nospy(Arg)
     Argdecl;
{
  register Term X0, T1, T2;
  predicate *def;
  Atom name;
  int arity;
#ifdef TRACE
  Deref(X0, A(0));

  if (IsSTR(X0) && StrFunctor(Str(X0)) == functor_slash_2) {
    GetStrArg(T1, Str(X0), 0);
    Deref(T1, T2);
    GetStrArg(T2, Str(X0), 1);
    Deref(T2, T2);
    if (IsATM(T1) && IsINT(T2)) {
      name = Atm(T1);
      arity = GetInteger(T2);
      def = find_predicate(name, arity);
      if(def != NULL)
      	nospy_def(def);
      else
	printf("{Warning: nospy - no matching predicate}\n");
      return TRUE;
    }
  }
  printf("{DOMAIN ERROR: nospy expected predicate spec.}\n");
#endif
  return FALSE;
}
  
bool akl_nospyall(Arg)
     Argdecl;
{
#ifdef TRACE
   nospyall();
   return TRUE;
#else
   return FALSE;
#endif
 }


bool akl_trace(Arg)
    Argdecl;
{

#ifdef TRACE
  debugging = CREEP;
  printf("{The debugger will first creep -- showing everything (trace)}\n");
  if(tracing_init==FALSE)      
    tracing=debugging;
  return TRUE;
#endif
  return FALSE;
}

bool akl_notrace(Arg)
    Argdecl;
{
#ifdef TRACE
    tracing = NOTRACE;
    debugging = NOTRACE;
    printf("{The debugger is switched off}\n");
    return TRUE;
#else
    return FALSE;
#endif
}

bool akl_sys_check_trace(Arg)
    Argdecl;
{
#ifdef TRACE
  tracing_init=TRUE;
#endif
  return TRUE;
}    

bool akl_sys_notrace(Arg)
    Argdecl;
{
#ifdef TRACE
    tracing = NOTRACE;
#endif
    return TRUE;
}    
     
bool akl_failcatch(Arg)
    Argdecl;
{
#ifdef TRACE
    failcatching = TRUE;
    return TRUE;
#else
    return FALSE;
#endif
}

bool akl_nofailcatch(Arg)
    Argdecl;
{
#ifdef TRACE
    failcatching = FALSE;
    return TRUE;
#else
    return FALSE;
#endif
}


#ifdef TRACE

int trace_call(mode,def, areg, andb,flag,ins)
    tracemode mode;     
    predicate *def;
    Term *areg;
    andbox *andb;
    int flag;
     choicebox *ins;
{
  unsigned int option=SPY_OPTION;
  int fail_case;

  if(ins==NULL)
    trace_insert=(choicebox *)1;
  else
    trace_insert=ins;
  
  switch(mode) {

  case TRACE_CALL_GOAL:
    { int res;
      int tsiz = TRACE_TSIZ;
      
      if((flag) && Stop_Children_Call(andb)) /* akl goal */
	{
	  andb->trace |= MONITOR;
	}
      if (def->spypoint != NOSPY)
	printf("+");
      else
	printf(" ");
      if(Stop_Children_Call(andb))
	printf("*");
      else 
	printf(" ");
      if(tracing & SKIP)
	{
	  option |= OLD_SKIP_OPTION;
	  printf("^");
	}
      else printf(" ");

    call_goal_print:
      printf(" Call: ");
      trace_printcall(def, areg, tsiz);
      if(tracing & TLEAP) 
	{
	  printf("\n");
	  return TRACE_OK;
	}
      if(flag) option |=MONITOR_CALL_OPTION;
      else option |=MONITOR_CALL_C_OPTION;
      option |=SKIP_OPTION;
      res = debug_user_interaction(option,andb,def,NULL);
      if(res == TRACE_PRINT) {
	tsiz = -1;
	printf("   ");
	goto call_goal_print;
      }
      return res;
    }
    
  case TRACE_EXIT_GOAL:		/*  ONLY C_FUNCTIONS */
    { int res;
      int tsiz = TRACE_TSIZ;
      
      if(andb->father->trace & MONITOR)
	andb->father->trace &= ~MONITOR;

      if(Stop_Skip_This(andb))	/*  skipped this goal */
	{
	  andb->trace &= SKIP;
	  if(!(filter & TRACE_EXIT_GOAL)) return TRACE_OK;
	  if (def->spypoint != NOSPY) printf("+ ");
	  else printf("  ");
	  if(trace_suspend>0) printf("S");
	  else printf("s");
	}
      else
	{
	  if (def->spypoint != NOSPY) printf("+");
	  else printf(" ");
	  if(tracing & SKIP)
	    {
	      option |= OLD_SKIP_OPTION;
	      printf(" ^");
	    }
	  else printf("  ");
	}

    exit_goal_print:
      printf(" Exit: ");
      trace_printcall(def, areg, tsiz);
      if(tracing & TLEAP) 
	{
	  printf("\n");
	  return TRACE_OK;
	}
      res = debug_user_interaction(option,andb,def,NULL);
      if(res == TRACE_PRINT) {
	tsiz = -1;
	printf("   ");
	goto exit_goal_print;
      }
      return res;
    }
    
  case TRACE_FAIL_GOAL:		/*  ONLY C functions */
    { int res;
      int tsiz = TRACE_TSIZ;
      
      fail_case=NO_STOP;
      if(Top_Level(andb))
	{
	  if(failcatching) 
	    {
	      tracing &= ~LEAP;
	      tracing |= CREEP;
	    }
	  if(!tracing) return TRACE_OK;
	  if(Stop_Skip_Fail(andb))
	    {
	      if(Stop_Skip_This(andb)) 
		{
		  andb->trace &= ~SKIP;
		  fail_case |=STOP_SKIP_THIS;
		  if(filter & TRACE_FAIL_GOAL)
		    fail_case |=STOP;
		}
	      else 
		{
		  fail_case |=STOP_SKIP_OTHER;
		}
	    }
	  if(Stop_Spy(def)) 
	    {
	      fail_case |=STOP_SPY;
	      if((tracing & (LEAP|CREEP)) && (filter & TRACE_FAIL_GOAL))
		fail_case |=STOP;
	    }
	  if((tracing & CREEP) && (filter & TRACE_FAIL_GOAL))
	    fail_case |= STOP;
	  if(Stop_Children_Call(andb))
	    {
	      andb->trace &= ~(MONITOR|MONITOR_CHILDREN);
	      fail_case |=(STOP|STOP_MONITOR);
	    }
	}
      else
	{
	  if(Stop_Skip_Fail(andb))
	    {
	      if(Stop_Skip_This(andb)) 
		{
		  andb->trace &= ~SKIP;
		  fail_case=STOP_SKIP_THIS;
		  if(filter & TRACE_FAIL_GOAL)
		    fail_case |=STOP;
		}
	      else 
		{
		  fail_case |=STOP_SKIP_OTHER;
		}
	    }
	  if(Stop_Spy(def)) 
	    {
	      fail_case |=STOP_SPY;
	      if((tracing & (LEAP|CREEP)) && (filter & TRACE_FAIL_GOAL))
		fail_case |=STOP;
	    }
	  if((tracing & CREEP) && (filter & TRACE_FAIL_GOAL))
	    fail_case |= STOP;
	  if(Stop_Children_Call(andb))
	    {
	      andb->trace &= ~(MONITOR|MONITOR_CHILDREN);
	      fail_case |=(STOP|STOP_MONITOR);
	    }
	}
      if(!(fail_case & STOP)) return trace_fail_and(andb);

      if(fail_case & STOP_SPY) printf("+");
      else printf(" ");
      if(fail_case & STOP_MONITOR) printf("*");
      else printf(" ");
      if(fail_case & STOP_SKIP_THIS)
	{
	  if(trace_suspend>0) printf("S");
	  printf("s");
	}
      else if(fail_case & STOP_SKIP_OTHER) printf("^");
      else printf(" ");

    fail_goal_print:
      printf(" Fail: ");
      trace_printcall(def, areg, tsiz);
      if((tracing & TLEAP)) printf("\n");
      else {
	res = debug_user_interaction(option,andb,def,NULL);
	if(res == TRACE_PRINT) {
	  tsiz = -1;
	  printf("   ");
	  goto fail_goal_print;
	}
      }
    }
    return trace_fail_and(andb);
  default:
    FatalError("trace mode not defined");
    return -1; /* dummy */
  }
}



/* interface from abstractions */

int trace_apply(mode, def, abstr, arity, areg, andb, flag, ins)
    tracemode mode;     
    predicate *def;
    Term abstr;
    int arity;
    Term *areg;
    andbox *andb;
    int flag;
    choicebox *ins;
{

  Reference cdr;
  int i;
  Term l;
  Term treg[2];
	  
  cdr = NULL;
  l = NIL;
  for(i = 0; i < arity; i++) {
    Term c;
    MakeListTerm(c,andb);
    if(l == NIL)
      l = c;
    else
      *cdr = c;
    cdr = LstCdrRef(Lst(c));
    LstCar(Lst(c)) = X(i);
  }
  if(cdr != NULL)
    *cdr = NIL;
	  
  treg[0] = abstr;		/* the abstraction */
  treg[1] = l;			/* the list of arguments */

  return trace_call(mode, def, treg, andb, flag, ins);
}

int trace_apply_list(mode, def, abstr, arg, andb, flag, ins)
    tracemode mode;     
    predicate *def;
    Term abstr;
    Term arg;
    andbox *andb;
    int flag;
    choicebox *ins;
{

  Term treg[2];
  treg[0] = abstr;
  treg[1] = arg;	

  return trace_call(mode, def, treg, andb, flag, ins);
}


int trace_fail_and(andb)
     andbox *andb;
{
  andcont *cont1;
  unsigned int option=SPY_OPTION;
  int fail_case;
  int res;
  int tsiz = TRACE_TSIZ;
  
  cont1=andb->cont;  

  if(cont1->next==NULL) return TRACE_OK;

  fail_case=NO_STOP;

  if(Top_Level(andb))
    {
      if(cont1==NULL) 
	{
	  if(!debugging) return TRACE_OK;
	  else
	    FatalError("trace_fail_and");
	}
      if(cont1->trace & TRACE_INIT)
	{
	  return TRACE_OK;
	}
      andb->cont=cont1->next;
      if(tracing & SKIP) 
	{
	  if((andb->trace & (CREEP)) & (cont1->trace & CREEP))
	    {
	      andb->trace &= ~CREEP;
	      tracing &= ~SKIP;
	      tracing |= CREEP;
	      fail_case |=STOP_SKIP_THIS;
	      if((filter & TRACE_FAIL_GOAL))
		fail_case |=STOP;
	    }
	  else
	    {
	      fail_case |=STOP_SKIP_OTHER;
	      option |=OLD_SKIP_OPTION;
	    }
	}
      if(Stop_Spy(cont1->def))
	{
	  fail_case |=STOP_SPY;
	  if((tracing & LEAP) && (filter & TRACE_FAIL_GOAL))
	    fail_case |=STOP;
	}
      if(cont1->trace & MONITOR_CHILDREN) 
	fail_case |=(STOP|STOP_MONITOR);
      if((tracing & CREEP) && (filter & TRACE_FAIL_GOAL))
	fail_case |=STOP;
    }
  else
    {
      if(cont1==NULL) return TRACE_OK;
      andb->cont=cont1->next;
      if(tracing & SKIP) 
	{
	  if((andb->trace & (CREEP)) & (cont1->trace & CREEP))
	    {
	      andb->trace &= ~CREEP;
	      tracing &= ~SKIP;
	      tracing |= CREEP;
	      fail_case |=STOP_SKIP_THIS;
	      if((filter & TRACE_FAIL_GOAL))
		fail_case |=STOP;
	    }
	  else
	    {
	      fail_case |=STOP_SKIP_OTHER;
	      option |=OLD_SKIP_OPTION;
	    }
	}
      if(Stop_Spy(cont1->def))
	{
	  fail_case |=STOP_SPY;
	  if((tracing & LEAP) && (filter & TRACE_FAIL_GOAL))
	    fail_case |=STOP;
	}
      if(cont1->trace & MONITOR_CHILDREN) 
	fail_case |=(STOP|STOP_MONITOR);
      if((tracing & CREEP) && (filter & TRACE_FAIL_GOAL))
	fail_case |=STOP;
    }
  if(!(fail_case & STOP)) return trace_fail_and(andb);

  if(fail_case & STOP_SPY) printf("+");
  else printf(" ");
  if(fail_case & STOP_MONITOR) printf("*"); 
  else printf(" ");
  if(fail_case & STOP_SKIP_OTHER) printf("^"); 
  else if(fail_case & STOP_SKIP_THIS)  
    {
      if(trace_suspend) printf("S");
      else printf("s");
    }
  else printf(" ");

 print_fail_and:
  if(cont1->label==proceed)
    printf(" Fail: ");
  else
    printf(" Fail In Context: ");
  trace_printexit(cont1, tsiz);
  if(tracing & TLEAP) printf("\n");
  else {
    res = debug_user_interaction(option,andb,cont1->def,NULL);
    if(res == TRACE_PRINT) {
      tsiz = -1;
      printf("   ");
      goto print_fail_and;
    }
  }
  return trace_fail_and(andb);
}

void trace_begin(andb)
     andbox *andb;
{
  andcont *cont=andb->cont;
  
  while(cont!=NULL)
    {
      cont->trace |=TRACE_INIT;
      cont=cont->next;
    }
  tracing=debugging;
  tracing_init=FALSE;
}

int trace_exit_cont(andb,ins)
     andbox *andb;
     choicebox *ins;
{
  andcont *cont;
  unsigned int option=SPY_OPTION;
  int res;
  int tsiz = TRACE_TSIZ;

  trace_insert=ins;
  cont=andb->cont;

  if(Top_Level(andb))
    {
      if(andb->cont->trace & TRACE_INIT)
	{
	  return TRACE_OK;
	}
    }

  if (cont->def->spypoint != NOSPY) printf("+");
  else printf(" ");
  if(cont->trace & MONITOR_CHILDREN) printf("*");
  else printf(" ");
  if(andb->cont->trace & CREEP)
    {
      andb->cont->trace &= ~CREEP;
      if(trace_suspend>0) printf("S");
      else printf("s");
      andb->trace &= ~CREEP;
    }
  else 
    {
      if(tracing & SKIP) printf("^");
      else printf(" ");
    }

 print_exit_cont:

  printf(" Exit: ");
  trace_printexit(cont,tsiz);
  if(tracing & TLEAP) printf("\n");
  else 
    {
      if(andb->trace & CREEP) option |=OLD_SKIP_OPTION;
      res = debug_user_interaction(option,andb,cont->def,NULL);
      if(res == TRACE_PRINT) {
	tsiz = -1;
	printf("   ");
	goto print_exit_cont;
      }
    }
  return TRACE_OK;
}

int trace_goal(mode,chb,flag)
    tracemode mode;     
    choicebox *chb;
    int flag;
{

  unsigned int option=SPY_OPTION;
  int fail_case;
  int res;
  int tsiz = TRACE_TSIZ;

  trace_insert=NULL;

  switch(mode) {

  case TRACE_EXIT_GOAL:
    if (chb->def->spypoint != NOSPY) printf("+");
    else printf(" ");
    if(chb->trace & MONITOR) printf("*");
    else printf(" ");
    if(tracing & SKIP) 
      {
	if(chb->father->trace & SKIP)
	  {
	    if(trace_suspend>0) printf("S");
	    else printf("s");
	    chb->father->trace &= ~SKIP;
	  }
	else
	  {
	    printf("^");
	    option |= OLD_SKIP_OPTION;
	  }
      }
    else printf(" ");

  print_exit_goal:
    printf(" Exit:");
    trace_printgoal(chb, tsiz);
    if(tracing & TLEAP)
      {
	printf("\n");
	return TRACE_OK;
      }
    res = debug_user_interaction(option,chb->father,chb->def,chb);
    if(res == TRACE_PRINT) {
      tsiz = -1;
      printf("   ");
      goto print_exit_goal;
    }
    return res;

  case TRACE_FAIL_GOAL:
    fail_case=NO_STOP;
    if(Top_Level(chb->father))
      {
	if(failcatching) 
	  {
	    tracing &= ~LEAP;
	    tracing |= CREEP;
	  }
	if(Stop_Skip_Fail(chb->father))
	  {
	    if(Stop_Skip_This(chb->father))
	      {
		chb->father->trace &= ~SKIP;
		fail_case |=STOP_SKIP_THIS;
		if(filter & TRACE_FAIL_GOAL)
		  fail_case |=STOP;
	      }
	    else 
	      {
		fail_case=STOP_SKIP_OTHER;
		option |= OLD_SKIP_OPTION;
	      }
	  }
	if(Stop_Spy(chb->def)) 
	  {
	    fail_case |=STOP_SPY;
	    if((tracing & (LEAP|CREEP)) && (filter & TRACE_FAIL_GOAL))
	      fail_case |=STOP;	      
	  }
	if(((tracing & CREEP)) && (filter & TRACE_FAIL_GOAL))
	  fail_case |=STOP;
	if(Stop_Monitor_Chb(chb))
	  fail_case |=(STOP|STOP_MONITOR);
      }
    else
      {
	if(Stop_Skip_Fail(chb->father))
	  {
	    if(Stop_Skip_This(chb->father))
	      {
		chb->father->trace &= ~SKIP;
		fail_case |=STOP_SKIP_THIS;
		if(filter & TRACE_FAIL_GOAL)
		  fail_case |=STOP;
	      }
	    else 
	      {
		fail_case=STOP_SKIP_OTHER;
		option |= OLD_SKIP_OPTION;
	      }
	  }
	if(Stop_Spy(chb->def)) 
	  {
	    fail_case |=STOP_SPY;
	    if((tracing & (LEAP|CREEP)) && (filter & TRACE_FAIL_GOAL))
	      fail_case |=STOP;	      
	  }
	if(((tracing & CREEP)) && (filter & TRACE_FAIL_GOAL))
	  fail_case |=STOP;
	if(Stop_Monitor_Chb(chb))
	  fail_case |=(STOP|STOP_MONITOR);
      }	      
    if(!(fail_case & STOP)) return trace_fail_and(chb->father);

    if(fail_case & STOP_SPY) printf("+");
    else printf(" ");
    if(fail_case & STOP_MONITOR) printf("*");
    else printf(" ");
    if(fail_case & STOP_SKIP_THIS)
      {
	if(trace_suspend>0) printf("S");
	printf("s");
      }
    else if(fail_case & STOP_SKIP_OTHER) printf("^");
    else printf(" ");

  print_fail_goal:
    printf(" Fail: ");
    trace_printgoal(chb, tsiz);
    if(tracing & TLEAP) printf("\n");
    else {
      res = debug_user_interaction(option,chb->father,chb->def,chb);
      if(res == TRACE_PRINT) {
	tsiz = -1;
	printf("   ");
	goto print_fail_goal;
      }
    }
    return trace_fail_and(chb->father);

  case TRACE_SUSPEND_GOAL:
    if (chb->def->spypoint != NOSPY) printf("+");
    else printf(" ");
    if(chb->trace) printf("*");
    else printf(" ");
    if(tracing & SKIP) 
      {
	if(chb->father->trace & SKIP)
	  {
	    if(trace_suspend>1) printf("S");
	    else printf("s");
	    chb->father->trace &= ~SKIP;
	  }
	else
	  {
	    printf("^");
	    option |= OLD_SKIP_OPTION;
	  }
      }
    else printf(" ");

  print_suspend_goal:
    printf(" Suspend: ");
    trace_printgoal(chb, tsiz);
    if(tracing & TLEAP)
      {
	printf("\n");
	return TRACE_OK;
      }
    if(flag) option |= MONITOR_OPTION;
    else option |= MONITOR_C_OPTION;
    res = debug_user_interaction(option,chb->father,chb->def,chb);
    if(res == TRACE_PRINT) {
      tsiz = -1;
      printf("   ");
      goto print_suspend_goal;
    }
    return res;

  case TRACE_REENTER_GOAL:
    if (chb->def->spypoint != NOSPY) printf("+");
    else printf(" ");
    if(chb->trace & MONITOR) printf("*");
    else printf(" ");
    if(tracing & SKIP)
      {
	option |= OLD_SKIP_OPTION;
	printf("^");
      }
    else printf(" ");

  print_reenter_goal:
    printf(" Reenter: ");
    trace_printgoal(chb, tsiz);
    if(tracing & TLEAP)
      {
	printf("\n");
	return TRACE_OK;
      }
    if(flag) option |= MONITOR_OPTION;
    else option |= MONITOR_C_OPTION;
    option |= SKIP_OPTION;
    res = debug_user_interaction(option,chb->father,chb->def,chb);
    if(res == TRACE_PRINT) {
      tsiz = -1;
      printf("   ");
      goto print_reenter_goal;
    }
    return res;
  default:
    FatalError("trace mode not defined");
    return -1; /* dummy */
  }
}


int trace_nondet(mode,cand)
   tracemode mode;     
   andbox *cand;
{
  trace_insert=NULL;
  switch(mode){
  case TRACE_NON_DET_PRE:
    if(cand->father->father->father->def==NULL)
      printf("    Nondet-Pre : Mother=[ROOT], Cand=[%s:%d]",
	     Choicebox_Name(cand->father),Andbox_No(cand));
    else
      printf("    Nondet-Pre : Mother=[%s:%d], Cand=[%s:%d]",
	   Choicebox_Name(cand->father->father->father),Andbox_No(cand->father->father),
	   Choicebox_Name(cand->father),Andbox_No(cand));
    break;
  case TRACE_NON_DET_POST:
    if(cand->father->father->father->def==NULL)
    printf("    Nondet-Post: Mother=[ROOT], Cand=[%s:%d]",
	   Choicebox_Name(cand->father),Andbox_No(cand));
      else
    printf("    Nondet-Post: Mother=[%s:%d], Cand=[%s:%d]",
	   Choicebox_Name(cand->father->father->father),Andbox_No(cand->father->father),
	   Choicebox_Name(cand->father),Andbox_No(cand));
    break;
  default:
    FatalError("trace mode not defined");
  }
    
  if(tracing & TLEAP)
    {
      printf("\n");
      return TRACE_OK;
    }
  return debug_user_interaction(NO_OPTION,cand,NULL,NULL);
}

int trace_guard(mode,andb)
   tracemode mode;     
   andbox *andb;
{
  trace_insert=NULL;
  switch(mode) {
  case TRACE_CREATE_GUARD:
    printf("   ");
    printf(" [Create:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    break;
  case TRACE_SUSPEND_GUARD:
    printf("   ");
    printf(" [Suspend:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    break;
  case TRACE_WAKE_GUARD:          
    printf("   ");
    printf(" [Wake:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    break;
  case TRACE_FAIL_GUARD:
    if(Top_Level(andb))
      {
	if(failcatching) 
	  {
	    tracing &= ~LEAP;
	    tracing |= CREEP;
	  }
      }
    if(!((tracing & CREEP) && (filter & TRACE_FAIL_GUARD)))
      return trace_fail_and(andb);
    printf("   ");
    if(andb->father->father!=NULL)
      printf(" [Fail:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    else
      printf(" [Fail:%d] :ROOT",Andbox_No(andb));
    if(tracing & TLEAP)
      {
	printf("\n");
	return trace_fail_and(andb);
      }
    debug_user_interaction(NO_OPTION,andb,NULL,NULL);
    return trace_fail_and(andb);
  case TRACE_PROMOTE_GUARD:
    printf(" ");
    if(andb->father->trace) 
      {
	if(andb->father->trace & MONITOR_CHILDREN)
	    andb->cont->trace |= MONITOR_CHILDREN;
	printf("*");
      }
    else printf(" ");
    printf("  ");
    printf("[Promote:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    break;
  case TRACE_NOISY_PRUNING_GUARD:
    printf(" ");
    if(andb->father->trace) 
      {
	if(andb->father->trace & MONITOR_CHILDREN)
	    andb->cont->trace |= MONITOR_CHILDREN;
	printf("*");
      }
    else printf(" ");
    printf("[Noisy pruning:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    break;
  case TRACE_UP_GUARD:
    printf("   ");
    if(Top_Level(andb))
      printf(" [Up:  ] :Root");
    else
      printf(" [Up:%d] :%s",Andbox_No(andb),Andbox_Name(andb));
    break;      
  default:
    FatalError("trace mode not defined");
  }
  if(tracing & TLEAP)
    {
      printf("\n");
      return TRACE_OK;
    }
  return debug_user_interaction(NO_OPTION,andb,NULL,NULL);
}

int trace_down_guard(i,fandb,tandb)
int i;andbox *fandb,*tandb;
{
  if(fandb==tandb) return TRACE_OK;
  if(fandb==NULL) return TRACE_OK;
  if(fandb->father->father==tandb)
    FatalError("trace_down_guard");

  trace_down_guard(i+1,fandb,tandb->father->father);
  trace_insert=NULL;
  if(((tracing & CREEP) && (filter & TRACE_DOWN_GUARD)) || Stop_Monitor_Chb(tandb->father))
    {
      if(Stop_Monitor_Chb(tandb->father))
	{
	  printf("   *[%d:Down:%d] :%s",i,Andbox_No(tandb),Andbox_Name(tandb));
	  return debug_user_interaction(NO_OPTION,tandb,NULL,NULL);
	}	  
      else
	{
	  printf("    [%d:Down:%d] :%s",i,Andbox_No(tandb),Andbox_Name(tandb));
	  if(tracing & TLEAP) printf("\n");
	  else return debug_user_interaction(NO_OPTION,tandb,NULL,NULL);
	}
    }
  return TRACE_OK;
}

#define Maybe_De_Skip(OPT,ANDB) if(OPT & OLD_SKIP_OPTION) de_skip(ANDB);

void de_skip(andb,chb)
andbox *andb;choicebox *chb;
{ 
  andbox *tandb=andb;
  andcont *cont;

  while(!(tandb->trace & (SKIP|CREEP)))
    {
      if((tandb->father==NULL) || (tandb->father->father==NULL))
	FatalError("Deskip\n");
      tandb=tandb->father->father;
    }
  if(tandb->trace & SKIP) 
    {
      tandb->trace &= ~SKIP;
      return;
    }
  else tandb->trace &= ~CREEP;
  cont=tandb->cont;
  while((!(cont->trace & CREEP)) && (cont->next!=NULL))
    {
      cont=cont->next;
    }
  if(cont->trace & CREEP)
    {
      cont->trace &= ~CREEP;
      return;
    }
  FatalError("De skip");
  return;
}
      

int debug_user_interaction(option,andb,def,chb)
unsigned int option;andbox *andb;
predicate *def;choicebox *chb;
{
  printf(" ? ");      
  goto top1;
 top:
  printf("                    ? ");
 top1:
  if(readstring(tracecom) == 0) {
    printf("\n");
    tracecom[0] = 'n';
    tracecom[1] = 0;
  }
  switch(tracecom[0]) {

  case 'n':
    Maybe_De_Skip(option,andb);
    tracing = NOTRACE;
    return TRACE_OK;

  case 'l':
    Maybe_De_Skip(option,andb);
    Tracing_Set(LEAP);
    return TRACE_OK;

  case 'm':
    Maybe_De_Skip(option,andb);
    Tracing_Set(NOTRACE);
    return TRACE_OK;

  case 's':
    Maybe_De_Skip(option,andb);
    if(option & SKIP_OPTION)
      {
	trace_suspend=0;
	andb->trace=SKIP;
	Tracing_Set(SKIP|LEAP);
	return TRACE_OK;
      }
    break;

  case 'S':
    Maybe_De_Skip(option,andb);
    if(option & SKIP_OPTION)
      {
	trace_suspend=0;
	andb->trace=SKIP;
	Tracing_Set(SKIP);
	return TRACE_OK;
      }
    break;

  case 't':
    if(option & OLD_SKIP_OPTION)
      {
	Tracing_Set(SKIP|LEAP);
	return TRACE_OK;
      }
    break;

  case 'T':
    if(option & OLD_SKIP_OPTION)
      {
	Tracing_Set(SKIP);
	return TRACE_OK;
      }
    break;

  case '+':
    if(option & SPY_OPTION)
      {
	spy_def_debug(def);
	goto top;
      }
    break;
    
  case '-':
    if(option & SPY_OPTION)
      {
	nospy_def_debug(def);
	goto top;
      }
    break;

  case '*':
    if(option & (MONITOR_OPTION|MONITOR_C_OPTION))
      {
	if(chb->trace & MONITOR)
	  {
	    printf("                    Already monitored\n");	  
	    goto top1;
	  }
	tracing |= MLEAP;
	chb->trace |=MONITOR;
	goto top;
      }
    if(option & (MONITOR_CALL_OPTION|MONITOR_CALL_C_OPTION))
      {
	if(andb->trace & MONITOR)
	  {
	    printf("                    Already monitored\n");	  
	    goto top1;
	  }
	andb->trace=MONITOR;
	tracing |= MLEAP;
	goto top;
      }
    break;

  case '/':
    if(option & (MONITOR_OPTION|MONITOR_C_OPTION))
      {
	if(chb->trace & MONITOR)
	  {
	    chb->trace &= ~MONITOR;
	    goto top;
	  }
	printf("                    Not monitored\n");
	goto top1;
      }
    break;

  case '!':
    if(option & MONITOR_OPTION)
      {
	if(chb->trace & MONITOR_CHILDREN)
	  {
	    printf("                    Children already monitored\n");
	    goto top1;
	  }
	tracing |= MLEAP;
	chb->trace |= MONITOR_CHILDREN;
	goto top;
      }
    if(option & MONITOR_CALL_OPTION)
      {
	if(andb->trace & MONITOR_CHILDREN)
	  {
	    printf("                    Children already monitored\n");
	    goto top1;
	  }
	tracing |= MLEAP;
	andb->trace |= MONITOR_CHILDREN;
	goto top;
      }
    break;

  case '|':
    if(option & MONITOR_OPTION)
      {
	if(chb->trace & MONITOR_CHILDREN)
	  {
	    chb->trace &= ~MONITOR_CHILDREN;
	    goto top;
	  }
	printf("                    Children not monitored\n");
	goto top1;
      }
    break;

  case '#':
    no_monitor_and_box(andb);
    goto top;
    
  case 'u':
    Maybe_De_Skip(option,andb);
    tracing=TLEAP|CREEP;
    return TRACE_OK;

  case 'U':
    Maybe_De_Skip(option,andb);
    tracing=TLEAP|LEAP;
    return TRACE_OK;

  case 'c':
  case '\n':
    Maybe_De_Skip(option,andb);
    Tracing_Set(CREEP);
    return TRACE_OK;

  case 'f':
     filter=filter_menu();
     goto top;

  case 'x':
     manual_update();
     goto top;

  case 'D':
    trace_full_cont=FALSE;
    printf("\n");
    display_configuration(andb,trace_insert,chb);
    printf("\n");
    goto top;

  case 'E':
    trace_full_cont=TRUE;
    printf("\n");
    display_configuration(andb,trace_insert,chb);
    printf("\n");
    goto top;

  case 'A':
    printf("\n");
    display_flat_andbox(andb,trace_insert,chb);
    printf("\n\n");
    goto top;
      
  case 'P':
    full_trace_path(andb);
    goto top;

  case 'w':
    return TRACE_PRINT;

  case 'a':
    tracing = NOTRACE;
    debugging = NOTRACE;
    longjmp(toplevenv,1);

  case 'h':
  case '?':
    printf("\
	Debugging options:\n\
	<cr> creep              c    creep\n");
    if(option & SKIP_OPTION)
    printf("\
        s    skip               S    skip all\n");
    if(option & OLD_SKIP_OPTION)
    printf("\
        t    continue skip      T    continue skip all\n");
    printf("\
	l    leap	        m    monitor  stop\n\
        u    unleash            U    unleash, show only spied\n\
	n    nodebug            a    abort\n");
    if(option & SPY_OPTION)
    printf("\
	+    spy this           -    nospy this\n");
    if(option & (MONITOR_OPTION|MONITOR_CALL_OPTION|MONITOR_CALL_C_OPTION|
		 MONITOR_C_OPTION))
    printf("\
        *    monitor this\n");
    if(option & (MONITOR_OPTION|MONITOR_C_OPTION))
    printf("\
         /    nomonitor this\n");
    if(option & (MONITOR_OPTION|MONITOR_CALL_OPTION))
    printf("\
        !    monitor children  \n");
    if(option & (MONITOR_OPTION))
    printf("\
        |    nomonitor children\n");
    printf("\
        #    nomonitor and-box\n\
        x    manual update      f    filter menu\n\
	A    show andbox	D    show configuration\n\
	P    show path          E    show full configuration\n\
	w    write goal\n\
	?    help               h    help\n\n");
    goto top;

  default:
    break;
  }
 printf("                    Not a valid option\n");
 goto top;
}
  

void no_monitor_and_box(andb)
andbox *andb;
{
  printf("NOT IMPLEMENTED\n");
  return;
}

void manual_update()
{
  printf("SKIP: When using the skip options the debugger may be entered  before the\n");
  printf("      executed goal has finished (spypoints and S option or monitors).\n");
  printf("      If you now choose the t or T-option you will continue the original skip\n");
  printf("      If you now choose the s or S-option you will start a new skip\n");
  printf("            (loosing information on the old skip)\n");
  return;
}


unsigned int init_filter()
{
  unsigned int f,i;
  for(f=0,i=0;i<=NO_TRACE_FLAGS;i++)
    f |= 1<<i;
  return f;
}


unsigned int filter_menu()
{
  unsigned int f,f1,i;

  f=filter;

 top:
  printf(" filter: ");      
  if(readstring(tracecom) == 0) {
    printf("\n");
    tracecom[0] = '\n';
    tracecom[1] = 0;
  }
  switch(tracecom[0]) {
    case '+':
      switch(tracecom[1]) {
         case 'A':
	  f |= Goal_Related; 
	  goto top;
         case 'a':
	  i=tracecom[2];
	  if((i<'1')|| (i>'5')) goto top;
	  f |= 1<<(i-1-'0');
	  goto top;
       	 case 'G':
	  f |= Guard_Related; 
	  goto top;
         case 'g':
	  i=tracecom[2];
	  if((i<'1')|| (i>'4')) goto top;
	  f |= 1<<(4+i-'0');
	  goto top;
         case 'M':
	  f |= Movement_Related; 
	  goto top;
         case 'm':
	  i=tracecom[2];
	  if((i<'1')|| (i>'2')) goto top;
	  f |= 1<<(8+i-'0');
	  goto top;
       	 case 'P':
	  f |= Promotion_Related;
	  goto top;
         case 'p':
	  i=tracecom[2];
	  if((i<'1')|| (i>'2')) goto top;
	  f |= 1<<(10+i-'0');
	  goto top;
       	 case 'N':
	  f |= Nondeterminism_Related;
	  goto top;
         case 'n':
	  i=tracecom[2];
	  if((i<'1')|| (i>'2')) goto top;
	  f |= 1<<(12+i-'0');
	  goto top;
	default:
	  goto top;
	}
      break;
    case '-':
      switch(tracecom[1]) {
         case 'A':
	  f &= ~(Goal_Related);
	  goto top;
         case 'a':
	  i=tracecom[2];
	  if((i<'1')|| (i>'5')) goto top;
	  f &= ~(1<<(i-1-'0'));
	  goto top;
       	 case 'G':
	  f &= ~(Guard_Related);
	  goto top;
         case 'g':
	  i=tracecom[2];
	  if((i<'1')|| (i>'4')) goto top;
	  f &= ~(1<<(4+i-'0'));
	  goto top;
         case 'M':
	  f &= ~(Movement_Related);
	  goto top;
         case 'm':
	  i=tracecom[2];
	  if((i<'1')|| (i>'2')) goto top;
	  f &= ~(1<<(8+i-'0'));
	  goto top;
       	 case 'P':
	  f &= ~(Promotion_Related);
	  goto top;
         case 'p':
	  i=tracecom[2];
	  if((i<'1')|| (i>'2')) goto top;
	  f &= ~(1<<(10+i-'0'));
	  goto top;
       	 case 'N':
	  f &= ~(Nondeterminism_Related);
	  goto top;
         case 'n':
	  i=tracecom[2];
	  if((i<'1')|| (i>'2')) goto top;
	  f &= ~(1<<(12+i-'0'));
	  goto top;
	default:
	  goto top;
	}
      break;
    case 'a':
      f = init_filter();
      break;
    case 'n':
      f = 0;
      break;
    case '\n':
      return(f);
    case 's':
      f1=init_filter();
      if(f==f1) {printf("all\n");break;}
      if(f==0) {printf("none\n");break;}
      printf("Goal-related: ");
      if ((f & Goal_Related) == Goal_Related) 
	printf("all");
      else if ((f & Goal_Related) == 0)
	printf("none");
      else
	{
	  if(f & TRACE_CALL_GOAL) printf("Call ");
	  if(f & TRACE_EXIT_GOAL) printf("Exit ");
	  if(f & TRACE_SUSPEND_GOAL) printf("Suspend ");
	  if(f & TRACE_REENTER_GOAL) printf("Reenter ");
	  if(f & TRACE_FAIL_GOAL) printf("Fail ");
	}
      printf("\n");
      printf("Guard-related: ");
      if ((f & Guard_Related) == Guard_Related)
	printf("all");
      else if ((f & Guard_Related) == 0)
	printf("none");
      else
	{
	  if(f & TRACE_CREATE_GUARD) printf("[Create] ");
	  if(f & TRACE_FAIL_GUARD) printf("[Fail] ");
	  if(f & TRACE_SUSPEND_GUARD) printf("[Suspend] ");
	  if(f & TRACE_WAKE_GUARD) printf("[Wake] ");
	}
      printf("\n");
      printf("Movement-related: ");
      if ((f & Movement_Related) == Movement_Related)
	printf("all");
      else if ((f & Movement_Related) == 0)
	printf("none");
      else
	{
	  if(f & TRACE_UP_GUARD) printf("[Up] ");
	  if(f & TRACE_DOWN_GUARD) printf("[Down] ");
	}
      printf("\n");
      printf("Promotion-related: ");
      if ((f & Promotion_Related) == Promotion_Related)
	printf("all");
      if ((f & Promotion_Related) == 0)
	printf("none");
      else
	{
	  if(f & TRACE_NOISY_PRUNING_GUARD) printf("[Noisy Promote] ");	  
	  if(f & TRACE_PROMOTE_GUARD) printf("[Promote] ");	  
	}
      printf("\n");
      printf("Nondeterminism-related: ");
      if ((f & Nondeterminism_Related) == Nondeterminism_Related)
	printf("all");
      else if ((f & Nondeterminism_Related) == 0)
	printf("none");
      else
	{
	  if(f & TRACE_NON_DET_PRE) printf("Nondet-Pre ");	  	  
	  if(f & TRACE_NON_DET_POST) printf("Nondet-Post ");	  	  
	}
      printf("\n");
      break;
    case 'h':
    case '?':
      printf("\
      Filter options:\n\
	a    all ports\n\
        n    no ports\n\
        s    show\n\
        +N   activate port or port group \n\
        -N   deactivate port or port group \n\
	?    help\n\
 	h    help\n\
        <cr> finish\n\
             Ports:\n\
                 Port Group A: Atomic Goal-related:\n\
                   a1: Call\n\
                   a2: Exit\n\
                   a3: Suspend \n\
                   a4: Reenter   \n\
                   a5: Fail \n\
                Port Group G: Guard-related:\n\
                   g1: [Create]\n\
                   g2: [Fail]\n\
                   g3: [Suspend]\n\
                   g4: [Wake]\n\
                Port Group M: Movement-related:\n\
	           m1: [Up]\n\
	           m2: [Down]\n\
                Port Group P: Promotion-related:\n\
                   p1: [Noisy Promote]\n\
                   p2: [Promote] \n\
                Port Group N: Nondeterminism-related:\n\
                   n1: Nondet-Pre\n\
                   n2: Nondet-Post\n\n"
	   );
      break;
    default:
      printf("Not a valid option \n");
      break;
    }
  goto top;
}

/*
void trace_path(andb,n)
   andbox *andb; int n;
{
    choicebox *chb;

  if(andb == NULL)
    {
      printf("    ");
      if(n>5) return;
      trace_path(andb,n+1);
      return;
    }
  if(n > 5) {
      printf("....");
      return;
    }
  chb =  andb->father;

  trace_path(chb->father, n+1);
  if(chb->def != NULL) printf("%3.3s.", AtmPname(chb->def->name));
  else printf("    ");
}
*/

void trace_printexit(cont, tsiz)
     andcont *cont;
     int tsiz;
{
  Term *areg;
  int arity;

  if(cont->def==NULL) 
    {
      if(!debugging)
	{
	 printf("ROOT\n");
	 return;
       }
      FatalError("printexit\n");
    }

  printf("%s",AtmPname(cont->def->name));
  arity=cont->def->arity;
  areg=cont->choice_cont->arg;

  if(arity>0)
    {
      if(arity > 0) {
	int i;
	printf("(");
	for(i = 0 ; i != arity ; i++) {
	  display_term(areg[i], tsiz);
	  if(i != arity - 1) printf(", ");
	}
	printf(")");
      }
      return;
    }
}

void trace_printcall(def, areg, tsiz)
     predicate *def;
     Term *areg;
     int tsiz;
{
  char *name;
  int 	arity;
  
  if(def != NULL)
    name = AtmPname(def->name);
  else
    name = "ROOT";
  printf("%s", name);
  

  if(def != NULL)
    arity = def->arity;
  else
    arity = 0;
  
  if(arity > 0) {
    int i;

    printf("(");
    for(i = 0 ; i != def->arity ; i++) {
      display_term(areg[i],tsiz);
      if(i != def->arity - 1) printf(", ");
    }
    printf(")");
  }
}

void trace_printgoal(chb, tsiz)
     choicebox *chb;
     int tsiz;
{
  char *name;
  int 	arity;

  if(chb->def != NULL)
    name = AtmPname(chb->def->name);
  else
    name = "ROOT";
  printf("%s", name);
  

  if(chb->cont != NULL)
    arity = chb->cont->arity;
  else
    arity = 0;
  
  if(arity > 0) {
    int i;

    printf("(");
    for(i = 0 ; i != arity ; i++) {
      display_term(chb->cont->arg[i], tsiz);
      if(i != arity - 1) printf(", ");
    }
    printf(")");
  }
}


void trace_printsusp(ab)
    andbox *ab;
{
  /* Display constraints. */
  constraint *constr;
  if(ab->constr != NULL) {
    for(constr = ab->constr; constr != NULL; constr = constr->next) {
      display_constraint(constr);
      if(constr->next!= NULL)
	printf(", ");
    }
    if(ab->tried == NULL)
      printf(" ");
  }
}


void full_trace_path(andb)
      andbox *andb;
{
  choicebox *chb;
  char *sp,*sp1;

  if(andb == NULL)
    return;

  chb =  andb->father;

  full_trace_path(chb->father);

  if(chb->def != NULL) {
    if(chb->def->spypoint != NOSPY) sp = "+";
    else sp = " ";
    if(chb->trace & (MONITOR|MONITOR_CHILDREN)) sp1="*";
    else sp1=" ";
    printf("%s%s%s/%d\n", sp, sp1, AtmPname(chb->def->name), chb->def->arity);
  }
}


void spy_def(def)
      predicate *def;
{
  Atom name = def->name;
  int arity = def->arity;
  
  if (def->spypoint != NOSPY)
    printf("{There already is a spypoint on %s/%d}\n", AtmPname(name), arity);
  else 
    {
      def->spypoint = spypoints;
      spypoints = def;
    }
  printf("{Spypoint placed on %s/%d}\n", AtmPname(name), arity);
}



void spy_def_debug(def)
      predicate *def;
{
  Atom name = def->name;
  int arity = def->arity;
  
  if (def->spypoint != NOSPY)
    printf("                    There already is a spypoint on %s/%d}", AtmPname(name), arity);
  else 
    {
      def->spypoint = spypoints;
      spypoints = def;
    }
}


void nospy_def(def)
      predicate *def;
{
  predicate **d;
  Atom name = def->name;
  int arity = def->arity;

  if (def->spypoint == NOSPY) {
    printf("{There is no spypoint on %s/%d}\n", AtmPname(name), arity);
  }
  else {
    for (d = (struct predicate **) &spypoints; *d != (struct predicate *) &spypoints; 
	 d = &((*d)->spypoint)) {
      if (*d == def) {
	*d = def->spypoint;
	def->spypoint = NOSPY;
	printf("{Spypoint removed from %s/%d}\n", AtmPname(name), arity);
	return;
      }
    }
    Error("error in spy");
  }
}

void nospyall()
{
  predicate *d,*d1;

  if (spypoints == (predicate *)&spypoints)
    printf("{There are no spypoints}\n");
  else 
    {
      for (d = spypoints; d != (struct predicate *) &spypoints; )
	{
	  d1 = d->spypoint;
	  d->spypoint = NOSPY;
	  d=d1;
	}
      spypoints = (predicate *)&spypoints;
      printf("All spypoints removed:\n");
    }
}

void nospy_def_debug(def)
      predicate *def;
{
  predicate **d;
  Atom name = def->name;
  int arity = def->arity;

  if (def->spypoint == NOSPY) {
    printf("                    {There is no spypoint on %s/%d}", AtmPname(name), arity);
  }
  else {
    for (d = (struct predicate **) &spypoints; *d != (struct predicate *) &spypoints; 
	 d = &((*d)->spypoint)) {
      if (*d == def) {
	*d = def->spypoint;
	def->spypoint = NOSPY;
	return;
      }
    }
    Error("error in spy");
  }
}

void debugging_aux()
{
  predicate *d;
  
  if (debugging & CREEP)
    printf("{The debugger will first creep -- showing everything (trace)}\n");
  else if (debugging & LEAP)
    printf("{The debugger will first leap -- showing spypoints (debug)}\n");
  else
    printf("{The debugger is switched off}\n");
    
  if(spypoints == (predicate *) &(spypoints))
    printf("{There are no spypoints}\n");
  else {
    printf("Spypoints:\n");
    for (d = spypoints; d != (predicate *) &(spypoints); d = d->spypoint)
      printf("    %s/%d\n", AtmPname(d->name), d->arity);
  }
}


#endif


void initialize_trace() {

#ifdef TRACE
  identity = 0;
#endif

  functor_slash_2 = store_functor(store_atom("/"),2);

  define("trace", akl_trace, 0);
  define("notrace", akl_notrace, 0);
  define("sys_notrace", akl_sys_notrace, 0);
  define("sys_check_trace", akl_sys_check_trace, 0);
  define("debug", akl_debug, 0);
  define("nodebug", akl_nodebug, 0);
  define("failcatch", akl_failcatch, 0);
  define("nofailcatch", akl_nofailcatch, 0);
  define("spy", akl_spy, 1);
  define("nospy", akl_nospy, 1);
  define("nospyall", akl_nospyall, 0);
  define("debugging", akl_debugging, 0);
  define("$is_debugging", akl_is_debugging, 0);
  define("$is_tracing", akl_is_tracing, 0);
  define("suspend_depth",akl_suspend_depth,1);
  define("trace_set_print_depth", akl_trace_set_print_depth, 1);
  define("trace_get_print_depth", akl_trace_get_print_depth, 1);
}
