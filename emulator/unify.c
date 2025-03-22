/* $Id: unify.c,v 1.34 1994/05/05 09:37:04 secher Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "debug.h"
#include "ptrhash.h"
#include "unify.h"
#include "examine.h"
#include "error.h"


/* unify() expects its arguments to be dereferenced.
 * It maintains a state which consists of a control stack and a hash table.
 * The stack keeps track of pairs of structure arguments that have not yet
 * been unified, and the hash table is used to record each pair of compound
 * structures as they are encountered. This allows for unification of cyclic
 * terms, and it also helps in the case of shared structures.
 *
 * unify_more() performs the unification, but does not reset the state.
 * Its purpose is to be called from within unification methods for generic
 * objects.
 *
 * unify_internal() is an alternative version of unify that calls
 * unify_more directly (unify itself tries to do cheap unification
 * before invoking the heavy machinery).
 */


#define UNIFY_INTERNAL(res) \
  { \
    exs->urec.sp = &exs->urec.stack[-1]; \
   \
    (res) = unify_more(x, y, andb, exs); \
   \
    if (exs->urec.table_used_flag) { \
      clear_hash_table(exs->urec.table); \
      exs->urec.table_used_flag = 0; \
    } \
  }

bool unify_internal(x, y, andb, exs)
     andbox *andb;
     Term x,y;
     exstate *exs;
{
  register bool res;
  UNIFY_INTERNAL(res)
  return res;
}


bool unify(x, y, andb, exs)
     andbox *andb;
     Term x,y;
     exstate *exs;
{
  register bool res;
  CheapUnify_ELSE(x, y, andb, res) 
    UNIFY_INTERNAL(res)
  return res;
}


#define BindCheckUVA(V,T,W,A,BindFail) \
  if (IsLocalUVA(V,A)) {\
    BindCheck(V,T,BindFail);\
  } else if (!bind_external_uva(W,A,V,T)) {\
    BindFail;\
  }

#define BindCheckSVA(V,T,W,A,BindFail) \
  if (IsLocalGVA(V,A)) {\
    WakeAll(W,V);\
    BindCheck(V,T,BindFail);\
  } else if (!bind_external_sva(W,A,V,T)) {\
    BindFail;\
  }

bool unify_more(x,y,andb,exs)
     andbox *andb;
     Term x,y;
     exstate *exs;
{
  register Reference xref, yref;
  unsigned long hval;
  hash_entry *eptr;
  struct unifystate *u = &exs->urec;

 loop:
  if(Eq(x, y))
    goto succeed;

  if (IsREF(y)) {
    yref = Ref(y);
    if (IsREF(x)) {
      xref = Ref(x);
      /* Take care of the myriads of variable/variable cases.
       * The rules are:
       *
       *	Bind a local UVA or SVA to any external variable.
       *	Bind an UVA to a SVA.
       *	Bind an UVA or SVA to any CVA variable.
       *	Unify two CVA variables with a unify method call.
       */
      if (VarIsUVA(xref)) {
	DerefUvaEnv(xref);
	if (IsLocalUVA(xref, andb)) {			/* x is UVA */
	  BindCheck(xref, y, goto failure);
	} else if (VarIsUVA(yref)) {
	  BindCheckUVA(yref, x, exs, andb, goto failure);
	} else if (GvaIsSva(RefGva(yref))) {
	  BindCheckSVA(yref, x, exs, andb, goto failure);
	} else {
	  BindCheckUVA(xref, y, exs, andb, goto failure);
	}
      } else {
	DerefGvaEnv(RefGva(xref));			/* x is GVA */
	if (GvaIsSva(RefGva(xref))) {
	  if (IsLocalGVA(xref, andb)) {			/* x is SVA */
	    if (VarIsUVA(yref)) {
	      DerefUvaEnv(yref);
	      if (IsLocalUVA(yref, andb)) {
		BindCheck(yref, x, goto failure);
	      } else {
		/* We already know that x is local: */
		WakeAll(exs, xref);
		BindCheck(xref, y, goto failure);
	      }
	    } else {
	      BindCheckSVA(xref, y, exs, andb, goto failure);
	    }
	  } else if (VarIsUVA(yref)) {
	    BindCheckUVA(yref, x, exs, andb, goto failure);
	  } else if (GvaIsSva(RefGva(yref))) {
	    BindCheckSVA(yref, x, exs, andb, goto failure);
	  } else {
	    /* We already know that x is external: */
	    TrailRef(exs, xref);
	    /* WakeLocalSVA(exs, xref, andb); */
	    Examine(exs, xref, andb);
	    BindCheck(xref, y, goto failure);
	  }
	} else if (VarIsUVA(yref)) {			/* x is CVA */
	  BindCheckUVA(yref, x, exs, andb, goto failure);
	} else if (GvaIsSva(RefGva(yref))) {
	  BindCheckSVA(yref, x, exs, andb, goto failure);
	} else if (IsLocalGVA(xref, andb)) {
	  /* We prefer to use the local variable's method */
	  if (!(RefCvaMethod(xref)->unify(x,y,andb,exs))) {
	    goto failure;
	  }
	} else {
	  if (!(RefCvaMethod(yref)->unify(y,x,andb,exs))) {
	    goto failure;
	  }
	}
      }
      goto succeed;
    } else {					/* IsNonVar(x) */
      xref = Ref(y);
      y = x;
      goto BindTerm;
    }	
  } else if (IsREF(x)) {
    xref = Ref(x);
  BindTerm:
    /* Bind xref to y */
    BindVariable(exs, andb, xref, y, goto failure);
    goto succeed;
  } else { /* unify non-variables */
    int index;
    Term *xtuple, *ytuple;

    SwitchTag(x,badcase,badcase,immcase,bigcase,fltcase,lstcase,strcase,gencase);
 badcase:
    FatalError("unify: tags are broken");
 immcase:
      goto failure;		/* Eq test was done earlier */
 bigcase:
      if (IsBIG(y) && bignum_compare(x, y) == 0)
        goto succeed;
      goto failure;
 fltcase:
      if (IsFLT(y) && FltVal(Flt(x)) == FltVal(Flt(y)))
        goto succeed;
      goto failure;
 lstcase:
    if (IsLST(y)) {
      xtuple = LstCdrRef(Lst(x));
      ytuple = LstCdrRef(Lst(y));
      index = 1;

    do_tuples:

      hval = TermHashValue(x);
      TermHashLookup(u->table, x, hval, eptr, goto not_found);
      x = GetEntryTerm(eptr);
      goto loop;

    not_found:
      TermHashEnter(u->table, eptr, x, y);
      u->table_used_flag = 1;

      if (index > 0) {
	/* More than one argument, so we push a stack entry for the rest */
	u->sp += 1;
	u->sp->tuple1 = xtuple;
	u->sp->tuple2 = ytuple;
	u->sp->index = index;
	if (u->sp == u->stack_end) {
	  /* Grow the stack by reallocating it... */
	  long sp_index = u->sp - &u->stack[0];
	  u->stack_length *= 2;
	  u->stack =
	    (unify_entry *) realloc(u->stack,
				    u->stack_length * sizeof(unify_entry));
	  if (u->stack == NULL) {
	    FatalError("Couldn't grow unification stack");
	  }
	  u->stack_end = &u->stack[u->stack_length - 1];
	  u->sp = &u->stack[0] + sp_index;
	}
      }
      GetArg(x, xtuple-index);
      Deref(x, x);
      GetArg(y, ytuple-index);
      Deref(y, y);
      goto loop;
    }
    goto failure;
 strcase:
    if (IsSTR(y)) {
      Structure xstr = Str(x), ystr = Str(y);
      if(StrFunctor(xstr) == StrFunctor(ystr)) {
	index = StrArity(xstr)-1;
	xtuple = StrArgRef(xstr, index);
	ytuple = StrArgRef(ystr, index);
	goto do_tuples;
      }
    }
    goto failure;
 gencase:
    if (Gen(x)->method->unify(x,y,andb,exs))
      goto succeed;
    else
      goto failure;
  }
 failure:
  return FALSE;			/* Failure return */

 succeed:
  if (u->sp == &u->stack[-1]) {
    return TRUE;
  } else {
    int index = u->sp->index;
    /* Process the next argument */
    index -= 1;
    GetArg(x, u->sp->tuple1 - index);
    Deref(x, x);
    GetArg(y, u->sp->tuple2 - index);
    Deref(y, y);

    if (index > 0) {
      u->sp->index = index;
    } else {
      /* This was the last argument of this tuple, so pop the stack */
      u->sp -= 1;
    }
    goto loop;
  }
}


