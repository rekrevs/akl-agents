/* $Id: config.c,v 1.18 1994/05/05 09:36:23 secher Exp $ */

#include <string.h>
#include <errno.h>
#include "include.h"
#include "term.h"
#include "storage.h"
#include "error.h"

/* The function tp is meant for debugging.
 * In GDB, type "print *tp(x)" to see the parts of the tagged pointer x.
 */
static struct debug_term { tval tag; void *address; } termbuf;

struct debug_term *tp(x)
    Term x;
{
  termbuf.tag = PTagOf(x);
  termbuf.address = (void *) AddressOf(x);
#ifdef OTH
  if (termbuf.tag == OTH) {
    termbuf.tag = STagOf(x);
    termbuf.address = (void *) AddressOth(x);
  }
#endif
  return &termbuf;
}

#ifdef TADTURMS

/* This is used by a few macros (notably TagPtr) when TADTURMS is defined,
 * since a macro can't initialize and return a structure. The only purpose
 * for TADTURMS is for a little easier debugging when your terms, pointers,
 * and unsigned longs get mixed up.
 */

Turm tad_to_turm(A)
    tagged_address A;
{
  Turm temp;
  Tad(temp) = A;
  return temp;
}

#endif


Atom make_atom(str,hashval)
    char *str;
    int hashval;
{
  atom *a;
  char *s;
  register int len;

  NewAtom(a);

  len = strlen(str);
  s = qalloc(sizeof(char) * (len + 1));
  (void) strcpy(s,str);
  
  a->hashval = hashval;
  a->pname = s;
  return a;
}

/*Create a functor with no features */
Functor make_functor(name,arity)
     Atom name;
     int arity;
{
  Functor f;

  NewFunctor(f);

  f->name = name;
  f->arity = arity;
  f->nums = arity;
  f->features = 0;
  f->length = 0;
  return f;
}


Fp make_constant_float(value)
  double value;
{
  Fp i;
  NewConFlt(i);
  i->val = value;
  i->tag2 = FLT_TAG2;
  return i;
}

/* From bignum.c */
extern Bignum	new_bignum PROTO((Bignum old));
extern long	bignum_sizeinbase PROTO((Bignum bn, int base));
extern Term	bignum_atoi PROTO((char *name));
extern void	bignum_itoa PROTO((Bignum bn, int base, char *buf));
extern double	get_bignum_float PROTO((Term x));
extern Term	bignum_from_float PROTO((Term x));
extern Bignum	bignum_copy_to_constspace PROTO((Bignum src));
extern bool	bignum_is_machine_integer PROTO((Bignum bn));
extern bool	bignum_is_machine_unsigned PROTO((Bignum bn));

/* From builtin.c */
extern void	itoa PROTO((long i, int base, char *buf));

Term atoi_term(name)
     char *name;
{
  sword value;

  if (strlen(name) <= (TADBITS-1)/4) {	/* Conservative estimate */
    value = atoi(name);			/* Sun atoi doesn't detect overflow */
    if (IntIsSmall(value))
      return MakeSmallNum(value);
  }

  return bignum_atoi(name);
}

/* This returns a sufficient buffer size for printing the magnitude of i
 * in base b. Space for a minus sign and terminating '\0' is not included.
 */

long integer_sizeinbase(i, b)
     Term i;
     int b;
{
  if (IsNUM(i))
    return SMALLNUMBITS;
  else if (IsBIG(i))
    return bignum_sizeinbase(Big(i), b);
  else
    return 0;
}

void term_itoa(i, base, buf)
     Term i;
     int base;
     char *buf;
{
  if (IsNUM(i))
    itoa(GetSmall(i), base, buf);
  else if (IsBIG(i))
    bignum_itoa(Big(i), base, buf);
  else
    buf[0] = '?';
}

Term integer_copy_to_constspace(x)
     Term x;
{
  if (IsBIG(x))
    return TagBig(bignum_copy_to_constspace(Big(x)));
  else
    return x;	/* It's not on the heap */
}

double get_float(x)
     Term x;
{
  if (IsFLT(x))
    return FltVal(Flt(x));
  else if (IsNUM(x))
    return (double) GetSmall(x);
  else
    return get_bignum_float(x);
}

Term integer_from_float(x)
     Term x;
{
  double f = FltVal(Flt(x));	/* We know that x is a float */

  if (f >= -MaxSmallNum && f < MaxSmallNum)
    return MakeSmallNum((sword)f);
  else
    return bignum_from_float(x);
}

/* Small integers have been taken care of by IsMachineInteger() */
bool is_machine_integer(i)
     Term i;
{
  if (IsBIG(i))
    return bignum_is_machine_integer(Big(i));
  else
    return FALSE;
}


/* Small integers have been taken care of by IsMachineUnsigned() */
bool is_machine_unsigned(i)
     Term i;
{
  if (IsBIG(i))
    return bignum_is_machine_unsigned(Big(i));
  else
    return FALSE;
}

