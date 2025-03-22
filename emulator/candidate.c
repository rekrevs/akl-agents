/* $Id: candidate.c,v 1.15 1994/03/29 18:08:25 jm Exp $ */ 

#include "include.h"
#include "instructions.h"
#include "term.h"
#include "initial.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"

/* For the GetOpCode() macro: */
#include "decodeinstr.h"

int innerflag;

/* local prototypes */
andbox *leftmost();
andbox *find();


bool akl_innermost(Arg)
     Argdecl;
{
  innerflag = 1;
  return TRUE;
}

bool akl_leftmost(Arg)
     Argdecl;
{
  innerflag = 0;
  return TRUE;
}

void initialize_candidate() {

  define("innermost", akl_innermost,0);
  define("leftmost", akl_leftmost,0);
}  




/* Finds a candidate for nondeterministic promotion.
 * It will find the leftmost candidate which does not
 * have a candidate in its scope.
 * 
 */

andbox *candidate(chb)
    choicebox *chb;
{
  return leftmost(chb);
}
		    



andbox *leftmost(chb)
    choicebox *chb;
{
  andbox *ab, *deep, *cand;
  choicebox *cb;

  cand = NULL;
  cb = chb;
    
 nxtcb:
    
  ab = cb->tried;
    
 nxtab:
  if(ab == NULL)
    goto endab;

  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_WAIT))) {
    if((ab->tried == NULL)) {
      cand = ab;
      goto found;
    }
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;      
  }

  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_CUT))) {
    if((ab->tried == NULL)) {
      if(ab->previous == NULL)
	return ab;
      ab = ab->next;
      goto nxtab;
    }
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;
  }
  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_COND))) {
    if((ab->tried == NULL)) {
      ab = ab->next;
      goto nxtab;
    }
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;      
  }
  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_COMMIT))) {
    if((ab->tried == NULL)) {
      ab = ab->next;
      goto nxtab;
    }
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;      
  }
  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_UNORDER))) {
    if((ab->tried == NULL)) {
      ab = ab->next;
      goto nxtab;
    }      
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;      
  }
  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_ORDER))) {
    if((ab->tried == NULL)) {
      ab = ab->next;
      goto nxtab;
    }      
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;      
  }
  if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_UNIT))) {
    if((ab->tried == NULL)) {
      ab = ab->next;
      goto nxtab;
    }      
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;
    goto nxtab;      
  }

 endab:

  cb = cb->next;
  if(cb == NULL)
    return NULL;
  goto nxtcb;


  /* we have found a candidate, look for a deeper candidate */

 found:

  if(!innerflag)
    return cand;


  ab = ab->next;

 fnxtab:
  if(ab == NULL)
    goto fendab;

  if(ab->tried != NULL) {
    deep = find(ab->tried);
    if(deep != NULL)
      return deep;
  }
  goto found;
    
 fendab:
  cb = cb->next;
  if(cb == NULL)
    return cand;
  ab = cb->tried;
  goto fnxtab;

}

andbox *find(chb)
    choicebox *chb;
{
    andbox *ab, *deep, *cand;
    choicebox *cb;

    cand = NULL;
    cb = chb;
    
 nxtcb:
    
    ab = cb->tried;
    
 nxtab:
    if(ab == NULL)
      goto endab;

    if(ab->tried == NULL) {
      if((GetOpCode(ab->cont->label) == EnumToCode(GUARD_WAIT))) {
	cand = ab;
	goto found;
      }
    } else {
      deep = find(ab->tried);
      if(deep != NULL)
	return deep;
    }

    ab = ab->next;
    goto nxtab;
    
 endab:

    cb = cb->next;
    if(cb == NULL)
      return NULL;
    goto nxtcb;


 /* we have found a candidate, look for a deeper candidate */

 found:
    ab = ab->next;

 fnxtab:
    if(ab == NULL)
      goto fendab;

    if(ab->tried != NULL) {
      deep = find(ab->tried);
      if(deep != NULL)
	return deep;
    }
    goto found;
    
 fendab:
    cb = cb->next;
    if(cb == NULL)
      return cand;
    ab = cb->tried;
    goto fnxtab;

}











