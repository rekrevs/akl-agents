/* $Id: tree.h,v 1.27 1994/04/22 07:18:33 bd Exp $ */

#define Leftmost(A)	((A)->previous == NULL)
#define Rightmost(A)	((A)->next == NULL)
#define Quiet(A)	(((A)->unify == NULL) && \
			 ((A)->constr == NULL || \
			  entailed(&(A)->constr)))

#define Solved(A)	((A)->tried == NULL)

#ifdef TRACE
#define Last(C,A)	(((C)->cont->instr.label == NULL) && \
			 ((A)->next == NULL) && \
			 ((A)->previous == NULL))
#else
#define Last(C,A)	(((C)->cont == NULL) && \
			 ((A)->next == NULL) && \
			 ((A)->previous == NULL))
#endif /*TRACE*/


#define OffsetToEnv 1

#define Dead(Andb) ((Andb)->status == DEAD)

#define Live(Andb) ((Andb)->status != DEAD)

/*
#define Stable(Andb) ((Andb)->status == STABLE)
*/
#define Stable(Andb) ((Andb)->status == STABLE || (Andb)->status == XSTABLE)

/*
#define UnStable(Andb) ((Andb)->status == UNSTABLE)
*/
#define UnStable(Andb) ((Andb)->status == UNSTABLE || (Andb)->status == XUNSTABLE)

#define ChoiceContSize(Arity)  (sizeof(choicecont) + sizeof(Term)*((Arity) - ANY))

#define AndContSize(Arity) 	(sizeof(andcont) + sizeof(Term) * ((Arity) - ANY))

#define UnifierSize (sizeof(unifier))

/*
#define ResetState(A)
#define SetState(A)
*/
#define ResetState(A) A->status += 2
#define SetState(A) A->status -= 2

/*
typedef enum {
  DEAD,
  STABLE,
  UNSTABLE
} sflag;
*/
typedef enum {
  DEAD,
  STABLE,
  UNSTABLE,
  XSTABLE,
  XUNSTABLE
} sflag;

typedef struct andbox {
  sflag			status;
  struct envid		*env;
#ifdef TRACE
  int 			trace;
  int 			clno;
  int			id;
#endif
  struct unifier	*unify;  
  struct constraint	*constr;
  struct choicebox	*tried;
  struct andcont	*cont;
  struct choicebox	*father;
  struct andbox		*next, *previous;
} andbox;

typedef struct andcont {
#ifdef TRACE
  int 			trace;
  struct predicate      *def;
  int                   clno;
  struct choicecont     *choice_cont;
#endif
  code			*label;
  struct andcont 	*next;
  int			ysize;
  Term			yreg[ANY];
} andcont;

typedef struct choicebox {
#ifdef TRACE
  int 			trace;
  int                   next_clno;
  int			id;
#endif
  struct choicecont	*cont;
  struct choicebox 	*next, *previous;
  struct andbox		*father;
  struct predicate	*def;
  struct andbox		*tried;
  int			type;
} choicebox;

#define ZIPPEDIDOO	17

typedef struct choicecont {
  union {
    code	*label;
    bool	(*cinfo)();
  } instr;
  int 			arity;
  Term 			arg[ANY];
} choicecont;

typedef struct constrtable {
  bool			(*install)();
  void			(*reinstall)();
  void			(*deinstall)();
  void			(*promote)();
  int 			(*print)();
  struct constraint     *(*copy)();
  struct constraint     *(*gc)();
} constrtable;

typedef struct constraint {
  constrtable		*method;
  struct constraint 	*next;
} constraint;

typedef struct unifier {
  struct unifier	*next;
  Term			arg1;
  Term			arg2;
} unifier;

typedef enum {
  CHB,
/*CHB_USER,*/
  ANDB
} susptag;

typedef union redo {
  andbox 		*andb;
  choicebox 		*chb;
} redo;

typedef struct suspension {
  susptag		type;
  struct suspension	*next;
  redo		 	suspended;
} suspension;

