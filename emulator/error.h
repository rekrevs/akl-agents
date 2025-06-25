/* $Id: error.h,v 1.18 1994/04/25 15:23:00 rch Exp $
 */

#define FatalError(String)\
    { fprintf(stderr,"{FatalError: %s}\n",String);\
      error_exit(1); }


#define Error(String)\
    fprintf(stderr,"{Error: %s}\n",String);


/* 
 * The error reporting macros may assume the existance
 * of a variable exs in their environment.
 *
 */

#define MINOR_FAULT(Arg) {return FALSE;}

#define WARNING(String) { \
    fprintf(stderr,"{WARNING: %s}\n", (String)); \
  }

#define EXCEPTION(Term, Arg) {(Term) = make_exception_term((Arg), exs->andb);}

#define ERRNO make_errno_term(exs->andb)


extern void error_exit();
extern void akl_error();
    
extern Term make_exception_term();
extern Term make_errno_term();

extern void initialize_error();

/*****************************************************************************/

#define ENGINE_ERROR_1(Culprit, Message) { \
  fprintf(stderr, "{ENGINE ERROR: "); \
  fdisplay_term(stderr, (Culprit), -1); \
  fprintf(stderr, " - %s}\n", (Message)); \
}

#define ENGINE_ERROR_2(Name, Arity, Message) { \
  fprintf(stderr, "{ENGINE ERROR: %s/%ld - %s}\n", (Name), (Arity), (Message)); \
}

/* SYSTEM_ERROR really should abort, not fail. */
#define SYSTEM_ERROR(Message) { \
  fprintf(stderr, "{SYSTEM ERROR: %s}\n", (Message)); \
  return FALSE; \
}

/*
 * The following exception raising macros, down to METHOD_EXISTENCE_ERROR,
 * assume the availability of A(_).
 */

#define DOMAIN_ERROR(Name, Arity, ArgNo, Domain) { \
  fprintf(stderr, "{DOMAIN ERROR: "); \
  PRINT_GOAL((Name), (Arity)) \
  fprintf(stderr, " - arg %d: expected %s, found ", (ArgNo), (Domain)); \
  fdisplay_term(stderr, A((ArgNo)-1), -1); \
  fprintf(stderr, "}\n"); \
  return FALSE; \
}

/* PERMISSION_ERROR really should abort, not fail. */
#define PERMISSION_ERROR(Name, Arity, Operation, ObjectType, Culprit, Message) { \
  fprintf(stderr, "{PERMISSION ERROR: "); \
  PRINT_GOAL((Name), (Arity)) \
  fprintf(stderr, " - cannot %s %s ", (Operation), (ObjectType)); \
  if ((Culprit) != NullTerm) { \
    fdisplay_term(stderr, (Culprit), -1); \
    fputc(' ', stderr); \
  } \
  fprintf(stderr, "- %s}\n", (Message)); \
  return FALSE; \
}

/* REPRESENTATION_ERROR really should abort, not fail. */
#define REPRESENTATION_ERROR(Name, Arity, ArgNo, ErrorType) { \
  fprintf(stderr, "{REPRESENTATION ERROR: "); \
  PRINT_GOAL((Name), (Arity)) \
  fprintf(stderr, " - arg %d: %s}\n ", (ArgNo), (ErrorType)); \
  return FALSE; \
}

#define PRINT_GOAL(Name, Arity) { \
  char c; \
  int i; \
  fprintf(stderr, "%s", (Name)); \
  if ((Arity) != 0) { \
    c = '('; \
    for (i = 0; i < (Arity); i++) { \
      fputc(c, stderr); \
      fdisplay_term(stderr, A(i), -1); \
      c = ','; \
    } \
    fputc(')', stderr); \
  } \
}

/* METHOD_EXISTENCE_ERROR really should abort, not fail. */
#define METHOD_EXISTENCE_ERROR(Message, Object) { \
  fprintf(stderr, "{EXISTENCE ERROR: "); \
  fdisplay_term(stderr, (Message), -1); \
  fputc('@', stderr); \
  fdisplay_term(stderr, (Object), -1); \
  fprintf(stderr, " - undefined method}\n"); \
  return FALSE; \
}

/*
 * The following exception raising macros assume the availability of A[_].
 */

#define METHOD_DOMAIN_ERROR(Name, Arity, ArgNo, Domain, Object) { \
  fprintf(stderr, "{DOMAIN ERROR: "); \
  METHOD_PRINT_GOAL((Name), (Arity), (Object)) \
  fprintf(stderr, " - arg %d: expected %s, found ", (ArgNo), (Domain)); \
  fdisplay_term(stderr, A[(ArgNo)-1], -1); \
  fprintf(stderr, "}\n"); \
  return FALSE; \
}

/* METHOD_PERMISSION_ERROR really should abort, not fail. */
#define METHOD_PERMISSION_ERROR(Name, Arity, Operation, ObjectType, Culprit, Message, Object) { \
  fprintf(stderr, "{PERMISSION ERROR: "); \
  METHOD_PRINT_GOAL((Name), (Arity), (Object)) \
  fprintf(stderr, " - cannot %s %s ", (Operation), (ObjectType)); \
  if ((Culprit) != NullTerm) { \
    fdisplay_term(stderr, (Culprit), -1); \
    fputc(' ', stderr); \
  } \
  fprintf(stderr, "- %s}\n", (Message)); \
  return FALSE; \
}

/* METHOD_REPRESENTATION_ERROR really should abort, not fail. */
#define METHOD_REPRESENTATION_ERROR(Name, Arity, ArgNo, ErrorType, Object) { \
  fprintf(stderr, "{REPRESENTATION ERROR: "); \
  METHOD_PRINT_GOAL((Name), (Arity), (Object)) \
  fprintf(stderr, " - arg %d: %s}\n ", (ArgNo), (ErrorType)); \
  return FALSE; \
}

#define METHOD_PRINT_GOAL(Name, Arity, Object) { \
  char c; \
  int i; \
  fprintf(stderr, "%s", (Name)); \
  if ((Arity) != 0) { \
    c = '('; \
    for (i = 0; i < (Arity); i++) { \
      fputc(c, stderr); \
      fdisplay_term(stderr, A[i], -1); \
      c = ','; \
    } \
    fputc(')', stderr); \
  } \
  fputc('@', stderr); \
  fdisplay_term(stderr, (Object), -1); \
}

