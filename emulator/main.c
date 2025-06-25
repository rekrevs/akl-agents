/* $Id: main.c,v 1.26 1993/06/21 13:37:32 bd Exp $ */ 

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "load.h"
#include "storage.h"
#include "config.h"
#include "trace.h"
#include "initial.h"
#include "foreign.h"
#include "names.h"
#include "display.h"
#include "intrpt.h"

int akl_argc;
char **akl_argv;

extern int yydebug;


int main(argc,argv)
     int argc;
     char **argv;
{
    int i;
    char *bootfiles[BOOTMAX];
    FILE *bootfile;
    int bootnr;
    exstate *exs;

#ifdef macintosh
    argc = setup_argv(&argv);
#endif

    /* Process commands */
    akl_argc = 0;
    akl_argv = argv+argc;
    bootnr = 0;

    for (i=1; i<argc; i++) {
/*
      if (strcmp(argv[i],"-yydebug")==SAME) {
	yydebug = 1;
      }
*/

      if (strcmp(argv[i],"-b")==SAME) {
	i++;
	bootfiles[bootnr++] = argv[i];
      } else {
	akl_argc = argc-i;
	akl_argv = argv+i;
	break;
      }
      
    }

    pred_protection = FALSE;
    pred_protect = TRUE;

    initialise();

    {
	char pathBuf[MAXPATHLEN+1];

	if(bootnr == 0)
	    fprintf(stderr, "No bootfile!\n"),
	    exit(1);

	for(i=0; i < bootnr; i++) {

	    if(!expand_file_name(bootfiles[i], pathBuf))
		fprintf(stderr, "Strange bootfile path -- %s!\n", bootfiles[i]),
		exit(1);

	    if((bootfile = fopen(pathBuf,"r")) == NULL)
		fprintf(stderr, "Cannot find bootfile -- %s!\n", pathBuf),
		exit(1);

	    load(bootfile);
	    fclose(bootfile);
	  }
      }

    pred_protection = TRUE;
    pred_protect = FALSE;

    {
      predicate *main, *restart;
      choicebox *root;
      andbox *mother;

      main = get_predicate(store_atom("main"),0);

      if (main->predicate_type == UNDEFINED_PREDICATE) {
	fprintf(stderr, "No main/0-definition in boot-file(s).\n");
	exit(1);
      }

      restart = get_predicate(store_atom("restart"),0);

      if (restart->predicate_type == UNDEFINED_PREDICATE) {
	fprintf(stderr, "No restart/0-definition in boot-file(s).\n");
	exit(1);
      }

      exs = init_exstate();

      PushContext(exs);

      i = setjmp(toplevenv);

      if (i==2)
	exit(0);
      else if (i==1)
	main = restart;

    mainretry:
                
      
      MakeRootChoicebox(root,NULL,NULL);

      MakeEmptyChoiceCont(root,exs->areg,0);
      
      root->previous = NULL;
      root->next = NULL;

      Root(exs);

      Start(exs);

      MakeAndbox(mother, root);

      /* halt is pointing to a guard_halt instruction */
      MakeAndCont(mother,halt,NULL,0);

      root->tried = mother;
      
      exs->pc = main->code.incoreinfo;
      exs->arity = main->arity,
      exs->def = main;
      exs->insert = NULL;
      
      exs->andb = mother;
      exs->root = root;
      
      engine(exs);

      if(exs->root->tried == NULL) {
	fprintf(stderr,"{Configuration failed}\n");
      } else {
      	if(exs->root->tried->tried != NULL) {
	  fprintf(stderr,"{Configuration suspended}\n");
	  display_choicebox(exs->root, NULL,NULL);
	  goto mainretry;
	}
      }
    }
    return 0;
}

