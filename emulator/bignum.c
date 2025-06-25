/* $Id: bignum.c,v 1.27 1994/04/14 14:53:55 boortz Exp $ */

#ifndef NOBIGNUM

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
#include "builtin.h"
#include "error.h"

#include <limits.h>
#include <math.h>

extern void mpz_xor_kludge();	/* NYI in the gmp library */
extern void mpz_mul_2exp_si();	/* for practical reasons */

struct mp_bignum {
  uword		tag2;
  MP_INT	mp_int;
};
  
extern void *bignum_alloc();
extern void *bignum_realloc();
extern void  bignum_dealloc();

extern void *bignum_const_alloc();
extern void *bignum_const_realloc();
extern void  bignum_const_dealloc();


/* -------------------------------------------------------------------- */

Bignum new_bignum(old)	/* "old" may be NULL */
     Bignum old;	/* ignore it whatever it is... */
{
  Bignum new;

  NEW(new, mp_bignum);
  new->tag2 = BIG_TAG2;

  return new;
}

/* -------------------------------------------------------------------- */

Bignum make_bignum(value)
     sword value;
{
  Bignum new;

  NEW(new, mp_bignum);
  new->tag2 = BIG_TAG2;
  mpz_init_set_si(&new->mp_int, value);

  return new;
}

/* -------------------------------------------------------------------- */

void bignum_gc(old,new,gcst)
     Bignum old, new;
     gcstatep gcst;
{
  mpz_init_set(&new->mp_int, &old->mp_int);
}

/* -------------------------------------------------------------------- */

long bignum_sizeinbase(bn, base)
     Bignum bn;
     int base;
{
  return mpz_sizeinbase(&bn->mp_int, base);
}

/* -------------------------------------------------------------------- */

int bignum_print(file, bn)
     FILE *file;
     Bignum bn;
{
#define FIXED_BUF_SIZE 512
  char *buf;
  int res;
  long size = mpz_sizeinbase(&bn->mp_int,10) + 2;
  char fixed_buf[FIXED_BUF_SIZE];

  if (size > FIXED_BUF_SIZE)
    buf = salloc(size);
  else
    buf = fixed_buf;

  mpz_get_str(buf, 10, &bn->mp_int);
  res = fprintf(file, "%s", buf);

  if (size > FIXED_BUF_SIZE)
    free(buf);			/* should use sfree or something */
  return res;
}

/* -------------------------------------------------------------------- */

Term canonize_bignum(b)
     Bignum b;
{
  if (mpz_cmp_si(&b->mp_int, -MaxSmallNum) >= 0 &&
      mpz_cmp_si(&b->mp_int, MaxSmallNum) < 0) {
    return MakeSmallNum(mpz_get_si(&b->mp_int));
  } else {
    return TagBig(b);
  }
}

/* -------------------------------------------------------------------- */

Term bignum_atoi(name)
     char *name;
{
  Bignum res;
  res = new_bignum(NULL);
  mpz_init_set_str(&res->mp_int, name, 10);
  return canonize_bignum(res);
}

/* -------------------------------------------------------------------- */

void bignum_itoa(bn, base, buf)
     Bignum bn;
     int base;
     char *buf;
{
  mpz_get_str(buf, base, &bn->mp_int);
}

/* -------------------------------------------------------------------- */

bool bignum_is_machine_integer(b)
     Bignum b;
{
  if (mpz_cmp_si(&b->mp_int, LONG_MIN) >= 0 &&
      mpz_cmp_si(&b->mp_int, LONG_MAX) <= 0)
    return TRUE;
  else
    return FALSE;
}

/* -------------------------------------------------------------------- */

bool bignum_is_machine_unsigned(b)
     Bignum b;
{
  if (mpz_cmp_si(&b->mp_int, 0) >= 0 &&
      mpz_cmp_ui(&b->mp_int, ULONG_MAX) <= 0)
    return TRUE;
  else
    return FALSE;
}

/* -------------------------------------------------------------------- */

