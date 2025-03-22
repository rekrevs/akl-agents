/* $Id: error.c,v 1.15 1994/04/08 11:42:54 rch Exp $
 */

#include <errno.h>

#include "include.h"
#include "term.h"
#include "tree.h"
#include "instructions.h"
#include "predicate.h"
#include "exstate.h"
#include "display.h"
#include "foreign.h"
#include "error.h"
#include "config.h"
#include "storage.h"
#include "names.h"
#include "abstraction.h"

Functor functor_exception_1;
Functor functor_errno_1;

void error_exit(e)
    int e;
{
#ifdef unix
    break_to_gdb();		/* defined in foreign.c */
#endif
    exit(e);
}

Term make_exception_term(err,andb)
     Term err;
     andbox *andb;
{
  Structure sexc;

  MakeStruct(sexc, functor_exception_1, andb);
  StrArgument(sexc,0) = err;

  return TagStr(sexc);
}

Term make_errno_term(andb)
     andbox *andb;
{
  Structure sexc;
  Term no;
  
  MakeStruct(sexc, functor_errno_1, andb);
  MakeIntegerTerm(no, errno);
  StrArgument(sexc,0) = no;

  return TagStr(sexc);
}


void initialize_error() {
  functor_exception_1 = store_functor(store_atom("exception"),1);
  functor_errno_1 = store_functor(store_atom("errno"),1);
}
