/* $Id: intrpt.h,v 1.3 1993/07/30 09:22:39 sverker Exp $ */

#include <setjmp.h>
#include <signal.h>

extern jmp_buf toplevenv;

void initialize_intrpt();

extern int interruptflag;

#define NO_INTRPT 0
#define INTRPT_ABORT 1