int bignum_compare(x, y)
     Term x, y;
{
  /* We assume that both x,y are dereferenced, both of them are not small,
   * and neither one is a float.
   */

  if (IsBIG(x)) {
    if (IsNUM(y)) {
      return mpz_cmp_si(&Big(x)->mp_int, GetSmall(y));
    } else if (IsBIG(y)) {
      return mpz_cmp(&Big(x)->mp_int, &Big(y)->mp_int);
    }
  } else if (IsBIG(y) && IsNUM(x)) {
    return -mpz_cmp_si(&Big(y)->mp_int, GetSmall(x));
  }

  Error("non-numeric domain in numeric comparison");
  return -1;	/* Kludge! */
}


/* -------------------------------------------------------------------- */

sword get_bignum_integer(i)
     Term i;
{
  if (IsBIG(i))
    return mpz_get_si(&Big(i)->mp_int);
  else
    {
      Error("get_bignum_integer() applied on non-integer term");
      return 0;
    }
}

/* -------------------------------------------------------------------- */

uword get_bignum_unsigned(i)
     Term i;
{
  if (IsBIG(i))
    return mpz_get_ui(&Big(i)->mp_int);
  else
    {
      Error("get_bignum_unsigned() applied on non-integer term");
      return 0;
    }
}

/* -------------------------------------------------------------------- */

uword bignum_hash_value(i)
     Term i;
{
  MP_INT *bp = &Big(i)->mp_int;
  unsigned int hval;

  hval = mpz_get_ui(bp);

  return hval;
}

/* -------------------------------------------------------------------- */

#define SIGN_BIT	(1+((unsigned long)LONG_MAX))	/* 0x8000000    */
#define F_SIGN_BIT	(1.0+((double)LONG_MAX))	/* 2147483648.0 */
#define F_LIMB_BASE	(2.0*F_SIGN_BIT)		/* 4294967296.0 */

double get_bignum_float(x)
     Term x;
{
  if (IsBIG(x)) {
    Bignum b = Big(x);
    long i, size;
    unsigned long *d;
    double f;

 /* NOTE: Here we use knowledge about the internal MP_INT representation! */

    d = b->mp_int._mp_d;
    size = b->mp_int._mp_size;

    if (size < 0)
      i = -size;
    else
      i = size;

    f = 0.0;

    while (i--) {
      unsigned long u = d[i];

      /* The following is adapted from Sicstus. */
      /* I don't know what the problem is...... */

      if (u & SIGN_BIT)		/* trouble on some machines */
	f = f*F_LIMB_BASE + F_SIGN_BIT + (u - SIGN_BIT);
      else
	f = f*F_LIMB_BASE + u;
    }

    if (size < 0)
      return -f;
    else
      return f;
  } else {
    Error("get_bignum_float() applied on non-integer term");
    return 0.0;
  }
}

/* -------------------------------------------------------------------- */

Term bignum_from_float(x)
     Term x;
{
  char buf[512];
  int foo;
  double f = FltVal(Flt(x));	/* We know that x is a float */

  /* The idea is ripped-off from Sicstus... */

  if (f < 0)
    f = ceil(f);
  else
    f = floor(f);

  sprintf(buf, "%.0f", f);

  foo = buf[1] - '0';

  if (buf[1] != 0 && foo >= 0 && foo <= 9)
    return bignum_atoi(buf);
  else {
    sprintf(buf, "bignum_from_float() failed for %.0f", f);
    Error(buf);
    return MakeSmallNum(0);
  }
}

/* -------------------------------------------------------------------- */

Term hard_mul_int(x, y)
     sword x, y;
{
  Bignum bx;
  Bignum bz;
  bx = make_bignum(y >= 0 ? x : -x);
  bz = new_bignum(NULL);
  mpz_init(&bz->mp_int);
  mpz_mul_ui(&bz->mp_int, &bx->mp_int, (y >= 0 ? y : -y));
  return canonize_bignum(bz);
}

/* -------------------------------------------------------------------- */

