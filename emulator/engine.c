/* $Id: engine.c,v 1.90 1994/05/17 13:16:22 jm Exp $  */

#define Register register

#include "include.h"
#include "instructions.h"
#include "term.h"
#include "tree.h"
#include "copy.h"
#include "predicate.h"

#include "exstate.h"
#include "error.h"

#include "bam.h"

#include "storage.h"
#include "engine.h"
#include "unify.h"
#include "gc.h"
#include "examine.h"
#include "candidate.h"
#include "debug.h"
#include "trace.h"
#include "initial.h"
#include "names.h"
#include "config.h"
#include "intrpt.h"

#include "aggregate.h"
#include "regdefs.h"
#include "port.h"
#include "abstraction.h"

#include "decodeinstr.h"
#include "instrdefs.h"



#define DUMMY_CODE	0

#ifdef TRACE
extern readstring();
extern int tracing;
char tracecom[500];
#endif


#ifdef THREADED_CODE

address *instr_label_table = NULL;
long instr_label_table_length = 0;

enum_opcode code_to_enum(op)
    opcode op;
{
  int i = instr_label_table_length;
  while (i--) {
    if (op == instr_label_table[i]) {
      return i;
    }
  }
  return 0; /* is never reached? */
}

#endif


#define reinstall(andb)	install(andb,exs)


#define SaveRegisters\
{\
  exs->andb = andb;\
  exs->insert = insert;\
}

#define RestoreRegisters\
{\
  andb = exs->andb;\
  insert = exs->insert;\
  chb = andb->father;\
  yreg = andb->cont->yreg;\
}


#define CheckExceptions \
{\
if(interruptflag) {\
  interruptflag = NO_INTRPT;\
  longjmp(toplevenv,1);\
  }\
if(gcflag) {\
  exs->arity = arity;\
  exs->andb = andb;\
  exs->insert = insert;\
  gc(exs);\
  andb = exs->andb;\
  insert = exs->insert;\
  chb = andb->father;\
  yreg = andb->cont->yreg;\
  }\
}    


