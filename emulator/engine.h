/* $Id: engine.h,v 1.8 1993/05/26 21:01:47 sverker Exp $ */



#define X(N) (areg[N])
#define Y(N) (yreg[N])

#define A(N) (exs->areg[N])

#define Xb(O) (*((Term *)(((u8 *)areg)+(O))))
#define Yb(O) (*((Term *)(((u8 *)yreg)+(O))))


#define Arg exs

#define Argdecl exstate *exs


#define Argproto exstate *


extern void engine();
extern void init_engine();