Term hard_shift_int(x, y, negate)
     sword x, y;
     int negate;
{
  Bignum bx;
  Bignum bz;
  uword y_abs;
  bx = make_bignum(x);
  bz = new_bignum(NULL);
  mpz_init(&bz->mp_int);
  y_abs = (y >= 0 ? y : -y);
  if ((negate < 0) == (y < 0))
    mpz_mul_2exp(&bz->mp_int, &bx->mp_int, y_abs);
  else
    mpz_div_2exp(&bz->mp_int, &bx->mp_int, y_abs);
  return canonize_bignum(bz);
}

/* -------------------------------------------------------------------- */

void *bignum_alloc(size)
     size_t size;
{
  char *limbs;

  NEWX(limbs,char,size);

  return limbs;
}


/* Note that shrinking will create holes on the heap */

void *bignum_realloc(limbs,size,newsize)
     char *limbs;
     size_t size, newsize;
{
  if (newsize < size)
    return limbs;		/* return old object */

  NEWX(limbs,char,newsize);
  return limbs;
}

void bignum_dealloc(limbs,size)
     void *limbs;
     size_t size;
{
}


/* -------------------------------------------------------------------- */

void *bignum_const_alloc(size)
     size_t size;
{
  char *limbs;
  NEWCONX(limbs,char,size);
  return limbs;
}

/* Dummy realloc and dealloc for constspace */

void *bignum_const_realloc(limbs,size,newsize)
     char *limbs;
     size_t size, newsize;
{
  FatalError("Tried to realloc bignum in constspace");
  return NULL;
}

void bignum_const_dealloc(limbs,size)
     void *limbs;
     size_t size;
{
  FatalError("Tried to dealloc bignum in constspace");
}

Bignum bignum_copy_to_constspace(src)
     Bignum src;
{
  Bignum dest;

  NEWCONX(dest, mp_bignum, 1);
  dest->tag2 = BIG_TAG2;

  mp_set_memory_functions(bignum_const_alloc,
			  bignum_const_realloc,
			  bignum_const_dealloc);

  mpz_init_set(&dest->mp_int, &src->mp_int);

  mp_set_memory_functions(bignum_alloc,
			  bignum_realloc,
			  bignum_dealloc);

  return dest;
}




/*************************************************************************/

Term arithmetic_domain_error()
{
  return NullTerm;
}


/* We assume that both x,y are dereferenced, both of them are not small,
 * and neither one is a float.
 */

#define BigBinaryArithmetic(Name, Mpz_func) \
Term Name(x, y) \
     Term x, y; \
{ \
  Bignum z = new_bignum(NULL); \
  mpz_init(&z->mp_int); \
 \
  if (IsBIG(x)) { \
    if (IsBIG(y)) { \
      Mpz_func(&z->mp_int, &Big(x)->mp_int, &Big(y)->mp_int); \
      return canonize_bignum(z); \
    } else if (IsNUM(y)) { \
      Bignum tmp = make_bignum(GetSmall(y)); \
      Mpz_func(&z->mp_int, &Big(x)->mp_int, &tmp->mp_int); \
      return canonize_bignum(z); \
    } \
  } else if (IsNUM(x) && IsBIG(y)) { \
    Bignum tmp = make_bignum(GetSmall(x)); \
    Mpz_func(&z->mp_int, &tmp->mp_int, &Big(y)->mp_int); \
    return canonize_bignum(z); \
  } \
 \
  return arithmetic_domain_error(); \
}


/* This one is for min and max */

#define BigBinaryArithmeticX(Name, Operator) \
Term Name(x, y) \
     Term x, y; \
{ \
  if (bignum_compare(x, y) Operator 0) \
    return x; \
  else \
    return y; \
}


#define BigUnaryArithmetic(Name, Mpz_func) \
Term Name(x) \
     Term x; \
{ \
  Bignum z; \
 \
  if (IsBIG(x)) { \
    z = new_bignum(NULL); \
    mpz_init(&z->mp_int); \
    Mpz_func(&z->mp_int, &Big(x)->mp_int); \
    return canonize_bignum(z); \
  } \
 \
  return arithmetic_domain_error(); \
}


/* Same as above, except that Mpz_func gets an extra unsigned "1" argument */

