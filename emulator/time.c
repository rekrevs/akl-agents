/* $Id: time.c,v 1.10 1994/05/17 13:16:31 jm Exp $  */ 
#include "sysdeps.h"

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "initial.h"
#include "storage.h"
#include "config.h"
#include "names.h"
#include "error.h"
#include "display.h"
#include "functor.h"
#include "compare.h"
#include "time.h"

#ifdef unix
#include <sys/time.h>
#include <sys/resource.h>


#ifndef HAS_NO_GETRUSAGE

struct rusage u;

int systime() {
  getrusage(RUSAGE_SELF,&u);
  return u.ru_utime.tv_sec*1000 + u.ru_utime.tv_usec / 1000;
}

#else
/* Define or fake the systime() function here.
 * If we are running under UNIX System 5, we can do it with clock().
 */
#ifdef SYS5

int systime() {
  /* [BD] This gives user+sys time, not quite the same as ru_utime above. */
  return (int)(clock()/(CLOCKS_PER_SEC/1000));
}

#endif
#endif
#endif



bool akl_systime(Arg)
    Argdecl;
{

    int tmp;
    register Term X0;
    Term tex;
       
    Deref(X0, A(0));        

    tmp = systime();

    MakeIntegerTerm(tex, tmp);

    return unify(X0, tex, exs->andb, exs);
}



bool akl_sysuntil(Arg)
    Argdecl;
{
  int now, until;
  register Term X0, X1, trm;
       
  Deref(X0, A(0));        
  Deref(X1, A(1));        

  if (!IsINT(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("until", 2, 1, "integer");
  }

  until = GetInteger(X0);
    
  now = systime();

  if(until < now) {
    MakeIntegerTerm(trm, now);
    return unify(X1,trm,exs->andb,exs);
  } else {
    return REDO;
  }
}
     



void initialize_time() {
  define("systime", akl_systime,1);
  define("sysuntil", akl_sysuntil,2);
}

