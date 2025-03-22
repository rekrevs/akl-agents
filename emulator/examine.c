/* $Id: examine.c,v 1.34 1994/03/25 18:19:31 bd Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "unify.h"
#include "trace.h"
#include "storage.h"
#include "config.h"
#include "error.h"

/* true when from is under a dead and-box, used when determining
 * whether to wake a suspended goal or not.
 */
int isdead(from,to)
    andbox *from, *to;
{
 start:
    if(from == NULL) return FALSE;
    if(from == to) return FALSE;
    if(Dead(from)) return TRUE;
    from = from->father->father;
    goto start;
}

/* true when from is under or equal to to, used when determining
 * whether a constraint is in the scope of a binding
 */
int isunder(from,to)
    andbox *from, *to;
{

 start:
    if(from == NULL) return FALSE;
    if(Dead(from)) return FALSE;
    if(from == to) return TRUE;
    from = from->father->father;
    goto start;
}


andbox *install(andb,exs)
  andbox *andb;
  exstate *exs;
{
  unifier *lastu;
  unifier *unif;
  constraint *constr;
  constraint *allconstr;
  
  Term V0, V1;
  Term X0 ,X1;

  SetState(andb);

  lastu = NULL;

  /* [BD] We do value trailing in the unifier list.
   * Note that this cannot work in a parallell implementation.
   */

  allconstr = andb->constr;
  andb->constr = NULL;

  for(unif = andb->unify; unif != NULL; unif = unif->next) {
    /* install unify constraints */
    V0 = unif->arg1;
    V1 = unif->arg2;

    if(IsVar(V0) && IsUnBoundRef(Ref(V0))
       && (IsNonVar(V1) || IsUnBoundRef(Ref(V1)))) {
      /* V0 is an unbound variable */
      /* V1 is a non-variable or an unbound variable */
      /* Nothing is changed */
      BindUnifier(unif);
      lastu = unif;
    } else {
      Deref(X0,V0);
      Deref(X1,V1);
      if(!unify(X0,X1,andb,exs)) {
	if(lastu != NULL)
	  lastu->next = NULL;
	else
	  andb->unify = NULL;
	return andb;
      } else {
	if(lastu != NULL)
	  lastu->next = unif->next;
	else
	  andb->unify = unif->next;
      }
    
    }
  }

  while ((constr = allconstr)) {
    constrtable *meth = constr->method;
    if (meth == NULL)
      allconstr = allconstr->next;
    else
      switch (meth->install(constr,andb,exs)) {
      case FALSE:
	LinkConstr(andb,constr);
	return andb;
      case TRUE:
	allconstr = allconstr->next;
	break;
      case SUSPEND:
	allconstr = allconstr->next;
	LinkConstr(andb,constr);
	break;
      }
  }
  return NULL;
}


andbox *installb(from, to, exs)
     andbox* from, *to;
     exstate *exs;
{
  andbox *fail;
  
  if( from == to ) {
    return NULL;
  }

  fail = installb(from,to->father->father,exs);

  if(fail)
    return fail;

  PushContext(exs);

  return install(to,exs);
}
  

  
/* examine the suspensions of the variable, make WAKE and RETRY entries */  
void examine(var, andb, exs)
     Reference var;
     andbox *andb;
     exstate *exs;
{
  suspension *s, *prev;
  
  prev = NULL;
  for(s = SvaSusp(GvaSva(RefGva(var))); s != NULL; s = s->next) {
    switch(s->type) {
    case ANDB:
      if(isunder(s->suspended.andb,andb)) {
	/* make a wake entry on the taskstack */
	Wake(exs,s->suspended.andb);
	/* remove the suspension from the variable */
	UnlinkSusp(var,prev,s);
      } else {
	prev = s;
      }
      break;
    case CHB:
      if(isunder(s->suspended.chb->father,andb)) {
	/* make a retry entry on the taskstack */
	Recall(exs,s->suspended.chb);
	/* remove the suspension from the variable */
	UnlinkSusp(var,prev,s);
      } else {
	prev = s;
      }
      break;
    default:
      FatalError("Wrong type of suspension");
    }
  }
}
      