#define BigUnaryArithmetic1(Name, Mpz_func) \
Term Name(x) \
     Term x; \
{ \
  Bignum z; \
 \
  if (IsBIG(x)) { \
    z = new_bignum(NULL); \
    mpz_init(&z->mp_int); \
    Mpz_func(&z->mp_int, &Big(x)->mp_int, 1); \
    return canonize_bignum(z); \
  } \
 \
  return arithmetic_domain_error(); \
}


Term bignum_shift(x, y, negate)
     Term x, y;
     int negate;
{
  Bignum z;
  Bignum tmp;
  long yi;

  if (IsBIG(y)) {
    tmp = Big(y);
    if (!bignum_is_machine_integer(tmp)) {
      Error("in arithmetic expression: shift count has too large magnitude");
      return MakeSmallNum(-1);	/* Kludge! */
    }
    yi = mpz_get_si(&Big(y)->mp_int);
    if (negate < 0)
      yi = -yi;
    z = new_bignum(NULL);
    mpz_init(&z->mp_int);
    if (IsBIG(x)) {
      mpz_mul_2exp_si(&z->mp_int, &Big(x)->mp_int, yi);
      return canonize_bignum(z);
    } else if (IsNUM(x)) {
      tmp = make_bignum(GetSmall(x));
      mpz_mul_2exp_si(&z->mp_int, &tmp->mp_int, yi);
      return canonize_bignum(z);
    }
  } else if (IsNUM(y) && IsBIG(x)) {
    z = new_bignum(NULL);
    mpz_init(&z->mp_int);
    yi = GetSmall(y);
    if (negate < 0)
      yi = -yi;
    mpz_mul_2exp_si(&z->mp_int, &Big(x)->mp_int, yi);
    return canonize_bignum(z);
  }

  return arithmetic_domain_error();
}


Term bignum_identity(x)
     Term x;
{
  if (IsBIG(x))
    return x;
  else
    return arithmetic_domain_error();
}


BigBinaryArithmetic(bignum_add, mpz_add)
BigBinaryArithmetic(bignum_sub, mpz_sub)
BigBinaryArithmetic(bignum_mul, mpz_mul)
BigBinaryArithmetic(bignum_div, mpz_div)
BigBinaryArithmetic(bignum_mod, mpz_mod)
BigBinaryArithmetic(bignum_and, mpz_and)
BigBinaryArithmetic(bignum_or,  mpz_ior)
BigBinaryArithmetic(bignum_xor, mpz_xor_kludge)

BigBinaryArithmeticX(bignum_min, <)
BigBinaryArithmeticX(bignum_max, >)

BigUnaryArithmetic(bignum_neg, mpz_neg)
BigUnaryArithmetic(bignum_com, mpz_com)

BigUnaryArithmetic1(bignum_inc, mpz_add_ui)
BigUnaryArithmetic1(bignum_dec, mpz_sub_ui)


void mpz_mul_2exp_si(res, x, i)
     MP_INT *res, *x;
     long i;
{
  unsigned long u;
  if (i >= 0) {
    u = i;
    mpz_mul_2exp(res, x, u);
  } else {
    u = -i;
    mpz_div_2exp(res, x, u);
  }
}


void mpz_xor_kludge(res, x, y)
     MP_INT *res, *x, *y;
{
  /* SUPER-KLUDGE FOLLOWS: */

  MP_INT t1, t2, t3;

  mpz_init(&t1);
  mpz_init(&t2);
  mpz_init(&t3);
  mpz_com(&t1, x);		/*    ~x     */
  mpz_and(&t2, &t1, y);		/*   ~x&y    */
  mpz_com(&t1, y);		/*    ~y     */
  mpz_and(&t3, &t1, x);		/*   ~y&x    */
  mpz_ior(res, &t2, &t3);	/* ~x&y|~y&x */
  mpz_clear(&t1);
  mpz_clear(&t2);
  mpz_clear(&t3);
}


void initialize_bignum() {

  mp_set_memory_functions(bignum_alloc,bignum_realloc,bignum_dealloc);

}

#else

/* No bignums. Define fake stubs to make the whole thing link anyway */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "gc.h"
#include "copy.h"
#include "storage.h"
#include "config.h"
#include "initial.h"
#include "bignum.h"
#include "builtin.h"
#include "error.h"