/* unify_alias() is a callback procedure to be used from the unify-methods
 * of generic objects that need to detect cycles. It is used in conjunction
 * with unify_more(), in the following way: suppose our unify-method is called
 * to unify x and y. If we find (x,x') in the hash table, this means that
 * we have earlier assumed or proved that x and x' will unify. If this is
 * the case, we try to unify x' and y instead of x and y. Otherwise we enter
 * (x,y) into the hash table and proceed to unify the contents of x with
 * the contents of y, by one or more calls to unify_more. If we have a cycle
 * somewhere, unify_more will eventually end up with two identical pointers,
 * and return TRUE.
 *
 * unify_alias() does all the necessary hash table operations.
 * It returns TRUE if an x' (alias) is found, FALSE otherwise.
 * A small example of a unify-method that uses unify_alias() :
 *
 *      bool unifyfoo(x,y,andb,exs)
 *          Term x, y;
 *          andbox *andb; exstate *exs;
 *      {
 *        if(!IsFoo(y))
 *          return FALSE;
 *        if(Foo(x)->flag != Foo(y)->flag)
 *          return FALSE;
 *        if(unify_alias(&x, y, exs))
 *          return unify_more(x, y, andb, exs);
 *
 *        return unify_more(Foo(x)->bar, Foo(y)->bar, andb, exs)
 *              && unify_more(Foo(x)->baz, Foo(y)->baz, andb, exs);
 *      }
 * 
 *
 */

bool unify_alias(refx, y, exs)
     Term *refx;
     Term y;
     exstate *exs;
{
  unsigned long hval;
  hash_entry *eptr;
  Term x = *refx;
  struct unifystate *u = &exs->urec;
  hval = TermHashValue(x);
  TermHashLookup(u->table, x, hval, eptr, goto not_found);
  *refx = GetEntryTerm(eptr);
  return TRUE;
 not_found:
  TermHashEnter(u->table, eptr, x, y);
  u->table_used_flag = 1;
  return FALSE;
}


void init_unifystate(exs)
     exstate *exs;
{
  struct unifystate *u = &exs->urec;
  u->stack = NULL;
  u->table = NULL;
  u->table_used_flag = 0;

  u->stack_length = UNIFY_STACK_LENGTH;
  u->stack =
    (unify_entry *) malloc(UNIFY_STACK_LENGTH*sizeof(unify_entry));
  if (u->stack == NULL) {
    FatalError("Couldn't allocate unify stack");
  }
  u->stack_end = &u->stack[UNIFY_STACK_LENGTH - 1];

  u->table = new_hash_table(UNIFY_TABLE_LENGTH);
  if (u->table == NULL) {
    FatalError("Couldn't allocate unify table");
  }

}