/* The heart of the AKL engine! */
void tengine(exs, initcodes)
     exstate *exs;
     int initcodes;
{
#if 0
  register heap		*heapcurrent;
#endif

  register heap		*heaplimit;	/* Its companion */

  register code		*pc REGISTER3;	/* The code pointer */

  register opcode	op REGISTER4;	/* The next opcode */
 
  register Term 	*areg REGISTER5; 	/* The argument registers */
  
  register Term 	*yreg REGISTER6 = NULL;	/* Current environment */

  register Term 	*str = NULL;  		/* Structure pointer */

  register Term 	*cont_pc;  		/* Interface CALL/EXECUTE */

  register Term		abstr;			/* Interface APPLY */
  
  register int		write_mode REGISTER1 = 0;

#ifdef BAM
  register try_stack_cell *try_top,*try_bottom;
  register cons_stack_cell *cons_top,*cons_bottom;
  int alias_flag = ALIAS_DEFAULT;
#endif

  register andbox 	*andb REGISTER2;	/* The current and box */

  register choicebox 	*chb = NULL;	/* The current choice box */

  register choicebox 	*insert;/* The choicebox insertion point */

  register andbox	*wand;	/* An andbox that must be woken */

  register choicebox	*rchb;	/* A choicebox that must be retried */

  register andbox	*cand;	/* A candidate for choice splitting */
  
  predicate		*def;	/* The predicate being called */

  u16			arity;	/* The arity of the called predicate */

#ifdef THREADED_CODE
#ifdef Item
#undef Item
#endif
#define Item(x)	&&CaseLabel(x),

  static address label_table[] =
  {
#include "opcodes.h"
  };
#endif


  if (initcodes)
  {
#ifdef THREADED_CODE
    instr_label_table = label_table;
    instr_label_table_length = sizeof(label_table)/sizeof(label_table[0]);
#endif
    return;
  }


  /* initialize the registers from the exstate */

#ifdef BAM
  cons_top = cons_bottom = exs->cons_stack.current;
  try_top = try_bottom = exs->try_stack.current;
#endif

  andb = exs->andb;
  insert = exs->insert;
  areg = exs->areg;
  pc = exs->pc;
  FetchOpCode(0);		/* Read the next opcode */
  arity = exs->arity;
  def = exs->def;

  /* dummy "forever" loop to fool the C compiler to move out invariants */
  for(;;) {

  goto choicehandler;
  
  /**********************************************************************
   *
   * Instruction DECODING
   *
   * Global
   *
   *   andb:  the currently installed andbox
   *   chb:   its parent choice-box
   *   pc:    as a cache for andb->cont->cont
   *   yreg:  as a cache for andb->cont->yreg
   *   insert the choicebox insertion point
   *	     
   *
   * Local (do not survive changing blocks)
   *
   *   areg:  to provide a call with its arguments
   *   def:   between call and try (for debugging)
   *   arity: between call and try (could be removed)
   *   str:   for compiled unification
   *
   */

#ifndef THREADED_CODE
 read_instr_dispatch:

  DisplayReadInstr;

#endif

  SwitchTo(op) {
  CaseLabel(SWITCH_ON_TERM):
    StartInstruction(SWITCH_ON_TERM);
    DerefJumpIfVar(X(0), varcase);
    SwitchTag(X(0),badcase,badcase,immcase,bigcase,fltcase,lstcase,strcase,gencase);
 badcase:
    FatalError("switch_on_term: tags are broken");

 immcase:
    if (IMMtermIsNUM(X(0))) {
      Branch(SotLiOffset);
    } else {
      Branch(SotLaOffset);		/* ATM */
    }
 bigcase:
      Branch(SotLiOffset);
 fltcase:
      Branch(SotLdOffset);
 lstcase:
      Branch(SotLlOffset);
 strcase:
      Branch(SotLsOffset);
 gencase:
      /* temporary, since UVA is the most general case we
       * jump to the try instructions for variables. In the
       * future is possible to let the term switch handle
       * even generics. One could even have a switch_on_
       * generic but that seems to be over-kill
       */
      Branch(SotLaOffset);
 varcase:
      Branch(SotLvOffset);

  CaseLabel(TRY_SINGLE):
    /* Build a choice-box and set up execution for
     * the only clause. 
     */

    FetchTryBranchLabel(pc);
    FetchOpCode(0);	/* prefetch opcode */

    PushContext(exs);

    MakeChoicebox(chb,def,andb);

    MakeEmptyChoiceCont(chb,areg,arity);
    
    InsertChoicebox(chb,insert,andb);

    MakeAndbox(andb, chb);
    
    insert = NULL;
    
    chb->tried = andb;

    TRACE_Create_First_Guard(andb,chb);
    
    NextOp();
	
  CaseLabel(TRY):
    /* Build a choice-box and set up execution for
     * the first clause. 
     */
    {
      code *label;
      FetchTryBranchLabel(label);
      
      PushContext(exs);

      MakeChoicebox(chb,def,andb);

      pc += TryInstructionSize;

      MakeChoiceCont(chb,pc,areg,arity);

      InsertChoicebox(chb,insert,andb);
      
      pc = label;
      FetchOpCode(0);	/* prefetch opcode */

      MakeAndbox(andb,chb);
      
      insert = NULL;
	
      chb->tried = andb;

      TRACE_Create_First_Guard(andb,chb);

      NextOp();
    }
	
  CaseLabel(RETRY):
    /* try the next clause */
    {
      andbox *prev;
      code *label;

      prev = andb;
      
      FetchTryBranchLabel(label);

      RestoreAreg(chb,areg);

      pc += TryInstructionSize;

      SetChoiceContLabel(chb,pc);
      
      pc = label;
      FetchOpCode(0);	/* prefetch opcode */

      MakeAndbox(andb,chb);

      InsertAndbox(andb, prev, chb);

      insert = NULL;

      TRACE_Create_Guard(andb);

      NextOp();
    }
  CaseLabel(TRUST):
    /* this is the last clause */
    {
      andbox *prev;

      prev = andb;
      
      FetchTryBranchLabel(pc);
      FetchOpCode(0);	/* prefetch opcode */

      RestoreAreg(chb,areg);
    
      RemoveChoiceCont(chb);
    
      MakeAndbox(andb,chb);

      InsertAndbox(andb,prev,chb);

      insert = NULL;
	
      TRACE_Create_Guard(andb);

      NextOp();
    }

  /* The guard instructions are the last instructions of the guard */

  CaseLabel(GUARD_UNORDER):                   /* ATTENTION */
    if(Quiet(andb) && EmptyTrail(exs) && Solved(andb)) {
	andbox *promoted;
	indx ai,a1i,a2i;
	Term a;

	Instr_Indices_3(GUARD_UNORDER, ai, a1i, a2i);

	promoted = andb;
	andb = chb->father;

	/* Remove current and-box from list of alternatives */
	if(promoted->previous != NULL)
	  promoted->previous->next = promoted->next;
	else
	  chb->tried = promoted->next;
	if(promoted->next != NULL)
	  promoted->next->previous = promoted->previous;

	/* Kill the current andbox */
	promoted->status = DEAD;

	/* Promote the andbox */
	PromoteAndbox(promoted,andb);

	promoted->cont->next = andb->cont;
	andb->cont = promoted->cont;

	andb->cont->label = pc;

	Deref(a, Yb(ai));
	MakeVariableTerm(Yb(a2i), andb);
	if(IsCollectObj(a)) {
	  Yb(a1i) = CollectObj(a)->value;
	  CollectObj(a)->value = Yb(a2i);
	}
	else
	  goto fail;

	/* If sibling solved and deterministic, try it */
	if(chb->tried != NULL && chb->tried->next == NULL &&
	   chb->tried->tried == NULL) {
	  andbox *failed;
	  andb = chb->tried;
	  failed = install(andb,exs);
	  if(failed != NULL) {
	    andb = failed;
	    goto fail;
	  }
	  pc = andb->cont->label;
	  FetchOpCode(0);
	  yreg = andb->cont->yreg;
	  NextOp();      
	} 
	goto choicehandler;
      }
    goto guardhandler;

    CaseLabel(GUARD_ORDER):                   /* ATTENTION */
    if(Quiet(andb) && EmptyTrail(exs) && Solved(andb) && Leftmost(andb)) {
	andbox *promoted;
	indx ai,a1i,a2i;
	Term a;

	Instr_Indices_3(GUARD_ORDER, ai, a1i, a2i);

	promoted = andb;
	andb = chb->father;

	/* Remove current and-box from list of alternatives */
	if(promoted->previous != NULL)
	  promoted->previous->next = promoted->next;
	else
	  chb->tried = promoted->next;
	if(promoted->next != NULL)
	  promoted->next->previous = promoted->previous;

	/* Kill the current andbox */
	promoted->status = DEAD;

	/* Promote the andbox */
	PromoteAndbox(promoted,andb);

	promoted->cont->next = andb->cont;
	andb->cont = promoted->cont;

	andb->cont->label = pc;

	Deref(a, Yb(ai));
	MakeVariableTerm(Yb(a2i), andb);
	if(IsCollectObj(a)) {
	  Yb(a1i) = CollectObj(a)->value;
	  CollectObj(a)->value = Yb(a2i);
	}
	else
	  goto fail;

	/* If sibling solved, try it */
	if(chb->tried != NULL && chb->tried->tried == NULL) {
	  andbox *failed;
	  andb = chb->tried;
	  failed = install(andb,exs);
	  if(failed != NULL) {
	    andb = failed;
	    goto fail;
	  }
	  pc = andb->cont->label;
	  FetchOpCode(0);
	  yreg = andb->cont->yreg;
	  NextOp();      
	} 
	goto choicehandler;
      }
    goto guardhandler;

  CaseLabel(GUARD_UNIT):
    /* If the guard is quiet and leftmost then we should promote
     * otherwise suspend.
     */
    if(Solved(andb) && Quiet(andb) && EmptyTrail(exs)) {
      if(Leftmost(andb)) {
	andbox	*promoted;
	indx ai, a1i;
	Term a;

	Instr_Index_Index(GUARD_UNIT, ai, a1i);

	/* ATTENTION */
	
	PromoteDebugPrint("promote");

	promoted = andb;
	andb = chb->father;
	PromoteAndbox(promoted,andb);

	RestoreContext(exs);

	PopContext(exs);

	KillAll(chb);

	RemoveChoicebox(chb,insert,andb);

	chb = andb->father;
	promoted->cont->next = andb->cont;
	andb->cont = promoted->cont;

	Deref(a,Yb(ai));
	if(IsCollectObj(a))
	  Yb(a1i) = CollectObj(a)->value;
	else
	  goto fail;

	NextOp();
      }
      RemoveChoiceCont(chb);
      KillLeft(andb);
    }
    goto guardhandler;

  CaseLabel(GUARD_COND):
    /* If the guard is quiet and leftmost then we should promote
     * otherwise suspend.
     */
    if(Solved(andb) && Quiet(andb) && EmptyTrail(exs)) {
      if(Leftmost(andb)) {
	andbox	*promoted;
	Instr_None(GUARD_COND);

	TRACE_Promote_Guard(andb);
	
	PromoteDebugPrint("promote");

	promoted = andb;
	andb = chb->father;
	PromoteAndbox(promoted,andb);

	RestoreContext(exs);

	PopContext(exs);

	KillAll(chb);

	RemoveChoicebox(chb,insert,andb);

	chb = andb->father;
	promoted->cont->next = andb->cont;
	andb->cont = promoted->cont;
	NextOp();
      }
      RemoveChoiceCont(chb);
      KillLeft(andb);
    }
    goto guardhandler;

  CaseLabel(GUARD_CUT):
    /* If the guard is solved, quiet and leftmost or solved
     * and determinstic then it should promote otherwise suspend.
     */
    if(Solved(andb)) {
      if (Quiet(andb) && EmptyTrail(exs)) {
	if(Leftmost(andb)) {
	  andbox	*promoted;
	  Instr_None(GUARD_CUT);

	  TRACE_Promote_Guard(andb);
	  PromoteDebugPrint("promote");

	  promoted = andb;
	  andb = chb->father;
	  PromoteAndbox(promoted,andb);

	  RestoreContext(exs);

	  PopContext(exs);

	  KillAll(chb);

	  RemoveChoicebox(chb,insert,andb);

	  chb = andb->father;
	  promoted->cont->next = andb->cont;
	  andb->cont = promoted->cont;
	  NextOp();
	}
	RemoveChoiceCont(chb);
	KillLeft(andb);
        goto guardhandler;
      }
      if(Last(chb,andb)) {
	Instr_None(GUARD_CUT);
	TRACE_Noisy_Pruning(andb);   
	goto promotion;
      }
      goto guardhandler;
    }
    goto guardhandler;

  CaseLabel(GUARD_COMMIT):
    /* If the guard is quiet then we should promote otherwise suspend.
     */
    if(Quiet(andb) && EmptyTrail(exs) && Solved(andb)) { 
      andbox	*promoted;
      Instr_None(GUARD_COMMIT);

      TRACE_Promote_Guard(andb);
      PromoteDebugPrint("promote");

      promoted = andb;
      andb = chb->father;
      PromoteAndbox(promoted,andb);

      RestoreContext(exs);

      PopContext(exs);

      KillAll(chb);

      RemoveChoicebox(chb,insert,andb);

      chb = andb->father;
      promoted->cont->next = andb->cont;
      andb->cont = promoted->cont;
      NextOp();
    } 
    goto guardhandler;

  CaseLabel(GUARD_WAIT):
	
    if(Solved(andb) && Last(chb,andb) &&
       GetOpCode(pc) == EnumToCode(GUARD_WAIT)) {
      Instr_None(GUARD_WAIT);
      TRACE_Promote_Guard(andb); 
      goto promotion;
    }

    goto guardhandler;
	
  CaseLabel(GUARD_HALT):

    goto mainhandler;
    
  CaseLabel(ALLOCATE):
    /* First instruction in the guard, should follow try, retry, trust.
     * Sets up the local environment, that is, allocates space for the
     * locally defined variables. The local environment is stored in an
     * and-continuation.
     */
    {
      indx ysize;
      Instr_Size(ALLOCATE,ysize);
      MakeAndCont(andb,NULL,andb->cont,ysize);
      yreg = andb->cont->yreg;
	
      NextOp();
    }
  CaseLabel(DEALLOCATE):
    /* Pop the current continuation.
     */
    {
      Instr_None(DEALLOCATE);
      TRACE_Exit_Cont(andb,insert);

      andb->cont = andb->cont->next;
      yreg = andb->cont->yreg;

      NextOp();
    }
  CaseLabel(CALL):

    /* We are now supposed to have all arguments in registers
     * and are ready to call up a new predicate. We must however
     * save a pointer to where we leave this predicate, a
     * continuation.
     */
	
    /* Get arguments to CALL */
	
    Instr_Pred(CALL,def);

    /* call definition */

  
  call_definition:
  
    /* save the program counter in the continuation */
    andb->cont->label = pc;

    cont_pc = pc;

  common_call_definition:

    /* Store the arity, needed when the choicebox is created */
    
    arity = def->arity;

    /* this is an interface to BAM */
    heaplimit = heapend-1024;

    switch (def->enter_instruction) {
    case ENTER_EMULATED:
      CheckExceptions;
      TRACE_Call(def,areg,andb,insert);
      pc = def->code.incoreinfo;
      break;
    case ENTER_C:
      TRACE_Call_C(def,areg,andb,insert);
      SaveRegisters;
      switch((def->code.cinfo)(exs)) {
      case FALSE:
	RestoreRegisters;
	TRACE_Fail_Call_C(def,areg,andb,insert);
	goto fail;
      case TRUE:
	RestoreRegisters;	
	TRACE_Exit_Call_C(def,areg,andb,insert);
        pc = cont_pc;
	break;
      case SUSPEND:
	{
	  choicebox *new;

	  RestoreRegisters;

          MakeChoicebox(new,def,andb);

          MakeChoiceCinfoCont(new, (def->code.cinfo),areg,arity);

          InsertChoicebox(new,insert,andb);

	  insert = new;
	  
	  SuspendBuiltin(exs,new);

  	  TRACE_Suspend_Goal_C(new);
	}

	pc = cont_pc;
	
	break;
      case REDO:
	{
	  choicebox *new;
	  
	  RestoreRegisters;
	  MakeChoicebox(new, def, andb);
	  MakeChoiceCinfoCont(new, (def->code.cinfo), areg, arity);
	  InsertChoicebox(new, insert, andb);
	  insert = new;
	  RedoBuiltin(exs,new);
	  TRACE_Suspend_Goal_C(new);
	}
	break;
      }
      break;
    case ENTER_UNDEFINED:
      {
	predicate *truedef = (predicate *) Tad(def);
	ENGINE_ERROR_2(AtmPname(truedef->name), truedef->arity, "undefined agent")
      }

#ifdef TRACE
      tracing = CREEP;
#endif
      TRACE_Fail_Call_C(def,areg,andb,insert);

      goto fail;
    }

    FetchOpCode(0);
    NextOp();
	
  CaseLabel(EXECUTE):
    
    Instr_Pred(EXECUTE,def);

  execute_definition:  

    cont_pc = andb->cont->label;

  goto common_call_definition;

  CaseLabel(DEALLOC_EXECUTE):

    Instr_Pred(DEALLOC_EXECUTE,def);

#ifdef TRACE
    andb->cont->label=proceed;
#else
    andb->cont = andb->cont->next;
    yreg = andb->cont->yreg;
#endif

    goto execute_definition;

  CaseLabel(PROCEED):
    /* Return from a continuation-free call.
     */
    pc = andb->cont->label;
    FetchOpCode(0);

    arity = 0;
    CheckExceptions;

    NextOp();

  CaseLabel(DEALLOC_PROCEED):
    /* Just move on to the next and-continuation.
     */
    TRACE_Exit_Cont(andb,insert);

    andb->cont = andb->cont->next;
    pc = andb->cont->label;
    FetchOpCode(0);
    yreg = andb->cont->yreg;

    arity = 0;
    CheckExceptions;

    NextOp();

  CaseLabel(META_CALL):

   TRACE_Begin(andb);

    {   Register Term goal;
	indx i;
	
	Instr_Index(META_CALL,i);
	Deref(goal, Xb(i));
	
	if(IsATM(goal)) {
	  def = get_predicate(Atm(goal),0);
	  goto call_definition;
	}
	if(IsSTR(goal)) {
	  Register Structure g = Str(goal);
	  Register int j;

	  def = get_predicate(StrName(g), StrArity(g));

	  for(j=0 ; j != def->arity ; j++) 
	    GetStrArg(X(j), g, j);

	  goto call_definition;
	}
	
	ENGINE_ERROR_1(goal, "illegal goal")
	goto fail;
      }

  CaseLabel(CALL_APPLY):
    /* We are now supposed to have all arguments in registers
     * and are ready to apply an abstraction. The external 
     * arguments are copied from the abstraction to the registers.
     * We must save a pointer to where we leave this predicate, a
     * continuation.
     */

    {
      indx i;

      /* Get arguments to APPLY */
	
      Instr_Index_Size(CALL_APPLY,i,arity);

      abstr = Xb(i);

      /* save the program counter in the continuation */
      andb->cont->label = pc;

      cont_pc = pc;
      
    call_apply_definition:
      
      Deref(abstr, abstr);
      
      if(IsAbs(abstr)) {
        if( AbsArity(Abs(abstr)) != (AbsExt(Abs(abstr)) +  arity)) {
	  {
	    predicate *def = AbsDef(Abs(abstr));
	    ENGINE_ERROR_2(AtmPname(def->name), (def->arity - def->ext), "applied to wrong number of arguments")
	  }
          tracing = CREEP;
          TRACE_Fail_Apply(apply_def,abstr,arity,areg,andb, insert);
	  goto fail;
	}
	def = AbsDef(Abs(abstr));

	arity = AbsArity(Abs(abstr));

	/* load the external registers */
	{
	  int j, i, k;
	  k = AbsExt(Abs(abstr));
	  i = arity - k;
	  for(j = 0; j < k; j++) {
	    GetArg(X(i++), AbsArgRef(Abs(abstr),j));
	  }
	}
	
	heaplimit = heapend-1024;
	
	switch(def->enter_instruction) {
	case ENTER_EMULATED:
	  CheckExceptions;
	  TRACE_Call_Apply(apply_def,abstr,arity,areg,andb,insert);
	  pc = def->code.incoreinfo;
	  break;
	case ENTER_C:
          FatalError("abstraction defined as C predicate");
	case ENTER_UNDEFINED:
          FatalError("abstraction undefined");
	}
	FetchOpCode(0);
	NextOp();
      }
      if(IsVar(abstr)) {
	choicebox *new;
	Reference cdr, r;
	Term l;
	  
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
	  
	X(0) = abstr;		/* the abstraction */
	X(1) = l;		/* the list of arguments */


	def = apply_def;
	  
	MakeChoicebox(new, def, andb);
        new->type = ZIPPEDIDOO;
	MakeChoiceCont(new, (def->code.incoreinfo), areg, 2);
  
	InsertChoicebox(new,insert,andb);

	insert = new;

	r = Ref(abstr);
	if(VarIsUVA(r)) {
	  MakeBigVar(r);
	}
	MakeRecall(r,new);

	TRACE_Suspend_Goal_C(new);
	
	pc = cont_pc;
	FetchOpCode(0);
	NextOp();        
      }
      ENGINE_ERROR_1(abstr, "not abstraction")
      tracing = CREEP;      
      TRACE_Fail_Apply(apply_def,abstr,arity,areg,andb, insert);
      goto fail;
    }

	
  CaseLabel(EXECUTE_APPLY):

  execute_apply_definition:
    
    {
      indx i;

      /* Get arguments to APPLY */
	
      Instr_Index_Size(EXECUTE_APPLY,i,arity);

      abstr = Xb(i);

      cont_pc = andb->cont->label;
	
      goto call_apply_definition;
    }


  CaseLabel(DEALLOC_EXECUTE_APPLY):

#ifdef TRACE
    andb->cont->label=proceed;
#else
    andb->cont = andb->cont->next;
    yreg = andb->cont->yreg;
#endif

    goto execute_apply_definition;


  CaseLabel(EXECUTE_APPLY_LIST):
    /* The abstraction, the arity and a list of arguments are
     * all here. The arguments and the external 
     * arguments are copied to the registers.
     */

    {
      indx i1,i2;
      Term list;
      /* Get arguments to APPLY */
	
      Instr_Index_Index(EXECUTE_APPLY_LIST,i1,i2);

      abstr = Xb(i1);

      {
	Term cons;
	int i;

        list = Xb(i2);
        Deref(list, list);

	i = 0;
        cons = list;
	Deref(cons,cons);
        while(cons != NIL) {
	  if(IsLST(cons)) {
	    GetArg(X(i++), LstCarRef(Lst(cons)));
	    cons = LstCdr(Lst(cons));
	    Deref(cons,cons);
	  } else {
	    if(IsVar(cons)) {
	      choicebox *new;
	      Reference r;
	
	      X(0) = abstr;	/* the abstraction */
	      X(1) = list;	/* the list of arguments */

	      def = apply_def;
	  
	      MakeChoicebox(new, def, andb);
	      new->type = ZIPPEDIDOO;
	      MakeChoiceCont(new, (def->code.incoreinfo), areg, 2);
  
	      InsertChoicebox(new,insert,andb);

	      insert = new;

	      r = Ref(cons);
	      if(VarIsUVA(r)) {
		MakeBigVar(r);
	      }
	      MakeRecall(r,new);

	      TRACE_Suspend_Goal_C(new);

	      pc = andb->cont->label;
	      FetchOpCode(0);
              NextOp();
	    } else {
	      ENGINE_ERROR_1( list, "improper argument list")
              tracing = CREEP;
              TRACE_Fail_Apply_List(apply_def,abstr,list,andb, insert);
	      goto fail;
	    }
	  }
	}
	arity = i;
      }

      goto call_apply_definition;
    }
 
  CaseLabel(FAIL):
    goto fail;

  CaseLabel(GET_X_VARIABLE):
    {	indx n,i;
	Instr_Index_Index(GET_X_VARIABLE,n,i);
	Xb(n) = Xb(i);
	NextOp();
      }
  CaseLabel(GET_Y_VARIABLE):
    {	indx n, i;
	Instr_Index_Index(GET_Y_VARIABLE,n,i);
	Yb(n) = Xb(i);
	NextOp();
      }	
  CaseLabel(GET2_Y_VARIABLE):
    {	indx n, i;
	Instr_Index_Index_No_Prefetch(GET2_Y_VARIABLE,n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	FetchOpCode(0);
	Yb(n) = Xb(i);
	NextOp();
      }	
  CaseLabel(GET3_Y_VARIABLE):
    {	indx n, i;
	Instr_Index_Index_No_Prefetch(GET3_Y_VARIABLE,n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	FetchOpCode(0);
	Yb(n) = Xb(i);
	NextOp();
      }	
  CaseLabel(GET4_Y_VARIABLE):
    {	indx n, i;
	Instr_Index_Index_No_Prefetch(GET4_Y_VARIABLE,n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	FetchOpCode(0);
	Yb(n) = Xb(i);
	NextOp();
      }	
  CaseLabel(GET5_Y_VARIABLE):
    {	indx n, i;
	Instr_Index_Index_No_Prefetch(GET5_Y_VARIABLE,n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	Yb(n) = Xb(i);
	Next_Index_Index(n,i);
	FetchOpCode(0);
	Yb(n) = Xb(i);
	NextOp();
      }	
  CaseLabel(GET_X_VALUE):
    {	indx n,i;
	Instr_Index_Index(GET_X_VALUE,n,i);
	{
	  Term X1, Y1;
	  Deref(X1,Xb(n));
	  Deref(Y1,Xb(i));
	  UnifyDebugPrint("Derefed OK");
	  if(!unify(X1,Y1,andb,exs))
	    goto fail;
	}
	NextOp();
      }
	
  CaseLabel(GET_Y_VALUE):
    {	indx n,i;
	Instr_Index_Index(GET_Y_VALUE,n,i);
	{
	  Term X1, Y1;
	  Deref(X1,Yb(n));
	  Deref(Y1,Xb(i));
	  UnifyDebugPrint("Derefed OK");
	  if(!unify(X1,Y1,andb,exs))
	    goto fail;
	}
	NextOp();
      }
	
  CaseLabel(GET_CONSTANT):
    {
      Register Term Xi;
      indx i;
      Term c;
      Instr_Term_Index(GET_CONSTANT,c,i);
      Xi = Xb(i);
      DerefJumpIfVar(Xi, get_constant_var);
      if(IsIMM(Xi)) {
	if(Eq(Xi, c)) {
	  NextOp();
	}
      } else if(IsBIG(Xi) && IsBIG(c)) {
	if(bignum_compare(Xi, c) == 0) {
	  NextOp();
	}
      } else if(IsFLT(Xi) && IsFLT(c)) {
	if(FltVal(Flt(Xi)) == FltVal(Flt(c))) {
	  NextOp();
	}
      }
      goto fail;
  get_constant_var:
      {
	Register Reference V = Ref(Xi);
        BindVariable(exs,andb,V,c, goto fail);
	NextOp();
      }
    }
	
  CaseLabel(GET_NIL):
    {
      Register Term Xi;
      indx i;
      Instr_Index(GET_NIL,i);
      Xi = Xb(i);
      DerefJumpIfVar(Xi, get_nil_var);
      if( Eq(Xi, NIL) ) {
	NextOp();
      }
      goto fail;
  get_nil_var:
      {
	Register Reference V = Ref(Xi);
        BindVariable(exs, andb, V, NIL, goto fail);
	NextOp();
      }
    }
	
  CaseLabel(GET_STRUCTURE):
    {
      Register Term Xi;
      Term s;
      Functor f;
      indx i;
      Instr_Functor_Index(GET_STRUCTURE,f,i);
      Xi = Xb(i);
      DerefJumpIfVar(Xi, get_structure_var);
      if(IsSTR(Xi)) {
	Register Structure S = Str(Xi);
	if(StrFunctor(S) == f) {
	  str = StrArgRef(S,0);
	  NextReadOp();
	}
      }
      goto fail;

  get_structure_var:
      {
	Register Reference V = Ref(Xi);
	MakeStructTerm(s,f,andb);
	str = StrArgRef(Str(s), 0);
        BindVariable(exs,andb,V,s, goto fail);
	NextWriteOp();
      }
    }

  CaseLabel(GET_LIST):
    {
      Register Term Xi;
      Term l;
      indx i;
      Instr_Index(GET_LIST,i);
      Xi = Xb(i);
      DerefJumpIfVar(Xi, get_list_var);
      if(IsLST(Xi)) {
	str = LstCarRef(Lst(Xi));
	NextReadOp();
      }
      goto fail;
  get_list_var:
      {
	Register Reference V = Ref(Xi);
	MakeListTerm(l,andb);
        str = LstCarRef(Lst(l));
	BindVariable(exs,andb,V,l, goto fail);
        NextWriteOp();
      }
    }

  CaseLabel(GET_LIST_X0):
    if(IsREF(X(0))) {
      Register Reference X0 = Ref(X(0));
      Register Term l;
      Instr_None(GET_LIST_X0);
      MakeListTerm(l,andb);
      str = LstCarRef(Lst(l));
      BindVariable(exs,andb,X0,l,goto fail);
      NextWriteOp();
    }
    if(IsLST(X(0))) {
      str = LstCarRef(Lst(X(0)));
      Instr_None(GET_LIST_X0);
      NextReadOp();
    }
    goto fail;
	
  CaseLabel(GET_NIL_X0):
    if(IsREF(X(0))) {
      Register Reference X0 = Ref(X(0));
      Instr_None(GET_NIL_X0);
      BindVariable(exs,andb,X0,NIL,goto fail);
      NextOp();
    }
    if( Eq(X(0), NIL) ) {
      Instr_None(GET_NIL_X0);
      NextOp();
    }
    goto fail;
	
  CaseLabel(GET_STRUCTURE_X0):
    {
      Register Functor f;
      Instr_Functor(GET_STRUCTURE_X0,f);
      if(IsREF(X(0))) {
	Register Reference X0 = Ref(X(0));
	Register Term s;
	MakeStructTerm(s, f, andb);
	str = StrArgRef(Str(s),0);
	BindVariable(exs,andb,X0,s,goto fail);
	NextWriteOp();
      }
      if(StrFunctor(Str(X(0))) == f) {
	str = StrArgRef(Str(X(0)),0);
	NextReadOp();
      }
      goto fail;
    }
	
  CaseLabel(GET_CONSTANT_X0):
    {
      Register Term c;
      Instr_Term(GET_CONSTANT_X0,c);
      if(IsREF(X(0))) {
	Register Reference X0 = Ref(X(0));
	DerefVarEnv(X0);      
	BindVariable(exs,andb,X0,c,goto fail);
	NextOp();
      }
      if(IsIMM(X(0))) {
	if( Eq(X(0), c) ) {
	  NextOp();
	}
      } else if(IsBIG(X(0)) && IsBIG(c)) {
	if(bignum_compare(X(0), c) == 0) {
	  NextOp();
	}
      } else if(IsFLT(X(0)) && IsFLT(c)) {
	if(FltVal(Flt(X(0))) == FltVal(Flt(c))) {
	  NextOp();
	}
      }
      goto fail;
    }

  CaseLabel(GET_ABSTRACTION):
    {
      indx i;
      Term Xi;
      
      Instr_Pred_Index(GET_ABSTRACTION,def,i);
      Xi = Xb(i);
      DerefJumpIfVar(Xi, get_abstraction_var);
      if(IsAbs(Xi) && AbsDef(Abs(Xi)) == def) {
	str = AbsArgRef(Abs(Xi),0);
	NextReadOp();
      }
      goto fail;

  get_abstraction_var:
      {
	Term res;
	abstraction *new;
	Register Reference V = Ref(Xi);
        NewAbstraction(new, def, andb);
        res = TagGen(new);      
	str = AbsArgRef(new,0);
        BindVariable(exs,andb,V, res, goto fail);
	NextWriteOp();
      }		  
    }

  CaseLabel(PUT_X_VOID):
    { 	indx i;
	Instr_Index(PUT_X_VOID,i);
	MakeVariableTerm(Xb(i),andb);
	NextOp();
      }
	
  CaseLabel(PUT_Y_VOID):
    { 	indx n;
	Instr_Index(PUT_Y_VOID,n);
	MakeVariableTerm(Xb(n),andb);
	NextOp();
      }
	
  CaseLabel(PUT_X_VARIABLE):
    {	indx n, i;
	Instr_Index_Index(PUT_X_VARIABLE,n,i); 
	MakeVariableTerm(Xb(n),andb);
	Xb(i) = Xb(n);
	NextOp();
      }

	
  CaseLabel(PUT_Y_VARIABLE):
    {	indx n,i;
	Instr_Index_Index(PUT_Y_VARIABLE,n,i); 
	MakeVariableTerm(Yb(n),andb);
	Xb(i) = Yb(n);
	NextOp();
      }

	
  CaseLabel(PUT_X_VALUE):
    {	indx n,i;
	Instr_Index_Index(PUT_X_VALUE,n,i); 
	Xb(i) = Xb(n);
	NextOp();
      }


  CaseLabel(PUT_Y_VALUE):
    {	indx n,i;
	Instr_Index_Index(PUT_Y_VALUE,n,i); 
	Xb(i) = Yb(n);
	NextOp();
      }

  CaseLabel(PUT2_Y_VALUE):
    {	indx n,i;
	Instr_Index_Index_No_Prefetch(PUT2_Y_VALUE,n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	FetchOpCode(0);
	Xb(i) = Yb(n);
	NextOp();
      }

  CaseLabel(PUT3_Y_VALUE):
    {	indx n,i;
	Instr_Index_Index_No_Prefetch(PUT3_Y_VALUE,n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	FetchOpCode(0);
	Xb(i) = Yb(n);
	NextOp();
      }

  CaseLabel(PUT4_Y_VALUE):
    {	indx n,i;
	Instr_Index_Index_No_Prefetch(PUT4_Y_VALUE,n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	FetchOpCode(0);
	Xb(i) = Yb(n);
	NextOp();
      }

  CaseLabel(PUT5_Y_VALUE):
    {	indx n,i;
        Instr_Index_Index(PUT5_Y_VALUE,n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	Xb(i) = Yb(n);
	Next_Index_Index(n,i); 
	FetchOpCode(0);
	Xb(i) = Yb(n);
	NextOp();
      }

	
  CaseLabel(PUT_CONSTANT):
    {	indx i;
	Term c;
	Instr_Term_Index(PUT_CONSTANT,c,i); 
	Xb(i) = c;
	NextOp();
      }

	
  CaseLabel(PUT_NIL):
    {   indx i;
	Instr_Index(PUT_NIL,i); 
	Xb(i) = NIL;
	NextOp();
      }
	
  CaseLabel(PUT_STRUCTURE):
    {	indx i;
	Functor f;
	Term s;
	Instr_Functor_Index(PUT_STRUCTURE,f,i);
	MakeStructTerm(s, f, andb);
	Xb(i) = s;
	str = StrArgRef(Str(Xb(i)),0);
	NextWriteOp();
      }
	
  CaseLabel(PUT_LIST):
    {	indx i;
	Term l;
	Instr_Index(PUT_LIST,i);
	MakeListTerm(l,andb);
	Xb(i) = l;
	str = LstCarRef(Lst(Xb(i)));
	NextWriteOp();
      }

  CaseLabel(PUT_ABSTRACTION):
    {
      indx i;
      abstraction *new;

      Instr_Pred_Index(PUT_ABSTRACTION,def,i);

      NewAbstraction(new, def, andb);
      Xb(i) = TagGen(new);      
      str = AbsArgRef(new,0);
      NextWriteOp();
    }

  CaseLabel(UNIFY_VOID):
    WriteModeDispatch(unify_void_write);
    Instr_None(UNIFY_VOID);
    str++;
    NextOp();
	
  CaseLabel(UNIFY_Y_VARIABLE):
    WriteModeDispatch(unify_y_variable_write);
    { 	indx n;
	Instr_Index(UNIFY_Y_VARIABLE,n);
	GetArg(Yb(n),str++);
	NextOp();
      }
  CaseLabel(UNIFY_X_VARIABLE):
    WriteModeDispatch(unify_x_variable_write);
    { 	indx n;
	Instr_Index(UNIFY_X_VARIABLE,n);
	GetArg(Xb(n),str++);
	NextOp();
      }
  CaseLabel(UNIFY_X_VALUE):
    WriteModeDispatch(unify_x_value_write);
    {
      indx n;
      Term s;
      Instr_Index(UNIFY_X_VALUE,n);
      GetArg(s,str++);
      {
	Term X1, Y1;
	Deref(X1,s);
	Deref(Y1,Xb(n));
	UnifyDebugPrint("Derefed OK");
	if(!unify(X1,Y1,andb,exs))
	  goto fail;
      }
      NextOp();
    }
	
  CaseLabel(UNIFY_Y_VALUE):
    WriteModeDispatch(unify_y_value_write);
    {
      indx n;
      Term s;
      Instr_Index(UNIFY_Y_VALUE,n);
      GetArg(s,str++);
      {
	Term X1, Y1;
	Deref(X1,s);
	Deref(Y1,Yb(n));
	UnifyDebugPrint("Derefed OK");
	if(!unify(X1,Y1,andb,exs))
	  goto fail;
      }
      NextOp();
    }
  
  CaseLabel(UNIFY_CONSTANT):
    WriteModeDispatch(unify_constant_write);
    {
      Term c;
      Term t;
      Instr_Term(UNIFY_CONSTANT,c);
      GetArg(t,str++);
      DerefJumpIfVar(t, unify_constant_var);
      if(IsIMM(t)) {
	if(Eq(t, c)) {
	  NextOp();
	}
      } else if(IsBIG(t) && IsBIG(c)) {
	if(bignum_compare(t, c) == 0) {
	  NextOp();
	}
      } else if(IsFLT(t) && IsFLT(c)) {
	if(FltVal(Flt(t)) == FltVal(Flt(c))) {
	  NextOp();
	}
      }
      goto fail;
  unify_constant_var:
      {
	Register Reference V = Ref(t);
	BindVariable(exs,andb,V,c, goto fail);
        NextOp();
      }
    }
      
  CaseLabel(UNIFY_NIL):
    WriteModeDispatch(unify_nil_write);
    {
      Term t;
      Instr_None(UNIFY_NIL);
      GetArg(t,str++);
      DerefJumpIfVar(t, unify_nil_var);
      if(IsATM(t)) {
	if(Eq(t, NIL)) {
	  NextOp();
	} else {
	  goto fail;
	}
      }
      goto fail;
  unify_nil_var:
      {
	Register Reference V = Ref(t);
	BindVariable(exs,andb,V,NIL, goto fail);
        NextOp();
      }
    }
    
  CaseLabel(UNIFY_LIST):
    WriteModeDispatch(unify_list_write);
    {
      Term t;
      Term l;
      GetArg(t,str++);
      Instr_None(UNIFY_LIST);
      DerefJumpIfVar(t, unify_list_var);
      if(IsLST(t)) {
	str = LstCarRef(Lst(t));
	NextReadOp();
      }
      goto fail;
  unify_list_var:
      {
	Register Reference V = Ref(t);
	MakeListTerm(l,andb);
        str = LstCarRef(Lst(l));
	BindVariable(exs,andb,V,l, goto fail);
        NextWriteOp();
      }
    }
    
  CaseLabel(UNIFY_STRUCTURE):
    WriteModeDispatch(unify_structure_write);
    {
      Functor f;
      Term s;
      Term t;
      Instr_Functor(UNIFY_STRUCTURE,f);
      GetArg(t,str++);
      DerefJumpIfVar(t, unify_structure_var);
      if(IsSTR(t)) {
	Register Structure S = Str(t);
	if(StrFunctor(S) == f) {
	  str = StrArgRef(S,0);
	  NextReadOp();
	} else {
	  goto fail;
	}
      } 
      goto fail;
  unify_structure_var:
      {
	Register Reference V = Ref(t);
	MakeStructTerm(s, f, andb);
	str = StrArgRef(Str(s),0);
	BindVariable(exs,andb,V,s, goto fail);
	NextWriteOp();
      }
    }

  CaseLabel(SWITCH_ON_CONSTANT):
    {
      Register Term X0;
      Register Term c;
      Register int cond;
      Register int tablesize;
      X0 = X(0);
      Instr_Size_No_Prefetch(SWITCH_ON_CONSTANT,tablesize);

      /* Binary search */
      {
        Register int low = 0;
	Register int high = 2*tablesize;
	Register int pivot;

	for (;;) {
	  pivot = (((low+high) >> 2) << 1);
	  c = (Term)(*(pc+pivot));
	  cond = CompareConstants(X0, c);
	  if (cond == 0) {
	    Branch(pivot+1);
	  } else if (pivot == low) {
	    /* Not found, take the "else" branch */
	    Branch(2*tablesize);
	  } else if (cond < 0) {
	    high = pivot;
	  } else {
	    low = pivot;
	  }
	}
      }
    }

  CaseLabel(SWITCH_ON_STRUCTURE):
    {
      Register Functor f0;
      Register Functor f;
      Register int tablesize;
      f0 = StrFunctor(Str(X(0)));
      Instr_Size_No_Prefetch(SWITCH_ON_STRUCTURE,tablesize);

      /* Binary search */
      {
        Register int low = 0;
	Register int high = 2*tablesize;
	Register int pivot;

	for (;;) {
	  pivot = (((low+high) >> 2) << 1);
	  f = (Functor)(*(pc+pivot));
	  if (f0 == f) {
	    Branch(pivot+1);
	  } else if (pivot == low) {
	    /* Not found, take the "else" branch */
	    Branch(2*tablesize);
	  } else if ((uword)f0 < (uword)f) {
	    high = pivot;
	  } else {
	    low = pivot;
	  }
	}
      }
    }

#ifdef BAM
/***************************************************************************/

#ifdef __GNUC__
#define NYICaseLabel(Op) \
	CaseLabel(Op): \
{\
	char *buf; \
	sprintf(buf, "NYI: BAM instruction %s", #Op);\
	FatalError(buf);\
}
#else
#define NYICaseLabel(Op) \
	CaseLabel(Op): \
{\
	char *buf; \
	sprintf(buf, "NYI: BAM instruction: %d", Op);\
	FatalError(buf);\
}
#endif

#define IntGreater(x,y)	CompareIntegers(>,x,y)

#define Instr_Dlabel(Op) \
	StartInstruction(Op); \
	EndInstruction(2);

#define Instr_Index_Dlabel(Op,i) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	EndInstruction(3);

#define Instr_Index_Index_Dlabel_Dlabel(Op,i,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
	EndInstruction(4);

#define Instr_Index_Term_Dlabel_Dlabel(Op,i,x) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	TermWord(2,x); \
	EndInstruction(5);

#define LastLabelOffset (-1)
#define NextToLastLabelOffset (-2)

/* START OF BAM ENGINE */

  CaseLabel(FAIL_DEC):
    fail_dec:
    {
      if(EmptyTryStack){	
	DumpConsStack;		
	goto fail;
      }
      PopTryStack(pc);
      FetchOpCode(0);	/* prefetch opcode */
      NextOp();		
    }

  CaseLabel(LOAD_NIL):
    {
      Register indx i;
      Instr_Index(LOAD_NIL,i);
      Xb(i) = NIL;
      NextOp();
    }

  CaseLabel(LOAD_INTEGER):
    /* fall through */

  CaseLabel(LOAD_ATOM):
    {
      Register indx i;
      Register Term c;
      Instr_Index_Term(LOAD_ATOM,i,c);
      Xb(i) = c;
      NextOp();
    }

  CaseLabel(READ_CAR):
    {
      Register indx i, j;
      Instr_Index_Index(READ_CAR,i,j);
      GetLstCar(Xb(j), Lst(Xb(i)));
      NextOp();
    }

  CaseLabel(READ_CDR):
    {
      Register indx i, j;
      Instr_Index_Index(READ_CDR,i,j);
      GetLstCdr(Xb(j), Lst(Xb(i)));
      NextOp();
    }

  NYICaseLabel(READ_ARG);

  CaseLabel(SET_REG_REG):
    {
      Register indx i, j;
      Instr_Index_Index(SET_REG_REG,i,j);
      Xb(j) = Xb(i);
      NextOp();
    }

  CaseLabel(SET_REG_YVAR):
    {
      Register indx i, j;
      Instr_Index_Index(SET_REG_YVAR,i,j);
      Xb(j) = Yb(i);
      NextOp();
    }

  CaseLabel(SET_YVAR_REG):
    {
      Register indx i, j;
      Instr_Index_Index(SET_YVAR_REG,i,j);
      Yb(j) = Xb(i);
      NextOp();
    }

  NYICaseLabel(TERM_SWITCH);
  NYICaseLabel(ATOM_SWITCH);
  NYICaseLabel(ATOM_WITH_ELSE_SWITCH);
  NYICaseLabel(ATOM_WITH_ATOM_ELSE_SWITCH);
  NYICaseLabel(ATOM_TYPE);
  NYICaseLabel(ATOM_TYPE_WITH_ELSE);
  NYICaseLabel(KNOWN_ATOM_SWITCH);
  NYICaseLabel(KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH);
  NYICaseLabel(INTEGER_SWITCH);
  NYICaseLabel(INTEGER_WITH_ELSE_SWITCH);
  NYICaseLabel(INTEGER_WITH_INTEGER_ELSE_SWITCH);

  CaseLabel(INTEGER_TYPE):
    {
      Register indx i;
      Instr_Index_Dlabel(INTEGER_TYPE,i);
      DerefJumpIfVar(Xb(i), integer_type_varcase);
      if (IsINT(Xb(i))) {
	NextOp();
      } else {
	goto fail_dec;
      }      
    integer_type_varcase:
      Branch(LastLabelOffset);
    }

  NYICaseLabel(INTEGER_TYPE_WITH_ELSE);
  NYICaseLabel(KNOWN_INTEGER_SWITCH);
  NYICaseLabel(KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH);

  CaseLabel(LIST_SWITCH):
    { 
      Register indx i;
      StartInstruction(LIST_SWITCH);
      IndexWord(1,i);
      FetchOpCode(LsInstructionSize);	/* Speculative prefetch */

      if(heapcurrent >= heaplimit) {
	SaveHeapRegisters();
	reinit_heap(heapcurrent,SMALL_SIZE);	/* ???????????? */
	RestoreHeapRegisters();
	heaplimit = heapend-1024;
      }
      DerefJumpIfVar(Xb(i), list_switch_varcase);
      if (IsLST(Xb(i))) {
	Skip(LsInstructionSize);
      } else if (Eq(Xb(i), NIL)) {
	Branch(LsNilOffset);
      } else {
	goto fail_dec;
      }
   list_switch_varcase:
      Branch(LsVarOffset);
    }

  NYICaseLabel(LIST_TYPE_WITH_ELSE);
  NYICaseLabel(STRUCT_TYPE);
  NYICaseLabel(STRUCT_TYPE_WITH_ELSE);
  NYICaseLabel(KNOWN_FUNCTOR_SWITCH);
  NYICaseLabel(KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH);

  CaseLabel(INT_COMPARE_REG_REG):
    {
      Register indx i, j;
      Instr_Index_Index_Dlabel_Dlabel(INT_COMPARE_REG_REG,i,j);
      if (IntGreater(Xb(i), Xb(j))) {
	NextOp();
      } else if (Eq(Xb(i), Xb(j))) {
	Branch(LastLabelOffset);
      } else {
	Branch(NextToLastLabelOffset);
      }
    }

  CaseLabel(INT_COMPARE_REG_INT):
    {
      Register indx i;
      Register Term c;
      Instr_Index_Term_Dlabel_Dlabel(INT_COMPARE_REG_INT,i,c);
      if (IntGreater(Xb(i), c)) {
	NextOp();
      } else if (Eq(Xb(i), c)) {
	Branch(LastLabelOffset);
      } else {
	Branch(NextToLastLabelOffset);
      }
    }

  CaseLabel(EQ_REG_REG):
    {
      Term t1,t2;
      indx reg1,reg2;
      code *faillabel,*dklabel;

      Instr_Index_Index_No_Prefetch(EQ_REG_REG,reg1,reg2); /* Attention */
      
      FetchOpCode(EqRegRegSkipOffset);	/* Speculative prefetch */

      Deref(t1, Xb(reg1));
      Deref(t2, Xb(reg2));
      if(Eq(t1,t2)) {
	Skip(EqRegRegSkipOffset);
      }
      if(IsLST(t1)) {
	if(IsLST(t2)) { 
	  FatalError("not implemented eq_reg_reg");
	}
	if(IsVar(t2))      {
	  PushConsStack(MakeCons_Reg_NonVar(reg2),reg1);
	  goto eq_suspend;
	}
	goto eq_fail;
      }
      if(IsSTR(t1)) {
	if(IsSTR(t2)) {
	  FatalError("not implemented eq_reg_reg");	
	}
	if(IsVar(t2))      {
	  PushConsStack(MakeCons_Reg_NonVar(reg2),reg1);
	  goto eq_suspend;
	}
	goto eq_fail;
      }
      if(IsVar(t1)) {
	if(IsVar(t2)) {
	  PushConsStack(MakeCons_Reg_Var(reg2),reg1);
	  goto eq_suspend;
	}
	PushConsStack(MakeCons_Reg_NonVar(reg1),reg2);
	goto eq_suspend;
      }
      if(IsGEN(t1)) {
	if(IsGEN(t2)) {
	  FatalError("not implemented eq_reg_reg");	
	}
	if(IsVar(t2))      {
	  PushConsStack(MakeCons_Reg_NonVar(reg2),reg1);
	  goto eq_suspend;
	}
	goto eq_fail;
      }
    eq_fail:
      Branch(EqRegRegFailOffset);

    eq_suspend:
      Branch(EqRegRegSuspendOffset);
    }

  CaseLabel(MATCH_REG_REG):
    {
      Register indx i, j;
      Register Term Xi, Xj;
      Instr_Index_Index(MATCH_REG_REG,i,j);
      Deref(Xi, Xb(i));
      Deref(Xj, Xb(j));
      if (!unify(Xi, Xj, andb, exs))
	goto fail;
      NextOp();
    }

  NYICaseLabel(MATCH_REG_REG_OFF);
  NYICaseLabel(MATCH_REG_ATOM);
  NYICaseLabel(MATCH_REG_OFF_ATOM);
  NYICaseLabel(MATCH_REG_INTEGER);
  NYICaseLabel(MATCH_REG_OFF_INTEGER);

  CaseLabel(MATCH_REG_NIL):
    {
      Register indx i;
      Register Term Xi;
      Instr_Index(MATCH_REG_NIL,i);
      Xi = Xb(i);
      RevInternalDeref(Xi, goto match_reg_nil_nonvarcase);
      {
	Register Reference V = Ref(Xi);
	BindVariable(exs,andb,V,NIL, goto fail);
	NextOp();
      }
  match_reg_nil_nonvarcase:
      if(Eq(Xi,NIL)) {
	NextOp();
      } else {
        goto fail;
      }
    }

  NYICaseLabel(MATCH_REG_OFF_NIL);
  NYICaseLabel(MATCH_REG_YVAR);
  NYICaseLabel(MATCH_YVAR_YVAR);

  CaseLabel(MATCH_YVAR_REG_OFF):
    {
      Register indx i,j,offset;
      Register Term Yi, Xj, Xo;
      Instr_Index_Index_Offset(MATCH_YVAR_REG_OFF,i,j,offset);
      Yi = Yb(i);
      Xj = Xb(j);
      Xo = Ref(Xj)[offset];
      RevInternalDeref(Yi, goto match_yvar_reg_off_nonvarcase);
      {
	Register Reference V = Ref(Yi);
	BindVariable(exs,andb,V,Xo, goto fail);
	NextOp();
      }
  match_yvar_reg_off_nonvarcase:
      if(unify(Yi, Xo, andb, exs)) {
	NextOp();
      } else {
        goto fail;
      }
    }
      
  CaseLabel(MATCH_REG_H_LIST):
    {
      Register indx i, j;
      Register Term Xi;
      Register Term l;
      Instr_Index_Offset(MATCH_REG_H_LIST,i,j);
      Xi = Xb(i);
      l = TagLst((List)(heapcurrent-j));
      RevInternalDeref(Xi, goto match_reg_h_list_nonvarcase);
      {
	Register Reference V = Ref(Xi);
	BindVariable(exs,andb,V,l, goto fail);
	NextOp();
      }
  match_reg_h_list_nonvarcase:
      if(IsLST(Xi) && unify(Xi, l, andb, exs)) {
	NextOp();
      } else {
        goto fail;
      }
    }

  NYICaseLabel(MATCH_YVAR_H_LIST);
  NYICaseLabel(MATCH_REG_H_STRUCT);
  NYICaseLabel(MATCH_YVAR_H_STRUCT);
  NYICaseLabel(MATCH_REG_STRUCT);
  NYICaseLabel(MATCH_REG_STRUCT_LEVEL);
  NYICaseLabel(MATCH_REG_OFF_STRUCT);
  NYICaseLabel(MATCH_REG_OFF_STRUCT_LEVEL);
  NYICaseLabel(MATCH_YVAR_STRUCT);
  NYICaseLabel(MATCH_YVAR_STRUCT_LEVEL);
  NYICaseLabel(MATCH_REG_LIST_CAR);
  NYICaseLabel(MATCH_REG_LIST_CAR_LEVEL);
  NYICaseLabel(MATCH_REG_OFF_LIST_CAR);
  NYICaseLabel(MATCH_REG_OFF_LIST_CAR_LEVEL);
  NYICaseLabel(MATCH_YVAR_LIST_CAR);
  NYICaseLabel(MATCH_YVAR_LIST_CAR_LEVEL);

  CaseLabel(MATCH_REG_LIST_CDR):
    {
      Register indx i, j;
      code *label;

      Instr_Index_Label_Index(MATCH_REG_LIST_CDR,i,label,j);
      RevInternalDeref(Xb(i), goto match_reg_list_cdr_nonvarcase);
      {
	Register Reference V = Ref(Xb(i));
	pc = label;
	FetchOpCode(0);
	BindVariable(exs,andb,V,TagLst((List)heapcurrent), goto fail);
	NextOp();
      }
      
  match_reg_list_cdr_nonvarcase:
      if (!IsLST(Xb(i)))
	goto fail_dec;
      else {
	GetLstCdr(Xb(j), Lst(Xb(i)));
	NextOp();
      }
    }

  NYICaseLabel(MATCH_REG_LIST_CDR_LEVEL);
  NYICaseLabel(MATCH_REG_OFF_LIST_CDR);
  NYICaseLabel(MATCH_REG_OFF_LIST_CDR_LEVEL);

  CaseLabel(MATCH_YVAR_LIST_CDR):
    {
      Register indx i, j;
      code *label;

      Instr_Index_Label_Index(MATCH_YVAR_LIST_CDR,i,label,j);
      RevInternalDeref(Yb(i), goto match_yvar_list_cdr_nonvarcase);
      {
	Register Reference V = Ref(Yb(i));
	pc = label;
	FetchOpCode(0);
	BindVariable(exs,andb,V,TagLst((List)heapcurrent), goto fail);
	NextOp();
      }
      
  match_yvar_list_cdr_nonvarcase:
      if (!IsLST(Yb(i)))
	goto fail_dec;
      else {
	GetLstCdr(Xb(j), Lst(Yb(i)));
	NextOp();
      }
    }

  NYICaseLabel(MATCH_YVAR_LIST_CDR_LEVEL);

  CaseLabel(PUSH_LIST_IMM):
    Instr_None(PUSH_LIST_IMM);
    ListPushb(sizeof(Term));
    NextOp();

  NYICaseLabel(PUSH_LIST);
  NYICaseLabel(PUSH_STRUCTURE_IMM);
  NYICaseLabel(PUSH_STRUCTURE);

  CaseLabel(PUSH_NIL):
    Instr_None(PUSH_NIL);
    ValuePush(NIL);
    NextOp();

  CaseLabel(PUSH_REG):
    {
      Register indx i;
      Instr_Index(PUSH_REG,i);
      ValuePush(Xb(i));
      NextOp();
    }

  CaseLabel(PUSH_YVAR):
    {
      Register indx i;
      Instr_Index(PUSH_YVAR,i);
      ValuePush(Yb(i));
      NextOp();
    }

  NYICaseLabel(PUSH_VOID);

  CaseLabel(SET_REG_H_AND_PUSH_VOID):
    {
      Register indx i;
      Instr_Index(SET_REG_H_AND_PUSH_VOID,i);
      Xb(i) = TagRef((Reference)heapcurrent);
      VoidPush();
      NextOp();
    }

  CaseLabel(SET_YVAR_H_AND_PUSH_VOID):
    {
      Register indx i;
      Instr_Index(SET_YVAR_H_AND_PUSH_VOID,i);
      Yb(i) = TagRef((Reference)heapcurrent);
      VoidPush();
      NextOp();
    }

  CaseLabel(SET_REG_AND_YVAR_H_AND_PUSH_VOID):
    {
      Register indx i, j;
      Register Term V;
      Instr_Index_Index(SET_REG_AND_YVAR_H_AND_PUSH_VOID,i,j);
      V = TagRef((Reference)heapcurrent);
      Xb(i) = V;
      Yb(j) = V;
      VoidPush();
      NextOp();
    }

  NYICaseLabel(PUSH_ATOM);
  NYICaseLabel(PUSH_INTEGER);
  NYICaseLabel(PUSH_FUNCTOR);

  CaseLabel(SET_REG_H):
    {
      Register indx i;
      Instr_Index(SET_REG_H,i);
      Xb(i) = TagRef((Reference)heapcurrent);
      NextOp();
    }

  NYICaseLabel(SET_YVAR_H);

  CaseLabel(BTRY):
    {
      Register indx i;		
      Register pseudo_term pt;	
      code *label;

      label = pc + BTryInstructionSize;

      PushTryStack(label,cons_top);   /* MACRO InstrOffSet */
      Instr_Label_Index_Pseudo_No_Prefetch(BTRY,label,i,pt);
      pc = label;
      FetchOpCode(0);		/* prefetch opcode */
      PushConsStack(MakeCons_Reg_Ps(i),pt); 
      NextOp();		
    }

  CaseLabel(BTRUST):
    {
      Register indx i;		
      Register pseudo_term pt;	
      code *label;

      Instr_Label_Index_Pseudo_No_Prefetch(BTRUST,label,i,pt);
      pc = label;
      FetchOpCode(0);		/* prefetch opcode */
      PushConsStack(MakeCons_Reg_Ps(i),pt); 
      NextOp();		
    }

  NYICaseLabel(TEST_TRY);
  NYICaseLabel(TEST_TRUST);
  NYICaseLabel(S_TRY);
  NYICaseLabel(S_TRUST);
  NYICaseLabel(BACK);
  NYICaseLabel(GEN_TRY);
  NYICaseLabel(GEN_TRUST);

  CaseLabel(BEXECUTE):

    Instr_Pred(BEXECUTE,def);
#ifdef TRACE
    andb->cont->label=proceed;
#else
    andb->cont = andb->cont->next;
    yreg = andb->cont->yreg;
#endif
    goto bexecute_definition;

  CaseLabel(BEXECUTE_SIMPLE):

    Instr_Pred(BEXECUTE_SIMPLE,def);
#ifdef TRACE
    andb->cont->label=proceed;
#endif

   bexecute_definition:

    arity = def->arity;	

    switch (def->enter_instruction) {
    case ENTER_EMULATED:
      CheckExceptions;
      TRACE_Call(def,areg,andb,insert);
      pc = def->code.incoreinfo;
      break;
    case ENTER_C:
      TRACE_Call_C(def,areg,andb,insert);
      SaveRegisters;
      switch((def->code.cinfo)(exs)) {
      case FALSE:
	RestoreRegisters;		
	TRACE_Fail_Call_C(def,areg,andb,insert);
	goto fail;
      case TRUE:
	RestoreRegisters;		
	TRACE_Exit_Call_C(def,areg,andb,insert);
	pc = andb->cont->label;
	break;
      case SUSPEND:
	{
	  choicebox *new;
	  RestoreRegisters;
          MakeChoicebox(new,def,andb);
          MakeChoiceCinfoCont(new, (def->code.cinfo),areg,arity);
          InsertChoicebox(new,insert,andb);
	  insert = new;
	  SuspendBuiltin(exs,new);
	  TRACE_Suspend_Goal_C(new);
	}
	pc = andb->cont->label;
	break;
        case REDO;
	 {
	   choicebox *new;
	  
	   RestoreRegisters;
	   MakeChoicebox(new, send3_def, andb);
	   MakeChoiceCinfoCont(new, akl_send_3, areg, 3);
	   InsertChoicebox(new, insert, andb);
	   insert = new;
	   RedoBuiltin(exs,new);
	   TRACE_Suspend_Goal_C(new);
	 }
	  break;

      }
      break;
    case ENTER_UNDEFINED:
      {
	predicate *truedef = (predicate *) Tad(def);
	ENGINE_ERROR_2(AtmPname(truedef->name), truedef->arity, "undefined agent")
      }

#ifdef TRACE
      tracing = TRUE;
#endif
      TRACE_Fail_Call_C(def,areg,andb,insert);
      goto fail;
    }
    FetchOpCode(0);
    NextOp();
	
  CaseLabel(BCALL):
    /* We are now supposed to have all arguments in registers
     * and are ready to call up a new predicate. We must however
     * save a pointer to where we leave this predicate, a
     * continuation.
     */
	
    /* Get arguments to CALL */
    {
      Instr_Pred_Bitmask(BCALL,def);

      /* Store the arity, needed when the choicebox is created */
      arity = def->arity;

      /* save the program counter in the continuation */
      andb->cont->label = pc;

      /* call definition */

      switch (def->enter_instruction) {
      case ENTER_EMULATED:
	CheckExceptions;
	TRACE_Call(def,areg,andb,insert);
	pc = def->code.incoreinfo;
	break;
      case ENTER_C:
	TRACE_Call_C(def,areg,andb,insert);
	SaveRegisters;
	switch((def->code.cinfo)(exs)) {
	case FALSE:
	  RestoreRegisters;
	  TRACE_Fail_Call_C(def,areg,andb,insert);
	  goto fail;
	case TRUE:
	  RestoreRegisters;	
	  TRACE_Exit_Call_C(def,areg,andb,insert);
	  break;
	case SUSPEND:
	  {
	    choicebox *new;
	    RestoreRegisters;
	    MakeChoicebox(new,def,andb);
	    MakeChoiceCinfoCont(new, (def->code.cinfo),areg,arity);
	    InsertChoicebox(new,insert,andb);
	    insert = new;
	    SuspendBuiltin(exs,new);
	    TRACE_Suspend_Goal_C(new);
	  }
	  break;
        case REDO;
	 {
	   choicebox *new;
	  
	   RestoreRegisters;
	   MakeChoicebox(new, send3_def, andb);
	   MakeChoiceCinfoCont(new, akl_send_3, areg, 3);
	   InsertChoicebox(new, insert, andb);
	   insert = new;
	   RedoBuiltin(exs,new);
	   TRACE_Suspend_Goal_C(new);
	 }
	  break;
	}
	break;
      case ENTER_UNDEFINED:
	{
	  predicate *truedef = (predicate *) Tad(def);
	  ENGINE_ERROR_2(AtmPname(truedef->name), truedef->arity, "undefined agent")
	}

#ifdef TRACE
	tracing = CREEP;
#endif
	TRACE_Fail_Call_C(def,areg,andb,insert);
	goto fail;
      }
      FetchOpCode(0);
      NextOp();
    }
	
  CaseLabel(JUMP_GC_AND_DUMP_CONT):
#ifdef TRACE
    andb->cont->label=proceed;
#else
    andb->cont = andb->cont->next;
    yreg = andb->cont->yreg;
#endif
    /* fall through */

  CaseLabel(JUMP_GC):
    CheckExceptions;
    /* fall through */

  CaseLabel(JUMP):
    Branch(JumpLabelOffset);

  CaseLabel(BPROCEED_SIMPLE):
    {
      TRACE_Exit_Cont(andb,insert);

      pc = andb->cont->label;
      FetchOpCode(0);
      NextOp();
    }

  NYICaseLabel(BPROCEED);

  CaseLabel(CREATE_CONT):
    {
      indx ysize;
      Instr_Size(CREATE_CONT,ysize);
      /* Hmm, do we really need to init the label field to NULL? */
      MakeAndCont(andb,NULL,andb->cont,ysize);
      yreg = andb->cont->yreg;
	
      NextOp();
    }

  NYICaseLabel(TEST_LEVEL);
  NYICaseLabel(CONSTRAINT_STACK_TEST);

  CaseLabel(JUMP_ON_NONEMPTY_CONSTRAINT_STACK):
    {
      Instr_Dlabel(JUMP_ON_NONEMPTY_CONSTRAINT_STACK);
      if (!EmptyConsStack) {
	Branch(LastLabelOffset);
      }
      NextOp();
    }

  CaseLabel(ALIAS_CHECK):
    {
      Instr_None(ALIAS_CHECK);
      if(ZeroOrOneEntryConsStack) {
	NextOp();		/* Two entry case should be made inline */
      }

      alias_flag=alias_check_stack(exs,cons_top,cons_bottom,areg);
      if(alias_flag== ALIAS_CHECK_FAIL) {
	alias_flag=ALIAS_DEFAULT;
	goto fail_dec;
      }
      NextOp();
    }

  CaseLabel(ALIAS_CHECK_REG):
    {
      Register indx i;
      Instr_Index(ALIAS_CHECK_REG,i);
      if(EmptyConsStack) {
	NextOp();
      }
      FatalError("not yet implemented");
    }

  NYICaseLabel(FAIL_ON_ALIAS);
  NYICaseLabel(CHOICE_QUIET_CUT);

  CaseLabel(SUSPEND_LONE_VAR):
    {
      Register indx i;
      int arity;
      choicebox *new;
      code *label;

      Instr_Index_Label_Size_No_Prefetch(SUSPEND_LONE_VAR,i,label,arity);
      BamMakeChoicebox(new,def,andb,UNCOND_SUSP_CHOICE);
      BamMakeChoiceCont(new,label,areg,arity);
      InsertChoicebox(new,insert,andb);
      MakeRegSuspend(i,new,andb);
      pc=andb->cont->label;
      FetchOpCode(0);
      NextOp();
    }

  CaseLabel(SUSPEND_WITHOUT_REG):
    {
      Register indx i;
      int arity;
      choicebox *new;
      code *label;
      
      Instr_Label_Size_No_Prefetch(SUSPEND_WITHOUT_REG,label,arity);
      BamMakeChoicebox(new,def,andb,UNCOND_SUSP_CHOICE);
      BamMakeChoiceCont(new,label,areg,arity);
      InsertChoicebox(new,insert,andb);
      goto bam_suspend_handler;
    }
      
  CaseLabel(BSUSPEND):
    {
      Register indx i;
      int arity;
      choicebox *new;
      code *label;

      Instr_Index_Label_Size_No_Prefetch(BSUSPEND,i,label,arity);
      BamMakeChoicebox(new,def,andb,UNCOND_SUSP_CHOICE);
      BamMakeChoiceCont(new,label,areg,arity);
      InsertChoicebox(new,insert,andb);
      MakeRegSuspend(i,new,andb);
      
    bam_suspend_handler:
      {
      if(alias_flag==ALIAS_DEFAULT) { /* no trail entries */
	Register cons_stack_cell *current=cons_top;

	do {
	  current= current--;
	  switch(Cons_Tag(current->cons1)) {
	  case CONS_TAG_REG_PS:
	    i=ConsGetReg(current->cons1);
	    MakeRegSuspend(i,new,andb);
	    break;
	  case CONS_TAG_REG_NONVAR:
	    i=ConsGetReg(current->cons1);
	    MakeRegSuspend(i,new,andb);
	    break;
	  case CONS_TAG_REG_VAR:
	    i=ConsGetReg(current->cons1);
	    MakeRegSuspend(i,new,andb);
	    MakeRegSuspend(current->cons2.reg,new,andb);
	    break;
	  case CONS_TAG_TEST:
	    break;
	  }} while(current!=cons_bottom);
	pc=andb->cont->label;
	FetchOpCode(0);
	NextOp();
      }
      else			/*  first: alias check done - all suspension vars on trail */
	                        /*  second: dynamic - some suspension vars on trail , some on stack*/
	{
	  Register trailentry *tr=exs->trail.current;
	  Register trailentry *stop=exs->trail.current - alias_flag;

	  exs->trail.current=stop;
	  alias_flag=ALIAS_DEFAULT;
	  for(;tr != stop; tr--) {
	    Reference r = Ref(tr->var);
	    if(NotDummyLocal(r))
	      MakeChoiceWake(r,chb,andb);
	    }
	  if(EmptyConsStack) {
	    pc=andb->cont->label;
	    FetchOpCode(0);
	    NextOp();
	  }
	  FatalError("at this time should not arrive here");
	  goto bam_suspend_handler;
	}
      }
    }

  NYICaseLabel(CREATE_GUARD);

  CaseLabel(FLAT_QUIET_CUT):
    {
      int arity;
      choicebox *new;
      code *label;

      if(EmptyConsStack) {
	DumpTryStack;
	FetchOpCode(FlatQuietCutInstructionSize);
	Skip(FlatQuietCutInstructionSize);
      }
      Instr_Label_Size_No_Prefetch(FLAT_QUIET_CUT,label,arity);
      pc = label;
      FetchOpCode(0);

      BamMakeChoicebox(new,def,andb,UNCOND_SUSP_CHOICE);
      BamMakeChoiceCont(new,pc,areg,arity);
      InsertChoicebox(new,insert,andb);
      goto bam_suspend_handler;
    }

  NYICaseLabel(FLAT_QUIET_COMMIT);
  NYICaseLabel(FLAT_NOISY_WAIT);
  NYICaseLabel(SHORTCUT_QUIET_CUT);
  NYICaseLabel(SHORTCUT_QUIET_COMMIT);
  NYICaseLabel(SHORTCUT_NOISY_WAIT);
  NYICaseLabel(DEEP_QUIET_CUT);
  NYICaseLabel(DEEP_QUIET_COMMIT);
  NYICaseLabel(DEEP_NOISY_WAIT);

/* END OF BAM ENGINE */
/***************************************************************************/
#endif


  CaseLabel(SEND3):
    {
      Term inport, message, outport;
      Reference ref;
      bool res;
      bool (*sfunc)();
      
      Instr_None(SEND3);

      TRACE_Call_C(send3_def,areg,andb,insert);

      SaveRegisters;

      Deref(inport, X(1));
      if (!IsCvaTerm(inport))
	goto sendothercase;

      ref = Ref(inport);
      DerefGvaEnv(RefGva(ref));
      sfunc = RefCvaMethod(ref)->send;

      if(IsLocalGVA(ref, andb) && (sfunc != NULL)) {

	Deref(message, X(0));

	res = sfunc(message, inport, exs);

	if(res == TRUE) {
	  Deref(outport, X(2));
	  UNIFY(outport, inport, res);
	}
	goto sendswitch;
      }
      
    sendothercase:
      res = akl_send_3(exs);

    sendswitch:
      switch(res) {
      case FALSE:
	RestoreRegisters;
	TRACE_Fail_Call_C(send3_def,areg,andb,insert);
	goto fail;
      case TRUE:
	RestoreRegisters;	
	TRACE_Exit_Call_C(send3_def,areg,andb,insert);
	break;
      case SUSPEND:
	{
	  choicebox *new;
	  
	  RestoreRegisters;
	  MakeChoicebox(new, send3_def, andb);
	  MakeChoiceCinfoCont(new, akl_send_3, areg, 3);
	  InsertChoicebox(new, insert, andb);
	  insert = new;
	  SuspendBuiltin(exs,new);
  	  TRACE_Suspend_Goal_C(new);
	}
	break;
      case REDO:
	{
	  choicebox *new;
	  
	  RestoreRegisters;
	  MakeChoicebox(new, send3_def, andb);
	  MakeChoiceCinfoCont(new, akl_send_3, areg, 3);
	  InsertChoicebox(new, insert, andb);
	  insert = new;
	  RedoBuiltin(exs,new);
  	  TRACE_Suspend_Goal_C(new);
	}
	break;
      }
      
      NextOp();
    }

  CaseLabel(SUSPEND_FLAT):
    {
      choicebox *new;
      code *label;
      indx i;
      int arity;
      Reference r;

      Instr_Index_Label_Size_No_Prefetch(SUSPEND_FIRST,i,label,arity);
      MakeChoicebox(new, def, andb);
      new->type = ZIPPEDIDOO;
      MakeChoiceCont(new, label, areg, arity);
      InsertChoicebox(new, insert, andb);
      insert = new;
      r = Ref(Xb(i));
      if (VarIsUVA(r)) {
	MakeBigVar(r);
      }
      MakeRecall(r, new);	/* Not really, this is a flat MakeWake... */

      /* Do a PROCEED without the implicit deallocate */
      pc = andb->cont->label;
      FetchOpCode(0);
      yreg = andb->cont->yreg;

      arity = 0;
      CheckExceptions;

      NextOp();
    }

#ifndef THREADED_CODE
  default:
    {
      FatalError("Instruction does not exist");
    }
#endif
  }
 FatalError("We should never land here!");

/*********************
* write instructions
**********************/

 WriteModeLabel(unify_void_write)
  Instr_None(UNIFY_VOID);
  InitVariable(str,andb);
  str++;
  NextOp();

 WriteModeLabel(unify_y_variable_write)
  {	indx n;
	Instr_Index(UNIFY_Y_VARIABLE,n);
        InitVariable(str,andb);
	Yb(n) = TagRef(str);
	str++;
	NextOp();
      }

 WriteModeLabel(unify_x_variable_write)
  {	indx n;
	Instr_Index(UNIFY_X_VARIABLE,n);
        InitVariable(str,andb);
	Xb(n) = TagRef(str);
	str++;
	NextOp();
      }

 WriteModeLabel(unify_x_value_write)
  { 	indx n;
	Instr_Index(UNIFY_X_VALUE,n);
	*(str++) = Xb(n);
	NextOp();
      }

 WriteModeLabel(unify_y_value_write)
  { 	indx n;
	Instr_Index(UNIFY_Y_VALUE,n);
	*(str++) = Yb(n);
	NextOp();
      }

 WriteModeLabel(unify_constant_write)
  { 	Term  c;
	Instr_Term(UNIFY_CONSTANT,c);
	*(str++) = c;
	NextOp();
      }

 WriteModeLabel(unify_nil_write)
  {	Instr_None(UNIFY_NIL);
  	*(str++) = NIL;
	NextOp();
      }

 WriteModeLabel(unify_list_write)
  { 	Term l;
	Instr_None(UNIFY_LIST);
	MakeListTerm(l,andb);
	*str = l;
	str = LstCarRef(Lst(l));
	NextOp();
      }

 WriteModeLabel(unify_structure_write)
  { 	Functor f;
	Term s;
	Instr_Functor(UNIFY_STRUCTURE,f);	
	MakeStructTerm(s, f, andb);
	*str = s;
	str = StrArgRef(Str(s),0);
	NextOp();
      }



  /********************************************************************
   *
   * PROMOTION
   *
   * We assume (at least) the following:
   *
   * pc		refers to the instruction following the guard operator.
   * andb	refers to the and-box to be promoted.
   * chb	is the parent of andb.
   *
   * The and-box andb contains exactly one continuation,
   * the so called guard continuation, which contains the
   * code and the environment of the body.
   *
   * The and-box is solved, which means that it contains no
   * choice-boxes, but may contain constraints.
   * 
   * The trail may contain conditionally bound variables that
   * correspond to constraints.
   *
   * The task-stack may contain tasks that are obsolete(?).
   *
   */
  

 promotion:

  {
    andbox	*promoted;

    /* The father and box is the father of the current choice box */

    PromoteDebugPrint("promote");

    /* The father and box is the father of the current choice box */
    promoted = andb;
    andb = chb->father;

    /* We redirect the env-pointer to the fathers env-pointer. */

    PromoteAndbox(promoted,andb);
    
    Kill(promoted);

    /* Then we remove the current choice box from the tried chain in
     * the and box we promote to.
     */

    RemoveChoicebox(chb,insert,andb);

    /* The context is adjusted for the father and-box. */

    PromoteContext(exs,andb);
    
    /* The current choice box is set to the father choice box. */
    chb = andb->father;

    /* The constraints are moved to the parent and-box */

    PromoteConstraints(exs, promoted, andb);
    
    /* The current and-continuation is added to the list of continuations */
    promoted->cont->next = andb->cont;
    andb->cont = promoted->cont;

    /* pc is pointing to the next instruction in the promoted continuation */
    NextOp();
  }


  
  /****************************************************************************
   *
   *	This is the main handler. It is invoked only by the
   *	guard-halt instruction. If there are no more tasks to deal with
   *	the user should be notified about the result of the computation
   *
   */
  
 mainhandler:

  
  while(!EmptyWake(exs)){
      
    NextWake(exs,wand);

    /* if the and-box is dead it is ignored */
    if(Dead(wand))
      continue;
    if(wand->father->father == andb)
      goto wakehandler; 
    if(isdead(wand, andb))
      continue;
    goto wakehandler;
  }


  while(!EmptyRecall(exs)){
      
    NextRecall(exs,rchb);

#ifdef BAM
    if(rchb->type != NON_BAM_CHOICE) {
      if(rchb->type == DEAD_CHOICE)
	continue;
      
      /* Save a continuation to where we currently are */
      andb->cont->label = pc;
      
      if (rchb->father != andb) {
	/* if we fail on the way down to to
	 * fandb will be set to the failing and-box
	 */
	{
	  andbox *fandb;
	  fandb = installb(andb,wand,exs);
	  
	  if(fandb) {
	    andb = fandb;
	    chb = andb->father;
	    TRACE_Down_Guard(andb,fandb->father->father);
	    TRACE_Reenter_Goal(fandb->father);
	    goto fail;
	  }
	}
	
	andb = wand;
	exs->andb = andb;
	yreg = andb->cont->yreg;
      }
      
      chb = rchb;
      /* ??? TRACE_Reenter_Goal_C(chb); */
      
      exs->insert = chb;  
      
      { int i;
	
	for(i = 0; i < chb->cont->arity; i++)
	  X(i) = chb->cont->arg[i];
      }
      
      pc = chb->cont->instr.label;
      FetchOpCode(0);
      UnlinkChb(chb);
      chb->cont = NULL;		/* !!! Not needed when GC knows about this */
      chb->type = DEAD_CHOICE;
      NextOp();
    }
#endif
    /* if the choice-box is tried it is ignored */
    if(rchb->cont == NULL)
      continue;
    
    wand = rchb->father;
    
    /* if the and-box is dead it is ignored */
    if(Dead(wand))
      continue;
    /* if the choice-box is handled it is ignored */
    if(rchb->cont == NULL)	/* handled */
      continue;
    if(isdead(wand, andb))
      continue;
    goto recallhandler;
  }

  if(Solved(andb)) {
    /* if it is solved we return */
#ifdef TRACE
    tracing=NOTRACE;
#endif   
    ScheduleDebugPrint("Computation suceeded");
    return;
  }

  cand = candidate(andb->tried);
    
  if(cand != NULL) {
    NondetDebugPrint("Candidate in main and-box");  
    goto splithandler;
  }

  /* the computationis about to suspend, try an extra gc to
   * find closing ports.
   */
  ScheduleDebugPrint("Trying a last GC befor suspension");
  
  exs->arity = 0;
  exs->andb = andb;
  exs->insert = NULL;
  exs->arity=0;
  gc(exs);
  andb = exs->andb;
  chb = andb->father;
  yreg = andb->cont->yreg;

  if(EmptyRecall(exs)) {
    /* no ports where closed */
#ifdef TRACE
    tracing=NOTRACE;
#endif   
    ScheduleDebugPrint("Computation suspended");
    return;
  }

  ScheduleDebugPrint("Object where closed");
  goto mainhandler;


  /********************************************************************
   *
   * 	The guard handler.
   *
   */

 guardhandler:

  while(!EmptyWake(exs)){
      
    NextWake(exs,wand);

    /* if the and-box is dead it is ignored */
    if(Dead(wand))
      continue;
    if(wand->father->father == andb)
      goto wakehandler; 
    if(isdead(wand, andb))	/* dead    */
      continue;

    goto wakehandler;
  }

  while(!EmptyRecall(exs)){
      
    NextRecall(exs,rchb);

    /* if the choice-box is tried it is ignored */
    if(rchb->cont == NULL)
      continue;

    wand = rchb->father;
      
    /* if the and-box is dead it is ignored */
    if(Dead(wand))
      continue;
    if(isdead(wand, andb))	/* dead    */
      continue;

    goto recallhandler;
  }
	
  if(!Solved(andb) && EmptyTrail(exs) && Stable(andb)) {

    cand = candidate(andb->tried);
    
    if(cand != NULL) {
      NondetDebugPrint("Candidate in and-box");
      goto splithandler;
    }
  }

  goto suspendhandler;


  /********************************************************************
   * 
   *	Wake: install wand, continue.
   *
   */

 wakehandler:

  /* Save a continuation to where we currently are */

  andb->cont->label = pc;

  {
    andbox *fandb;

    /* if we fail on the way down to to
     * fandb will be set to the failing and-box
     */
	
    fandb = installb(andb,wand,exs);

    if(fandb) {
      TRACE_Down_Guard(andb,fandb->father->father);
      TRACE_Reenter_Goal(fandb->father);
      andb = fandb;
      chb = andb->father;
      goto fail;
    }
  }

  TRACE_Down_Guard(andb,wand->father->father);
  TRACE_Reenter_Goal(wand->father);
  TRACE_Wake_Guard(wand);

  andb = wand;
  chb = andb->father;

  /* Transfer control to the continuation */
  pc = andb->cont->label;
  FetchOpCode(0);
  yreg = andb->cont->yreg;
  NextOp();



  /********************************************************************
   * 
   *	Recall: Install wand, call rchb.
   *
   */

 recallhandler:
  
  /* Save a continuation to where we currently are */
  andb->cont->label = pc;

  /* if we fail on the way down to to
   * fandb will be set to the failing and-box
   */
  {
    andbox *fandb;
    fandb = installb(andb,wand,exs);

    if(fandb) {
      andb = fandb;
      chb = andb->father;
      TRACE_Down_Guard(andb,fandb->father->father);
      TRACE_Reenter_Goal(fandb->father);
      goto fail;
    }
  }
  

  TRACE_Down_Guard(andb,wand);

  andb = wand;
  chb = rchb;

  TRACE_Reenter_Goal_C(chb);
  
  { int i;
 
    for(i = 0; i < chb->cont->arity; i++)
      X(i) = chb->cont->arg[i];
  }

  if (chb->type == ZIPPEDIDOO) {
  insert = chb->previous;
    pc = chb->cont->instr.label;
    FetchOpCode(0);
    UnlinkChb(chb);
    chb->cont = NULL;
    chb = andb->father;
    yreg = andb->cont->yreg;
    NextOp();
  }

  exs->andb = andb;
  exs->insert = chb;  

  switch((chb->cont->instr.cinfo)(exs)) {
  case FALSE:
    andb = exs->andb;
    TRACE_Fail_Goal(chb);
    chb = andb->father;
    goto fail;
  case TRUE:
    andb = exs->andb;
    chb = exs->insert;
    UnlinkChb(chb);
    TRACE_Exit_Goal_C(chb);
    chb->cont = NULL;
    break;
  case SUSPEND:
    andb = exs->andb;
    chb = exs->insert;

    SuspendBuiltin(exs,chb);

    TRACE_Suspend_Goal_C(chb);
    break;

  case REDO:
    andb  = exs->andb;
    chb = exs->insert; 

    RedoBuiltin(exs, chb);
    TRACE_Suspend_Goal_C(chb);
    break;

  }
  chb = andb->father;
  pc = andb->cont->label;
  FetchOpCode(0);
  yreg = andb->cont->yreg;
  NextOp();
      
  /*********************************************************************
   *
   *	Choice splitting is performed with respect to a candidate.
   *
   */
  
 splithandler:
  {
    andbox *promoted;		/* The new copy */
    choicebox *fork;		/* The coicebox above cand */
    andbox *mother;		/* The andbox above fork */
    choicebox *root;		/* The choicebox above mother */

    if(GetOpCode(cand->cont->label) == EnumToCode(GUARD_CUT)) {
      /* install the candidate and promote it */
      TRACE_Down_Guard(andb,cand->father->father);
      TRACE_Reenter_Goal(cand->father);
      TRACE_Noisy_Pruning(cand);
      NondetDebugPrint("Noisy pruning guard");
      installb(andb,cand,exs);
      andb = cand;
      chb = andb->father;
      KillAll(chb);
      yreg = andb->cont->yreg;
      pc = andb->cont->label;
      Instr_None(DUMMY_CODE); 
      goto promotion;
    }
    
    NondetDebugPrint("Doing choice splitting");

    fork = cand->father;
    mother = fork->father;
    root = mother->father;

    assert(fork->tried->next != NULL);

    /* deinstall the andbox if it is equal to mother */
    if(andb == mother) {
      ResetState(andb);
      UndoUnifier(andb);
      UndoConstraints(andb);
      SuspendTrail(exs,andb);
      andb->cont->label = pc;
    } else {
      TRACE_Down_Guard(andb,mother);
      installb(andb,root->father,exs);
      PushContext(exs);
    }
    
    TRACE_Non_Det_Pre(cand);
    
    promoted = copy(cand,exs);

    TRACE_Non_Det_Post(promoted);

    chb = mother->father;
    
    /* If only one andbox remains the goal is deterministic */
    if(fork->tried->next == NULL) {
      Promote(exs,fork->tried);
    } else {
      if(Stable(mother)) {
	Split(exs,mother);
      }
    }

    andb = promoted;
    TRACE_Promote_Guard(andb);
    reinstall(andb->father->father);
    PushContext(exs);
    reinstall(andb);
    chb = andb->father;
    yreg = andb->cont->yreg;
    pc = andb->cont->label;
    Instr_None(DUMMY_CODE);
    goto promotion;
  }
  
  /*********************************************************************
   *
   *	The and-box is suspended.
   *
   */

 suspendhandler:

  arity = 0;
  CheckExceptions;

  /* Undo unify constraints */

  UndoUnifier(andb);

  /* Deinstall all constraints */

  UndoConstraints(andb);
  
  /* Undo the trail and add suspensions */
  
  SuspendTrail(exs,andb);

  ResetState(andb);

  TRACE_Suspend_Guard(andb);

  /* Save a pointer to where in the code we were.   */

  andb->cont->label = pc;

  /*******************************************************************
   *
   *	The choice handler.
   *
   */
  
 choicehandler:			/*  ATTENTION */
  {
    task *entry;

    while(!EmptyTask(exs)) {
      NextTask(exs,entry);

      switch(entry->type) {
	
      case PROMOTE:
	andb = entry->value.andbox;

	if(Dead(andb) || Dead(andb->father->father)) {
	  break;
	}

	reinstall(andb->father->father);

	PushContext(exs);

	chb = andb->father;
	
	reinstall(andb);

	TRACE_Down_Guard(andb->father->father->father->father,andb->father->father); 
	TRACE_Promote_Guard(andb);
	
	
	/* Transfer control to the continuation */
	pc = andb->cont->label;
	FetchOpCode(0);
	yreg = andb->cont->yreg;
	NextOp();    

      case SPLIT:
	andb = entry->value.andbox;

	if(Dead(andb)) {
	  break;
	}
	
	TRACE_Down_Guard(andb->father->father,andb);
	reinstall(andb);
	TRACE_Promote_Guard(andb);

	/* Transfer control to the continuation */
	pc = andb->cont->label;
	FetchOpCode(0);
	yreg = andb->cont->yreg;
	NextOp();

      case ROOT:
	/* this is the root choice box, there is nothing more to do */
#ifdef TRACE
	tracing=NOTRACE;
#endif
        Root(exs);
	return;

      case START:
	/* this is the first task */

	andb = exs->root->tried;
	FetchOpCode(0);
	yreg = andb->cont->yreg;	
	NextOp();
      default:
	FatalError("wrong type of task in choicehandler");
      }
    }
  }

  if(NonEmptyChoiceCont(chb)) {
    pc = chb->cont->instr.label;
    FetchOpCode(0);
    if(chb->tried != NULL)
       for(andb = chb->tried; andb->next != NULL; andb = andb->next);
    else
	andb = NULL;
	
    NextOp();
  }

  if(chb->tried == NULL) {
    /* the choicebox failed, fail the andbox above */

    TRACE_Fail_Goal(chb);

    andb = chb->father;
    chb = andb->father;
    PopContext(exs);
    goto fail;
  }
  
  TRACE_Suspend_Goal(chb);

  /*********************************************************************
   *
   *	Moving up.
   *
   */

  insert = chb;
  assert(chb->father != NULL);
  andb = chb->father;
  chb = andb->father;
  
  PopContext(exs);

  TRACE_Up_Guard(andb);
  
  pc = andb->cont->label;
  FetchOpCode(0);
  yreg = andb->cont->yreg;
  NextOp();

  /*********************************************************************
   *
   *	Failing
   *
   */
    


 fail:

  TRACE_Fail_Guard(andb);
 
  {
    andbox *failed;

    failed = andb;

    /* Deinstall unify constraints */

    UndoUnifier(failed);
    
    /* Deinstall any constraints */

    UndoConstraints(failed);

    /* Untrail any trailed stuff */

    UndoTrail(exs);

    /* Flush the tasks */

    RestoreWake(exs);

    RestoreRecall(exs);    

    /* Mark the andbox as dead */

    failed->status = DEAD;

    /* unlink the dead andbox */

    if(failed->previous != NULL)
      failed->previous->next = failed->next;
    else
      chb->tried = failed->next;
    if(failed->next != NULL)
      failed->next->previous = failed->previous;

    /* if there is only one remaining andbox, try it */
    andb = chb->tried;

    if(andb != NULL && Last(chb,andb) && Solved(andb)) {
      /* there is only one anbox and it is solved */
      failed = install(andb,exs);
      if(failed != NULL)
	goto fail;
      pc = andb->cont->label;
      FetchOpCode(0);
      yreg = andb->cont->yreg;
      insert = NULL;
      NextOp();      
    }
    goto choicehandler;
  }

  } /* end of dummy "forever" loop */
}


void engine(exs)
    exstate *exs;
{
  tengine(exs, 0);
}

void init_engine()
{
  tengine(NULL, 1);
}