#include <limits.h>
#include <math.h>


Bignum new_bignum(old)
     Bignum old;	/* ignore */
{
  Bignum new;

  NEW(new, bigstub);
  new->tag2 = BIG_TAG2;

  return new;
}

Bignum make_bignum(value)
     sword value;	/* ignore */
{
  return new_bignum(NULL);
}

void bignum_gc(old,new,gcst)
     Bignum old, new;
     gcstatep gcst;
{
}

long bignum_sizeinbase(bn, base)
     Bignum bn;
     int base;
{
  return TADBITS;
}

int bignum_print(file, bn)
     FILE *file;
     Bignum bn;
{
  return fprintf(file, "Overflow");
}

Term bignum_atoi(name)
     char *name;
{
  return TagBig(new_bignum(NULL));
}

void bignum_itoa(bn, base, buf)
     Bignum bn;
     int base;
     char *buf;
{
  sprintf(buf, "Overflow");
}

bool bignum_is_machine_integer(b)
     Bignum b;
{
  return FALSE;
}

bool bignum_is_machine_unsigned(b)
     Bignum b;
{
  return FALSE;
}

int bignum_compare(x, y)
     Term x, y;
{
  return -1;	/* Kludge! */
}

Term canonize_bignum(b)
     Bignum b;
{
  return TagBig(b);
}

sword get_bignum_integer(i)
     Term i;
{
  return MaxSmallNum;
}

uword get_bignum_unsigned(i)
     Term i;
{
  return MaxSmallNum;
}

uword bignum_hash_value(i)
     Term i;
{
  return MaxSmallNum;
}

double get_bignum_float(x)
     Term x;
{
  return (double)MaxSmallNum;
}

Term bignum_from_float(x)
     Term x;
{
  return TagBig(new_bignum(NULL));
}

/* Note: This is NOT a dummy! It tries to handle some non-overflow cases. */

Term hard_mul_int(x, y)
     sword x, y;
{
  sword z = x*y;
  if (IntIsSmall(z))
    return MakeSmallNum(z);
  else
    return TagBig(new_bignum(NULL));
}

/* Note: This is NOT a dummy! It tries to handle some non-overflow cases. */

Term hard_shift_int(x, y, negate)
     sword x, y;
     int negate;
{
  sword z;
  if (negate < 0)
    z = (x << -y);
  else
    z = (x << y);
  if (IntIsSmall(z))
    return MakeSmallNum(z);
  else
    return TagBig(new_bignum(NULL));
}

Bignum bignum_copy_to_constspace(src)
     Bignum src;
{
  Bignum dest;

  NEWCONX(dest, bigstub, 1);
  dest->tag2 = BIG_TAG2;
  return dest;
}

#define DummyBinaryArithmetic(Name) \
Term Name(x, y) \
     Term x, y; \
{ \
  return canonize_bignum(new_bignum(NULL)); \
}

#define DummyUnaryArithmetic(Name) \
Term Name(x) \
     Term x; \
{ \
  return canonize_bignum(new_bignum(NULL)); \
}

Term bignum_shift(x, y, negate)
     Term x, y;
     int negate;
{
  return canonize_bignum(new_bignum(NULL));
}

DummyBinaryArithmetic(bignum_add)
DummyBinaryArithmetic(bignum_sub)
DummyBinaryArithmetic(bignum_mul)
DummyBinaryArithmetic(bignum_div)
DummyBinaryArithmetic(bignum_mod)
DummyBinaryArithmetic(bignum_and)
DummyBinaryArithmetic(bignum_or)
DummyBinaryArithmetic(bignum_xor)
DummyBinaryArithmetic(bignum_min)
DummyBinaryArithmetic(bignum_max)

DummyUnaryArithmetic(bignum_identity)
DummyUnaryArithmetic(bignum_neg)
DummyUnaryArithmetic(bignum_com)
DummyUnaryArithmetic(bignum_inc)
DummyUnaryArithmetic(bignum_dec)

void initialize_bignum() {}

#endif /* NOBIGNUM */
