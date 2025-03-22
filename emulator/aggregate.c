/* $Id: aggregate.c,v 1.9 1994/03/30 08:46:34 bd Exp $ */

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
#include "error.h"
#include "aggregate.h"

Generic newcollectobj();
bool 	unifycollectobj();
int 	printcollectobj();
Generic	copycollectobj();
Generic	gccollectobj();

method collectobjmethod = {
  newcollectobj,
  unifycollectobj,
  printcollectobj,
  copycollectobj,
  gccollectobj,
  NULL,		/* collectobjs are not put on the close list, so 'uncopied', */
  NULL,		/* 'deallocate',  */
  NULL,		/* 'kill',  */
  NULL,		/* and 'close' methods are not used */
};

Generic newcollectobj(old)
     collectobj *old;
{
  collectobj *new;
  DerefGenEnv(old);
  if (InCopyEnv(old->env)) {
    NEW(new,collectobj);
    return (Generic) new;
  } else {
    return NULL;
  }
}

Generic copycollectobj(old,new)
     collectobj *old, *new;
{
  new->env = NewEnv(old->env);
  new->value = old->value;
  copy_location(&new->value);
  return (Generic) new;
}

Generic gccollectobj(old,new,gcst)
     collectobj *old, *new;
     gcstatep gcst;
{
  new->env = NewEnv(old->env);
  new->value = old->value;
  gc_location(&new->value,gcst);
  return (Generic) new;
}

bool unifycollectobj(cobj,y,andb,exs)
     Term cobj;
     Term y;
     andbox *andb;
     exstate *exs;
{
  if(Eq(cobj, y))
    return TRUE;
  return FALSE;
}

int printcollectobj(file,cobj,tsiz)
     FILE *file;
     collectobj *cobj;
     int tsiz;
{
  fprintf(file,"{COLLECTOR: %#lx}", (long)cobj);
  return 1;
}


/* COLLECTOR/2 CollectObj, List */
bool akl_open_collectobj(Arg)
     Argdecl;
{
  Term X0, X1, res;
  collectobj *cobj;
  
  Deref(X0,A(0));		/* The term unified with the collectobj */
  Deref(X1,A(1));		/* The value associated ith the collectobj */

  MakeGeneric(cobj,collectobj,&collectobjmethod);
  cobj->env = (envid*)&(exs->andb)->env;
  cobj->value = X1;

  res = TagGen(cobj);

  return unify(X0,res,exs->andb,exs);
}

void initialize_aggregate() {

  define("COLLECTOR",akl_open_collectobj,2);
}
