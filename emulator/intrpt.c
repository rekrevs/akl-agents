/* $Id: intrpt.c,v 1.7 1993/10/06 11:39:22 bd Exp $ */

#include "intrpt.h"
#include "include.h"
#include "term.h"
#include "predicate.h"
#include "trace.h"

jmp_buf toplevenv;

extern int readstring();

int interruptflag;

void sigint_h() {
  char com[500];

 top:
  fprintf(stderr, "\nAKL interruption (h for help)? ");
  if(readstring(com) == 0)
    longjmp(toplevenv,2);
  switch(com[0]) {
  case 'a':
    tracing = NOTRACE;
    debugging = NOTRACE;
    interruptflag = INTRPT_ABORT;
    return;
  case 'c':
    return;
  case 'd':
    /* [SJ] temporary... */
    debugging = LEAP;
    printf("{The debugger will first leap -- showing spypoints (debug)}\n");
    return;
  case 'e':
    longjmp(toplevenv,2);
  case 't':
    /* [SJ] temporary... */
    debugging = CREEP;
    printf("{The debugger will first creep -- showing everything (trace)}\n");
    if(tracing_init==FALSE)
      tracing=debugging;
    return;
  case 'h':
  default:
    fprintf(stderr,"\n\
AKL interrupt options:\n\
    a        abort           - cause abort\n\
    b        break           - cause break (NYI)\n\
    c        continue        - do nothing\n\
    d        debug           - start debugging\n\
    e        exit            - cause exit\n\
    t        trace           - start tracing\n\
    h        help            - get this list\n");
    goto top;
  }
}


void initialize_intrpt() {

  signal(SIGINT, sigint_h);
  interruptflag = NO_INTRPT;
}

