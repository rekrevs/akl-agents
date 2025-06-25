/* $Id: bam.c,v 1.5 1993/10/06 11:38:33 bd Exp $ */

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

#include "aggregate.h"
#include "regdefs.h"
#include "port.h"

#include "decodeinstr.h"
#include "instrdefs.h"


#ifdef BAM

andbox *dummy_local_andbox;

int alias_check_stack(exs,cons_top,cons_bottom,areg)
 exstate *exs;
 cons_stack_cell *cons_top,*cons_bottom;
 Term *areg;
{
  
  cons_stack_cell *top,*bottom;		
  cons2 c2;
  indx i;
  u16 ar;
  int no_entries1,no_entries2;
  block *saved_block;
  heap *saved_H,*heap_term;
  trailentry *tr,*stop;

  SaveHeap(saved_block, saved_H);

  no_entries1= exs->trail.current - exs->context.current->trail;

  for(top=cons_bottom;top<=cons_top;top++)    {
    switch(Cons_Tag(top->cons1))      {
      case CONS_TAG_REG_PS:
        i=ConsGetReg(top->cons1);
	c2=top->cons2;
	if(IsPseudoLST(c2.pseudo_term))
	  {
	    heap_term = heapcurrent; /* ATTENTION */
	    make_args_from_pseudo(LstSize/sizeof(Term),Lst(c2.pseudo_term),0);
	    if((unify(Xb(i),TagLst(heap_term),dummy_local_andbox,exs))==FALSE)
	      goto fail;
	    break;
	  }
	if(IsPseudoSTR(c2.pseudo_term))
	  {
	    heap_term = heapcurrent; /* ATTENTION */
	    ar=PseudoGetArity(c2.pseudo_term);
	    make_args_from_pseudo(StrSize(ar)/sizeof(Term),Str(c2.pseudo_term),0);
	    if((unify(Xb(i),TagStr(heap_term),dummy_local_andbox,exs))== FALSE)
	      goto fail;
	    break;
	  }
	if(IsPseudoIMM(c2.pseudo_term))
	  {
	    if(unify(Xb(i),c2.pseudo_term,dummy_local_andbox,exs)== FALSE)
	      goto fail;
	    break;
	  }
	if(IsPseudoTYPE(c2.pseudo_term)) break;
	FatalError("impossible pseudo_type");
      case CONS_TAG_REG_VAR:
      case CONS_TAG_REG_NONVAR:
        i=ConsGetReg(top->cons1);
        if((unify(Xb(i),Xb(c2.reg),dummy_local_andbox,exs)) == FALSE) goto fail;
	break;
      case CONS_TAG_TEST:
	break;
      }
  }
 
/* success */

  RestoreHeap(saved_block, saved_H);

  no_entries2= (exs->trail.current - exs->context.current->trail) - no_entries1;

  return no_entries2;

fail:

  /* undo bindings */

  no_entries2= (exs->trail.current - exs->context.current->trail) - no_entries1;

  tr = exs->trail.current;
  stop= tr - no_entries2;

  for(; tr != stop ; tr--) {
    Bind(Ref(tr->var),tr->val);
  }
  exs->trail.current = tr;

  RestoreHeap(saved_block, saved_H);

  return ALIAS_CHECK_FAIL;
}


int make_args_from_pseudo(argno,pt,offset)
    int argno;
    pseudo_term pt;
    int offset;			/* offset between end of structure block and free heap */
{
  int count=0;
  int ar,temp;

  for(;argno>0;argno--)
    {
      if(IsPseudoLST(pt)) {
	ListPush(offset+argno);
	temp = make_args_from_pseudo(LstSize/sizeof(Term),Lst(pt),offset);
	offset += temp;
	count += temp;
	continue;
      }
      if(IsPseudoSTR(pt)) {
	StrPush(offset+argno);
	ar=PseudoGetArity(pt);
	temp = make_args_from_pseudo(StrSize(ar)/sizeof(Term),Str(pt),offset);
	offset += temp;
	count += temp;
	continue;
      }
      if(IsPseudoIMM(pt)) {
	/* bd */
	continue;
      }
      if(IsPseudoREG(pt)) {
	/* bd */
	continue;
      }
      FatalError("should not arrive here in make_args_from_pseudo");
    }
  return count;
}
  
void initialize_bam(exs)
 exstate *exs;  
{
  cons_stack_cell *cons;
  try_stack_cell *try;
  
  exs->cons_stack.size = CONS_STACK_SIZE;
  cons= (cons_stack_cell *) malloc(sizeof(cons_stack_cell) * (CONS_STACK_SIZE + CONS_STACK_BUFFER));
  exs->cons_stack.start = cons;
  exs->cons_stack.current= cons;
  exs->cons_stack.end = cons + CONS_STACK_SIZE;

  exs->try_stack.size = TRY_STACK_SIZE;
  try= (try_stack_cell *) malloc(sizeof(try_stack_cell) * (TRY_STACK_SIZE + TRY_STACK_BUFFER));
  exs->try_stack.start = try;
  exs->try_stack.current= try;
  exs->try_stack.end = try + TRY_STACK_SIZE;

  dummy_local_andbox = (andbox *) malloc(sizeof(andbox));
  dummy_local_andbox->env = NULL;
  
}

cons_stack_cell *reinit_cons_stack(exs,cons)
     exstate *exs;
     cons_stack_cell *cons;
{
  FatalError("not implemented reinit_cons_stack");
}

try_stack_cell *reinit_try_stack(exs,try)
     exstate *exs;
     try_stack_cell *try;
{
  FatalError("not implemented reinit_try_stack");
}

#endif
