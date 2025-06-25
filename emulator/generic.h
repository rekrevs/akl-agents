typedef struct generic {
  tval 		tag;
  struct method *method;
  struct generic *next;
  envid 	*env;
} generic;


typedef struct method {
  generic	*(*copy)();
  generic	*(*gc)();
  term	 	**(*turtle)();
  int		(*garbage)();
  bool		(*send)();
  bool		(*unify)();
  bool		(*close)();
  int		(*print)();
} method;

#define NewGeneric(PTR,SIZE,METHODS) \
{ \
    NEWX(PTR,generic,SIZE); \
    ((generic)PTR)->method = METHODS; \
    ((generic)PTR->tag) = GEN; \
    ((generic)PTR->env) = andb->env; \
    AddClose(((generic)PRT),exs); \
}

#define CopyGeneric(PTR,SIZE,EXS,NEWPTR) \
{ \
    if(InCopy(((generic)PTR)->env)) { \
      NEWX(NEWPTR,generic,SIZE); \
      ((generic)NEWPTR)->method = ((generic)PTR)->method; \
      ((generic)NEWPTR->tag) = GEN; \
      ((generic)NEWPTR->env) = CpNewEnv(((generic)PTR)->env); \
      AddClose(((generic)NEWPRT),EXS); \
    } \
    else \
      NEWPTR = PTR; \
}

