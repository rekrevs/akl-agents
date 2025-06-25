/* $Id */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "storage.h"
#include "unify.h"
#include "copy.h"
#include "gc.h"
#include "initial.h"
#include "config.h"
#include "display.h"
#include "error.h"
#include "names.h"

#include "abstraction.h"

#include "code.h"
#include "encodeinstr.h"
#include "instrdefs.h"

#include "instructions.h"



Generic newabs();
bool 	unifyabs();
int 	printabs();
Generic	copyabs();
Generic	gcabs();

predicate *apply_def;

method absmethod = {
  newabs,
  unifyabs,
  printabs,
  copyabs,
  gcabs,
  NULL,
  NULL,
  NULL,
  NULL
};


	    
Generic newabs(old)
     abstraction *old;
{
  abstraction *new;
  DerefGenEnv(old);
  if (InCopyEnv(old->env)) {
    NEWX(new, abstraction, (sizeof(abstraction) + (sizeof(Term)*AbsExt(old))));
    return (Generic) new;
  } else {
    return NULL;
  }
}

Generic copyabs(old,new)
     abstraction *old, *new;
{
  new->def = old->def;
  new->env = NewEnv(old->env);
  copy_tuple(old->arg, new->arg, AbsExt(old));
  return (Generic) new;
}

Generic gcabs(old,new,gcst)
     abstraction *old, *new;
     gcstatep gcst;
{
  new->def = old->def;
  new->env = NewEnv(old->env);
  gc_tuple(old->arg, new->arg, AbsExt(old), gcst);
  return (Generic) new;
}


bool unifyabs(x,y,andb,exs)
     Term x, y;
     andbox *andb;
     exstate *exs;
{
  if(IsAbs(y)) {
    if(AbsDef(Abs(x)) == AbsDef(Abs(y))) {
      int i;
      if(unify_alias(&x, y, exs))
           return unify_more(x, y, andb, exs);
      for(i = 0; i != AbsExt(Abs(x)); i++) {
	if( !unify_more(AbsArgRef(Abs(x),i), AbsArgRef(Abs(y),i), andb, exs) )
	  return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;
}



int printabs(file,abs,tsiz)
     FILE *file;
     abstraction *abs;
     int tsiz;
{
  fprintf(file,"{abstraction: %#lx def: %#lx}", ((long)abs), (long)abs->def);
  return 1;
}

/* These predicate are exported to the AKL level.
 *
 *
 */


/* abs/1 */
bool akl_abs(Arg)
     Argdecl;
{
  Term X0;
  Deref(X0,A(0));
  
  if(IsAbs(X0))
    return TRUE;
  IfVarSuspend(X0);
  return FALSE;
}
  


void initialize_abstraction()
{
  define("abstraction", akl_abs, 1);

  {
    Define_Predicate("apply",2);
    Instr_Index_Index(EXECUTE_APPLY_LIST,0,1);
    End_Definition;
    apply_def = GetPred(ConstAtom("apply"),2);
  }

}

