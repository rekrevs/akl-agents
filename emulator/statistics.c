/* $Id: statistics.c,v 1.18 1993/06/03 13:02:22 bd Exp $
*/

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "initial.h"
#include "storage.h"
#include "code.h"
#include "statistics.h"
#include "config.h"
#include "time.h"



int exectime;
int sexectime;

#ifdef METERING
int copytotal;
int copytime;
int gctime;
int scopytotal;
int scopytime;
int sgctime;
#endif 



bool akl_statistics_runtime(Arg)
    Argdecl;
{

    int tmp;
    register Term X0, X1;
    Term tex, tsex;
       
    Deref(X0, A(0));        
    Deref(X1, A(1));        

    tmp = systime();

    MakeIntegerTerm(tex, (tmp - exectime));
    MakeIntegerTerm(tsex, (tmp - sexectime));

    if( unify(X0, tex, exs->andb, exs) && unify(X1, tsex, exs->andb, exs)){
	sexectime = tmp;
	return TRUE;
    } else {
	sexectime = tmp;
	return FALSE;
    }
}


bool akl_statistics_copytime(Arg)
    Argdecl;
{
#ifdef METERING
    register Term X0, X1;
    Term ct, cts;

    Deref(X0, A(0));   	/* Time copied total */    
    Deref(X1, A(1));	/* Time since last statistics */
    
    MakeIntegerTerm(ct, copytime);
    MakeIntegerTerm(cts, (copytime - scopytime));

    if( unify(X0, ct, exs->andb, exs) && unify(X1, cts, exs->andb, exs) ){
      scopytime = copytime;
      return TRUE;
    } else {
      return FALSE;
    }
#else
    return FALSE;
#endif
}


bool akl_statistics_gctime(Arg)
    Argdecl;
{
#ifdef METERING
    register Term X0, X1;
    Term gct, gcts;
  
    Deref(X0, A(0));   	/* Time gc total */    
    Deref(X1, A(1));	/* Time since last statistics */
    
    MakeIntegerTerm(gct, gctime);
    MakeIntegerTerm(gcts, (gctime - sgctime));

    if( unify(X0, gct, exs->andb, exs) && unify(X1, gcts,exs->andb, exs) ){
      sgctime = gctime;
      return TRUE;
    } else {
      return FALSE;
    }
#else
    return FALSE;
#endif
}


bool akl_statistics_nondet(Arg)
    Argdecl;
{
#ifdef METERING
    register Term X0, X1;
    Term ct, cts;
  
    Deref(X0, A(0));   	/* Number of nondeterministic steps total */
    Deref(X1, A(1));	/* Steps since last statistics */
    
    MakeIntegerTerm(ct, copytotal);
    MakeIntegerTerm(cts, (copytotal - scopytotal));

    if( unify(X0, ct, exs->andb, exs) && unify(X1, cts, exs->andb, exs)){
      scopytotal = copytotal;
      return TRUE;
    } else {
      return FALSE;
    }
#else
    return FALSE;
#endif
}



/* core = memory allocated by the unix process
   program = size of constant area, used
   gc = #GCs, bytes freed, time spent
   heap = size of heap, used
*/

bool akl_statistics_program(Arg)
    Argdecl;
{
    register Term X0, X1;
    Term as, asc;
    int total_size, used_size;
       
    total_size = constsize + sizeof(code)*codesize;
    used_size = (constcurrent-constspace) + sizeof(code)*(codecurrent-codespace);

    Deref(X0, A(0));        
    Deref(X1, A(1));        

    MakeIntegerTerm(as, total_size);
    MakeIntegerTerm(asc, used_size);

    if( unify(X0, as, exs->andb, exs) && unify(X1, asc, exs->andb, exs)) {
	return TRUE;
    } else
	return FALSE;
}
/*
bool akl_statistics_core(Arg)
    Argdecl;
{
    register Term X0;
    return FALSE;
}

bool akl_statistics_gc(Arg)
    Argdecl;
{
    register Term X0,X1,X2;
    return FALSE;
}

bool akl_statistics_heap(Arg)
    Argdecl;
{
    register Term X0,X1;
    return FALSE;
}
*/

bool akl_statistics(Arg)
    Argdecl;
{
    int tmp;

    tmp = systime();

#ifdef METERING
    fprintf(stderr,"{ nondet %d ( %d ) steps. }\n", copytotal,(copytotal - scopytotal));
    fprintf(stderr,"{ copytime %d ( %d ) ms. }\n", copytime,(copytime - scopytime));
    fprintf(stderr,"{ gctime %d ( %d ) ms. }\n", gctime,(gctime - sgctime));
    fprintf(stderr,"{ runtime %d ( %d ) ms. }\n", (tmp - exectime),(tmp - sexectime));    
    scopytotal = copytotal;
    scopytime = copytime;
    sgctime = gctime;
    sexectime = tmp;
#else
    fprintf(stderr,"{ runtime %d ( %d ) ms. }\n", (tmp - exectime),(tmp - sexectime));
    fprintf(stderr,"{ for more statistics compile with -DMETERING }\n");
    sexectime = tmp;
#endif
    return TRUE;
}



void initialize_statistics() {
  
  exectime = systime();
  sexectime = exectime;

#ifdef METERING
  copytime = 0;
  gctime = 0;
  copytotal = 0;
  scopytime = 0;
  sgctime = 0;
  scopytotal = 0;
#endif

  define("$statistics_program", akl_statistics_program,2);
  define("$statistics_runtime", akl_statistics_runtime,2);
  define("$statistics_copytime", akl_statistics_copytime,2);
  define("$statistics_nondet", akl_statistics_nondet,2);
  define("$statistics_gctime", akl_statistics_gctime,2);  
/*
  define("$statistics_core", akl_statistics_core,1);
  define("$statistics_gc", akl_statistics_gc,3);
  define("$statistics_heap", akl_statistics_heap,2);
*/
  define("$statistics", akl_statistics,0);
}
