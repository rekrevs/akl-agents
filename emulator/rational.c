/* $Id: rational.c,v 1.14 1994/03/08 09:24:00 bd Exp $ */

#if !defined(NOBIGNUM) && defined(RATNUM)

/* [BD] Rationals need fixing */

typedef struct mp_bignum mp_bignum, *MP_Bignum;

#define Bignum MP_Bignum

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "gmp.h"
#include "unify.h"
#include "gc.h"
#include "copy.h"
#include "storage.h"
#include "config.h"
#include "initial.h"
#include "bignum.h"
#include "rational.h"
#include "error.h"


Generic	newrat();
bool 	unifyrat();
int 	printrat();
Generic	copyrat();
Generic	gcrat();
int     uncopiedrat();
envid	*deallocaterat();
int 	killrat();
bool	closerat();


method ratmethod =
{
  newrat,
  unifyrat,
  printrat,
  copyrat,
  gcrat,
  uncopiedrat,
  deallocaterat,
  NULL,				/* no kill */
  NULL				/* no close */
};

typedef struct {
  generic	generic;
  MP_RAT	mp_rat;
} RatType;

#define Rat(Prt)  ((RatType *)Prt)
#define IsRational(Prt) (IsGEN(Prt) && Gen(Prt)->method == &(ratmethod))


/* defs from bignum.c */

struct mp_bignum {
  uword		tag2;
  MP_INT	mp_int;
};

#define Bign(Prt)  ((Bignum)Prt)
#define IsBignum(Prt) (IsBIG(Prt))


/* -------------------------------------------------------------------- */

Generic newrat(old)
     RatType *old;
{
  RatType *new;

  NEW(new, RatType);

  return (Generic) new;
}

/* -------------------------------------------------------------------- */

Generic copyrat(old,new)
     RatType *old, *new;
{
  mpq_init(&new->mp_rat);	/* memory leak!!!!! */
  mpq_set(&new->mp_rat, &old->mp_rat);

  return (Generic) new;
}

/* -------------------------------------------------------------------- */

Generic gcrat(old,new,gcst)
     RatType *old, *new;
     gcstatep gcst;
{
  mpq_init(&new->mp_rat);	/* memory leak!!!!! */
  mpq_set(&new->mp_rat, &old->mp_rat);

  return (Generic) new;
}

/* -------------------------------------------------------------------- */

envid *deallocaterat(prt)
     generic *prt;
{
  return NULL;
}

/* -------------------------------------------------------------------- */

int uncopiedrat(prt)
     generic *prt;
{
  return 0;
}

/* -------------------------------------------------------------------- */

bool unifyrat(xt,yt,andb,exs)
     Term xt, yt;
     andbox *andb;
     exstate *exs;
{
  if (!IsRational(xt)) {
    FatalError("unifyrat called with non-Rational argument");
  } else {
    RatType *x = Rat(Gen(xt));
    if (IsRational(yt)) {
      RatType *y = Rat(Gen(yt));
      return mpq_cmp(&x->mp_rat,&y->mp_rat) == 0;
    }
  }
  return FALSE;
}

/* -------------------------------------------------------------------- */

int printrat(file,rat,tsiz)
     FILE *file;
     RatType *rat;
     int tsiz;
{
  extern char *salloc();
  char *buf;
  MP_INT t0, t1;

  mpz_init(&t0);		/* memory leak */
  mpz_init(&t1);		/* memory leak */
  mpq_get_num(&t0, &rat->mp_rat);
  mpq_get_den(&t1, &rat->mp_rat);

  buf = salloc(mpz_sizeinbase(&t0,10) + 2);
  mpz_get_str(buf, 10, &t0);
  fprintf(file,"r'%s/", buf);
  free(buf);			/* should use sfree or something */

  buf = salloc(mpz_sizeinbase(&t1,10) + 2);
  mpz_get_str(buf, 10, &t1);
  fprintf(file,"%s", buf);
  free(buf);			/* should use sfree or something */
  return 1;
}

/* -------------------------------------------------------------------- */

bool akl_rat(Arg)
    Argdecl;
{
  register Term X0;
    
  Deref(X0, A(0));    
  IfVarSuspend(X0);
  return IsRational(X0);
}


/*************************************************************************/

#define RatArithmetic(Operator)\
{\
  Term X0, X1, X2;\
\
  Deref(X0, A(0));\
  Deref(X1, A(1));\
  Deref(X2, A(2));\
  \
  if ((IsRational(X0) && IsRational(X1))) {\
    RatType *res;\
    MakeGeneric(res, RatType, &ratmethod);\
\
    mpq_init(&res->mp_rat);\
\
    Operator (&res->mp_rat, &Rat(Gen(X0))->mp_rat, &Rat(Gen(X1))->mp_rat);\
\
    return unify(X2, TagGen(res), exs->andb,exs);\
  }\
  IfVarSuspend(X0); \
  IfVarSuspend(X1); \
  return FALSE;\
}

