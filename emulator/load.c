/* $Id: load.c,v 1.14 1994/04/13 14:57:30 rch Exp $ */

#include "include.h"
#include "term.h"
#include "initial.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "parser.tab.h"
#include "error.h"

extern FILE *yyin;
extern int parse();


void load(file)
    FILE *file;
{
    yyin = file;
    parse();
}


/* $load/1 succeed or fail */
bool akl_load(Arg)
    Argdecl;
{
    register Term X0;
    
    Deref(X0,A(0));

    if(IsATM(X0)) {
	if((yyin = fopen(AtmPname(Atm(X0)), "r")) == NULL)
	  PERMISSION_ERROR("$load", 1, "open", "file", X0, "no such file or directory")
	parse();
	fclose(yyin);
	return TRUE;
    } else
      DOMAIN_ERROR("$load", 1, 1, "atom")
}


void initialize_load() {

  define("$load",akl_load,1);
}
