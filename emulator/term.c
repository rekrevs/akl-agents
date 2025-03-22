/*$Id: term.c,v 1.5 1991/12/19 16:37:45 jm Exp $*/

#include "include.h"

variable *first_var(term)
     TAGGED term;
{
  variable *tmp;
  tagvalue tag;
  int i;

  DEREF(term,term);

  switch(TagOf(term)) {
  case HVA:
  case CVA:
    return Var(term);
  case INT:
  case ATM:
    return NULL;
  case LST:
    tmp = first_var(LstCar(term));
    if(tmp != NULL)
      return tmp;
    else
      return first_var(LstCdr(term));
  case STR:
    for(i = 0; i != StructureArity(term); i++) {
      tmp = first_var(StructureArgument(term,i));
      if(tmp != NULL)
	return tmp;
    }
    return NULL;
  case GEN:
    return NULL;
  default:
    FatalError("wrong type of tag in term");
  }
}
