/* $Id: send.c,v 1.2 1992/08/19 14:09:47 jm Exp $ */

#include "include.h"
#include "engine.h"


/* send/3 */

bool send(Arg)
     Argdecl;
{
  register Term X0;
  
  DEREF(X0,X(0)); /* The port to send to */

  if(IsGEN(X0)) {
    DerefGen(Gen(X0));
    if(IsLocalGen(Gen(X0),andb)) {
      return Gen(X0)->method->send(Arg);
    }
    return SUSPEND;
  }
  if(IsVar(X0)) {
    SuspendOn(exs,X0);
    return SUSPEND;
  }
  return FALSE;
}
    
  