#define RatComparison(Operator)\
{\
  Term X0, X1;\
    \
  Deref(X0, A(0));        \
  Deref(X1, A(1));        \
\
  if ((IsRational(X0) || IsVar(X0)) &&\
     (IsRational(X1) || IsVar(X1))) {\
    IfVarSuspend(X0);\
    IfVarSuspend(X1);\
    return\
      (mpq_cmp(&Rat(Gen(X0))->mp_rat,&Rat(Gen(X1))->mp_rat) Operator 0) ? TRUE:FALSE;\
  }\
  return FALSE;\
}


bool akl_big_to_rat(Arg)
     Argdecl;
{
  Term X0, X1, X2;
  RatType *rat;

  Deref(X0, A(0));
  Deref(X1, A(1));

  if (IsBignum(X0) && IsBignum(X1))
    {
      MakeGeneric(rat, RatType, &ratmethod);
      mpq_init(&rat->mp_rat);
      mpq_set_num(&rat->mp_rat,&Bign(Gen(X0))->mp_int);
      mpq_set_den(&rat->mp_rat,&Bign(Gen(X1))->mp_int);
      Deref(X2, A(2));
      return unify(X2, TagGen(rat), exs->andb,exs);
    }
  IfVarSuspend(X0);
  IfVarSuspend(X1);
  return FALSE;
}


bool akl_rat_to_big(Arg)
     Argdecl;
{
  Term X0, X1, X2;
  Bignum bn0, bn1;

  Deref(X0, A(0));

  if (IsRational(X0))
    {
      bn0 = new_bignum(NULL);
      bn1 = new_bignum(NULL);
      mpz_init(&bn0->mp_int);	/* memory leak */
      mpz_init(&bn1->mp_int);	/* memory leak */
      mpq_get_num(&bn0->mp_int,&Rat(Gen(X0))->mp_rat);
      mpq_get_den(&bn1->mp_int,&Rat(Gen(X0))->mp_rat);
      Deref(X1, A(1));
      Deref(X2, A(2));
      return unify(X1, TagGen(bn0), exs->andb,exs)
	      && unify(X2, TagGen(bn1), exs->andb,exs);
    }
  IfVarSuspend(X0);
  return FALSE;
}



bool akl_rat_add(Arg)
     Argdecl;
{
  RatArithmetic(mpq_add);
}


bool akl_rat_sub(Arg)
    Argdecl;
{
  RatArithmetic(mpq_sub);
}


bool akl_rat_div(Arg)
    Argdecl;
{
  RatArithmetic(mpq_div);
}


bool akl_rat_mul(Arg)
    Argdecl;
{
  RatArithmetic(mpq_mul);
}

/*
bool akl_rat_mod(Arg)
    Argdecl;
{
  RatArithmetic(mpq_mod);
}
*/
/* could be done with mpz_mul_2exp......

bool akl_rat_shift_up(Arg)
    Argdecl;
{
  RatArithmetic(<<);
}


bool akl_rat_shift_down(Arg)
    Argdecl;
{
  RatArithmetic(>>);
}
*/


bool akl_rat_equal(Arg)
     Argdecl;
{
  RatComparison(==);
}


bool akl_rat_not_equal(Arg)
     Argdecl;
{
  RatComparison(!=);
}


bool akl_rat_less(Arg)
     Argdecl;
{
  RatComparison(<);
}


bool akl_rat_not_less(Arg)
    Argdecl;
{
  RatComparison(>=);
}


bool akl_rat_greater(Arg)
    Argdecl;
{
  RatComparison(>);
}


bool akl_rat_not_greater(Arg)
     Argdecl;
{
  RatComparison(<=);
}


void initialize_rational() {

  define("rat_add",akl_rat_add,3);
  define("rat_sub",akl_rat_sub,3);
  define("rat_mul",akl_rat_mul,3);
  define("rat_div",akl_rat_div,3);
/*  define("big_mod",akl_big_mod,3); */
  define("rat_to_big",akl_rat_to_big,3);
  define("big_to_rat",akl_big_to_rat,3);

  define("rat_equal", akl_rat_equal,2);
  define("rat_not_equal", akl_rat_not_equal,2);
  define("rat_less", akl_rat_less,2);
  define("rat_not_less", akl_rat_not_less,2);  
  define("rat_greater", akl_rat_greater,2);
  define("rat_not_greater", akl_rat_not_greater,2);  
}

#else

void initialize_rational() {}		/* dummy for lazy programmers */

#endif /* RATNUM && !NOBIGNUM */
