/* $Id: is.c,v 1.15 1994/05/02 10:46:45 rch Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "initial.h"
#include "storage.h"
#include "config.h"
#include "names.h"
#include "error.h"
#include "bignum.h"
#include <math.h>

Functor functor_add;
Functor functor_sub;
Functor functor_mul;
Functor functor_div;
Functor functor_idiv;
Functor functor_mod;
Functor functor_min;
Functor functor_max;
Functor functor_and;
Functor functor_or;
Functor functor_xor;
Functor functor_shl;
Functor functor_shr;
Functor functor_neg;
Functor functor_plus;
Functor functor_integer;
Functor functor_com;
Functor functor_float;

Functor functor_acos;
Functor functor_asin;
Functor functor_atan;
Functor functor_atan2;
Functor functor_acosh;
Functor functor_asinh;
Functor functor_atanh;
Functor functor_ceil;
Functor functor_cos;
Functor functor_cosh;
Functor functor_exp;
Functor functor_fabs;
Functor functor_floor;
Functor functor_log;
Functor functor_log10;
Functor functor_pow;
Functor functor_sin;
Functor functor_sinh;
Functor functor_sqrt;
Functor functor_tan;
Functor functor_tanh;


#define BinaryEvalCast(Name, Expr, Bignumfunc) \
void Name(tmp1, tmp2, val) \
     Term tmp1, tmp2; \
     Term *val; \
{ \
  if (IsNUM(tmp1) && IsNUM(tmp2)) { \
    sword a, b; \
    a = GetSmall(tmp1); \
    b = GetSmall(tmp2); \
    MakeIntegerTerm(*val, (Expr)); \
    return; \
  } \
 \
  if (IsFLT(tmp1) || IsFLT(tmp2)) { \
    double a, b; \
    a = get_float(tmp1); \
    b = get_float(tmp2); \
    MakeFloatTerm(*val, (Expr)); \
    return; \
  } \
 \
  *val = Bignumfunc(tmp1, tmp2); \
  return; \
}

#define BinaryEvalInt(Name, Expr, Bignumfunc) \
void Name(tmp1, tmp2, val) \
     Term tmp1, tmp2; \
     Term *val; \
{ \
 start: \
  if (IsNUM(tmp1) && IsNUM(tmp2)) { \
    sword a, b; \
    a = GetSmall(tmp1); \
    b = GetSmall(tmp2); \
    MakeIntegerTerm(*val, (Expr)); \
    return; \
  } \
 \
  if (IsFLT(tmp1)) { \
    tmp1 = integer_from_float(tmp1); \
    goto start; \
  } \
 \
  if (IsFLT(tmp2)) { \
    tmp2 = integer_from_float(tmp2); \
    goto start; \
  } \
 \
  *val = Bignumfunc(tmp1, tmp2); \
  return; \
}

#define BinaryEvalFlt(Name, Expr) \
void Name(tmp1, tmp2, val) \
     Term tmp1, tmp2; \
     Term *val; \
{ \
  double a, b; \
 \
  a = get_float(tmp1); \
  b = get_float(tmp2); \
  MakeFloatTerm(*val, (Expr)); \
  return; \
}

#define UnaryEvalCast(Name, Expr, Bignumfunc) \
void Name(tmp1, val) \
     Term tmp1; \
     Term *val; \
{ \
  double a; \
 \
  if (IsNUM(tmp1)) { \
    sword a; \
    a = GetSmall(tmp1); \
    MakeIntegerTerm(*val, (Expr)); \
    return; \
  } \
 \
  if (IsFLT(tmp1)) { \
    a = FltVal(Flt(tmp1)); \
    MakeFloatTerm(*val, (Expr)); \
    return; \
  } \
 \
  *val = Bignumfunc(tmp1); \
  return; \
}

#define UnaryEvalInt(Name, Expr, Bignumfunc) \
void Name(tmp1, val) \
     Term tmp1; \
     Term *val; \
{ \
 start: \
  if (IsNUM(tmp1)) { \
    sword a; \
    a = GetSmall(tmp1); \
    MakeIntegerTerm(*val, (Expr)); \
    return; \
  } \
 \
  if (IsFLT(tmp1)) { \
    tmp1 = integer_from_float(tmp1); \
    goto start; \
  } \
   \
  *val = Bignumfunc(tmp1); \
  return; \
}

#define UnaryEvalFlt(Name, Expr) \
void Name(tmp1, val) \
     Term tmp1; \
     Term *val; \
{ \
  double a; \
 \
  a = get_float(tmp1); \
  MakeFloatTerm(*val, (Expr)); \
  return; \
}

#define BinaryEvalTest(Name, Test, UserName) \
bool Name(Arg) \
	Argdecl;\
{ \
  Term res, tmp1, tmp2;\
 \
  res = akl_eval(A(0), exs, &tmp1);\
  if (res != TRUE) { \
    if (res == FALSE) \
      DOMAIN_ERROR(UserName, 2, 1, "arithmetic expression") \
    else \
      return res; \
  } \
  res = akl_eval(A(1), exs, &tmp2);\
  if (res != TRUE) { \
    if (res == FALSE) \
      DOMAIN_ERROR(UserName, 2, 2, "arithmetic expression") \
    else \
      return res; \
  } \
 \
  if (IsNUM(tmp1) && IsNUM(tmp2)) { \
    sword a, b; \
    a = GetSmall(tmp1); \
    b = GetSmall(tmp2); \
    if (Test) return TRUE; \
    else return FALSE; \
  } \
 \
  if (IsFLT(tmp1) || IsFLT(tmp2)) { \
    double a, b; \
    a = get_float(tmp1); \
    b = get_float(tmp2); \
    if (Test) return TRUE; \
    else return FALSE; \
  } \
 \
  { \
    int a, b; \
    a = bignum_compare(tmp1, tmp2); \
    b = 0; \
    if (Test) return TRUE; \
    else return FALSE; \
  } \
}

#define BinaryTestBuiltin(Name, Test, UserName) \
bool Name(Arg) \
	Argdecl;\
{ \
  register Term X0, X1; \
 \
  Deref(X0, A(0)); \
  IfVarSuspend(X0); \
 \
  Deref(X1, A(1)); \
  IfVarSuspend(X1); \
 \
  if (IsNUM(X0) && IsNUM(X1)) { \
    sword a, b; \
    a = GetSmall(X0); \
    b = GetSmall(X1); \
    if (Test) return TRUE; \
    else return FALSE; \
  } \
 \
  if (IsFLT(X0) || IsFLT(X1)) { \
    double a, b; \
    a = get_float(X0); \
    b = get_float(X1); \
    if (Test) return TRUE; \
    else return FALSE; \
  } \
 \
  if (!(IsBIG(X0) || IsNUM(X0))) \
    DOMAIN_ERROR(UserName, 2, 1, "number") \
  if (!(IsBIG(X1) || IsNUM(X1))) \
    DOMAIN_ERROR(UserName, 2, 2, "number") \
  { \
    int a, b; \
    a = bignum_compare(X0, X1); \
    b = 0; \
    if (Test) return TRUE; \
    else return FALSE; \
  } \
}

/* eval_mul has to be specially coded to take care of overflow */

void eval_mul(tmp1, tmp2, val)
     Term tmp1, tmp2;
     Term *val;
{
  if (IsNUM(tmp1) && IsNUM(tmp2)) {
    sword a, b, a_abs, b_abs;
    a = GetSmall(tmp1);
    b = GetSmall(tmp2);
    a_abs = (a >= 0 ? a : -a);
    b_abs = (b >= 0 ? b : -b);
    if ((a_abs >> ((TADBITS-1)/2)) == 0 && (b_abs >> ((TADBITS-1)/2)) == 0) {
      MakeIntegerTerm(*val, a*b);
      return;
    } else {
      *val = hard_mul_int(a, b);
      return;
    }
  }

  if (IsFLT(tmp1) || IsFLT(tmp2)) {
    double a, b;
    a = get_float(tmp1);
    b = get_float(tmp2);
    MakeFloatTerm(*val, a*b);
    return;
  }

  *val = bignum_mul(tmp1, tmp2);
  return;
}


/* eval_idiv has to be specially coded in order to detect division by zero */

void eval_idiv(tmp1, tmp2, val)
     Term tmp1, tmp2;
     Term *val;
{
  Term zero = MakeSmallNum(0);

 start:
  if (Eq(tmp2, zero)) {
    *val = NullTerm;
    return;
  }

  if (IsNUM(tmp1) && IsNUM(tmp2)) {
    sword a, b;
    a = GetSmall(tmp1);
    b = GetSmall(tmp2);
    MakeIntegerTerm(*val, a/b);
    return;
  }

  if (IsFLT(tmp1)) {
    tmp1 = integer_from_float(tmp1);
    goto start;
  }

  if (IsFLT(tmp2)) {
    tmp2 = integer_from_float(tmp2);
    goto start;
  }

  *val = bignum_div(tmp1, tmp2);
  return;
}


/* eval_shl has to be specially coded to take care of overflow */

void eval_shl(tmp1, tmp2, val)
     Term tmp1, tmp2;
     Term *val;
{
 start:
  if (IsNUM(tmp1) && IsNUM(tmp2)) {
    sword a, b, a_abs, b_abs;
    a = GetSmall(tmp1);
    b = GetSmall(tmp2);
    a_abs = (a >= 0 ? a : -a);
    b_abs = (b >= 0 ? b : -b);
    if ((a_abs >> ((TADBITS-1)/2)) != 0 || b > ((TADBITS-1)/2)) {
      *val = hard_shift_int(a, b, 1);
      return;
    } else {
      a = (b>=0 ? a<<b_abs : a>>b_abs);
      MakeIntegerTerm(*val, a);
      return;
    }
  }

  if (IsFLT(tmp1)) {
    tmp1 = integer_from_float(tmp1);
    goto start;
  }

  if (IsFLT(tmp2)) {
    tmp2 = integer_from_float(tmp2);
    goto start;
  }

  *val = bignum_shift(tmp1, tmp2, 1);
  return;
}


/* eval_shr has to be specially coded to take care of overflow */

void eval_shr(tmp1, tmp2, val)
     Term tmp1, tmp2;
     Term *val;
{
 start:
  if (IsNUM(tmp1) && IsNUM(tmp2)) {
    sword a, b, a_abs, b_abs;
    a = GetSmall(tmp1);
    b = GetSmall(tmp2);
    a_abs = (a >= 0 ? a : -a);
    b_abs = (b >= 0 ? b : -b);
    if ((a_abs >> ((TADBITS-1)/2)) != 0 && -b > ((TADBITS-1)/2)) {
      *val = hard_shift_int(a, b, -1);
      return;
    } else {
      a = (b<=0 ? a<<b_abs : a>>b_abs);
      MakeIntegerTerm(*val, a);
      return;
    }
  }

  if (IsFLT(tmp1)) {
    tmp1 = integer_from_float(tmp1);
    goto start;
  }

  if (IsFLT(tmp2)) {
    tmp2 = integer_from_float(tmp2);
    goto start;
  }

  *val = bignum_shift(tmp1, tmp2, -1);
  return;
}


BinaryEvalCast(eval_add, a+b, bignum_add)
BinaryEvalCast(eval_sub, a-b, bignum_sub)
BinaryEvalCast(eval_min, (a < b) ? a : b, bignum_min)
BinaryEvalCast(eval_max, (a > b) ? a : b, bignum_max)
/* eval_mul was defined earlier */
BinaryEvalFlt(eval_div, a/b)     

/* eval_idiv was defined earlier */
BinaryEvalInt(eval_mod, a%b, bignum_mod)
BinaryEvalInt(eval_and, a&b, bignum_and)
BinaryEvalInt(eval_or, a|b, bignum_or)
BinaryEvalInt(eval_xor, a^b, bignum_xor)

/* eval_shl was defined earlier */
/* eval_shr was defined earlier */

BinaryEvalFlt(eval_atan2, atan2(a,b))
BinaryEvalFlt(eval_pow, pow(a,b))

UnaryEvalCast(eval_neg, -a, bignum_neg)
UnaryEvalCast(eval_plus, a, bignum_identity)
UnaryEvalCast(eval_inc, a+1, bignum_inc)
UnaryEvalCast(eval_dec, a-1, bignum_dec)

UnaryEvalInt(eval_integer, a, bignum_identity)
UnaryEvalInt(eval_com, ~a, bignum_com)

UnaryEvalFlt(eval_float, a)
UnaryEvalFlt(eval_acos, acos(a))
UnaryEvalFlt(eval_asin, asin(a))
UnaryEvalFlt(eval_atan, atan(a))
#ifndef NOASINH
UnaryEvalFlt(eval_acosh, acosh(a))
UnaryEvalFlt(eval_asinh, asinh(a))
UnaryEvalFlt(eval_atanh, atanh(a))
#endif
UnaryEvalFlt(eval_ceil, ceil(a))
UnaryEvalFlt(eval_cos, cos(a))
UnaryEvalFlt(eval_cosh, cosh(a))
UnaryEvalFlt(eval_exp, exp(a))
UnaryEvalFlt(eval_fabs, fabs(a))
UnaryEvalFlt(eval_floor, floor(a))
UnaryEvalFlt(eval_log, log(a))
UnaryEvalFlt(eval_log10, log10(a))
UnaryEvalFlt(eval_sin, sin(a))
UnaryEvalFlt(eval_sinh, sinh(a))
UnaryEvalFlt(eval_sqrt, sqrt(a))
UnaryEvalFlt(eval_tan, tan(a))
UnaryEvalFlt(eval_tanh, tanh(a))

#define BinaryEvalCase(Functor, Name) \
  if (op == Functor) { \
    Name(tmp1, tmp2, val); \
    if (!Eq(*val, NullTerm)) \
      return TRUE; \
  }

#define UnaryEvalCase(Functor, Name) \
  if (op == Functor) { \
    Name(tmp1, val); \
    if (!Eq(*val, NullTerm)) \
      return TRUE; \
  }

bool akl_eval(expr0, exs, val)
     Term expr0;
     exstate *exs;
     Term *val;
{
  Term expr, arg1, arg2, tmp1, tmp2;
  bool res;
  Functor op;
  int arity;

  Deref(expr, expr0);
  IfVarSuspend(expr);

  if (IsINT(expr) || IsFLT(expr)) {
    *val = expr;
    return TRUE;
  }

  if (!(IsSTR(expr)))
    return FALSE;

  op = StrFunctor(Str(expr));
  arity = StrArity(Str(expr));

  if (arity == 2) {
    GetStrArg(arg1, Str(expr), 0);
    GetStrArg(arg2, Str(expr), 1);
    res = akl_eval(arg1, exs, &tmp1);
    if (res != TRUE)
      return res;
    res = akl_eval(arg2, exs, &tmp2);
    if (res != TRUE)
      return res;

    BinaryEvalCase(functor_add, eval_add);
    BinaryEvalCase(functor_sub, eval_sub)
    BinaryEvalCase(functor_mul, eval_mul)
    BinaryEvalCase(functor_div, eval_div)
    BinaryEvalCase(functor_idiv, eval_idiv)
    BinaryEvalCase(functor_mod, eval_mod)
    BinaryEvalCase(functor_min, eval_min)
    BinaryEvalCase(functor_max, eval_max)
    BinaryEvalCase(functor_and, eval_and)
    BinaryEvalCase(functor_or, eval_or)
    BinaryEvalCase(functor_xor, eval_xor)
    BinaryEvalCase(functor_shl, eval_shl)
    BinaryEvalCase(functor_shr, eval_shr)
    BinaryEvalCase(functor_atan2, eval_atan2)
    BinaryEvalCase(functor_pow, eval_pow)

  }
  else if (arity == 1) {
    GetStrArg(arg1, Str(expr), 0);
    res = akl_eval(arg1, exs, &tmp1);
    if (res != TRUE)
      return res;

    UnaryEvalCase(functor_neg, eval_neg)
    UnaryEvalCase(functor_plus, eval_plus)
    UnaryEvalCase(functor_integer, eval_integer)
    UnaryEvalCase(functor_com, eval_com)
    UnaryEvalCase(functor_float, eval_float)
    UnaryEvalCase(functor_acos, eval_acos)
    UnaryEvalCase(functor_asin, eval_asin)
    UnaryEvalCase(functor_atan, eval_atan)
#ifndef NOASINH
    UnaryEvalCase(functor_acosh, eval_acosh)
    UnaryEvalCase(functor_asinh, eval_asinh)
    UnaryEvalCase(functor_atanh, eval_atanh)
#endif
#ifdef NOASINH
    if (op == functor_acosh)
      SYSTEM_ERROR("acosh unavailable")
    if (op == functor_asinh)
      SYSTEM_ERROR("asinh unavailable")
    if (op == functor_atanh)
      SYSTEM_ERROR("atanh unavailable")
#endif
    UnaryEvalCase(functor_ceil, eval_ceil)
    UnaryEvalCase(functor_cos, eval_cos)
    UnaryEvalCase(functor_cosh, eval_cosh)
    UnaryEvalCase(functor_exp, eval_exp)
    UnaryEvalCase(functor_fabs, eval_fabs)
    UnaryEvalCase(functor_floor, eval_floor)
    UnaryEvalCase(functor_log, eval_log)
    UnaryEvalCase(functor_log10, eval_log10)
    UnaryEvalCase(functor_sin, eval_sin)
    UnaryEvalCase(functor_sinh, eval_sinh)
    UnaryEvalCase(functor_sqrt, eval_sqrt)
    UnaryEvalCase(functor_tan, eval_tan)
    UnaryEvalCase(functor_tanh, eval_tanh)
  }

  return FALSE;
}

BinaryEvalTest(akl_eval_equal, a==b, "=:=")
BinaryEvalTest(akl_eval_not_equal, a!=b, "=\\=")
BinaryEvalTest(akl_eval_less, a<b, "<")
BinaryEvalTest(akl_eval_greater, a>b, ">")
BinaryEvalTest(akl_eval_less_equal, a<=b, "=<")
BinaryEvalTest(akl_eval_greater_equal, a>=b, ">=")
     
BinaryTestBuiltin(akl_equal, a==b, "equal")
BinaryTestBuiltin(akl_not_equal, a!=b, "not_equal")
BinaryTestBuiltin(akl_less, a<b, "less")
BinaryTestBuiltin(akl_greater, a>b, "greater")
BinaryTestBuiltin(akl_not_greater, a<=b, "not_greater")
BinaryTestBuiltin(akl_not_less, a>=b, "not_less")

bool akl_is(Arg)
     Argdecl;
{
  register Term X0;
  bool res;
  Term val;

  res = akl_eval(A(1), exs, &val);

  if (res == TRUE) {
    Deref(X0, A(0));
    return unify(X0, val, exs->andb, exs);
  }
  else if (res == FALSE)
    DOMAIN_ERROR("is", 2, 2, "arithmetic expression")
  else
    return res;
}

#define BinaryEvalBuiltin(Builtin, Func, UserName) \
bool Builtin(Arg) \
     Argdecl; \
{ \
  register Term X0, X1, X2; \
  Term val; \
  \
  Deref(X0, A(0)); \
  IfVarSuspend(X0); \
  \
  Deref(X1, A(1)); \
  IfVarSuspend(X1); \
  \
  Func(X0, X1, &val); \
  \
  if (!Eq(val, NullTerm)) { \
    Deref(X2, A(2)); \
    return unify(X2, val, exs->andb, exs); \
  } else if (!(IsBIG(X0) || IsFLT(X0) || IsNUM(X0))) \
    DOMAIN_ERROR(UserName, 3, 1, "number") \
  else /* if (!(IsBIG(X1) || IsFLT(X1) || IsNUM(X1))) */ \
    DOMAIN_ERROR(UserName, 3, 2, "number") \
}

#define UnaryEvalBuiltin(Builtin, Func, UserName) \
bool Builtin(Arg) \
     Argdecl; \
{ \
  register Term X0, X1; \
  Term val; \
  \
  Deref(X0, A(0)); \
  IfVarSuspend(X0); \
  \
  Func(X0, &val); \
  \
  if (!Eq(val, NullTerm)) { \
    Deref(X1, A(1)); \
    return unify(X1, val, exs->andb, exs); \
  } else /* if (!(IsBIG(X0) || IsFLT(X0) || IsNUM(X0))) */ \
    DOMAIN_ERROR(UserName, 2, 1, "number") \
}

BinaryEvalBuiltin(akl_add, eval_add, "add")
BinaryEvalBuiltin(akl_sub, eval_sub, "sub")
BinaryEvalBuiltin(akl_mul, eval_mul, "mul")
BinaryEvalBuiltin(akl_div, eval_div, "div")
BinaryEvalBuiltin(akl_idiv, eval_idiv, "idiv")
BinaryEvalBuiltin(akl_mod, eval_mod, "mod")
BinaryEvalBuiltin(akl_min, eval_min, "min")
BinaryEvalBuiltin(akl_max, eval_max, "max")
BinaryEvalBuiltin(akl_and, eval_and, "and")
BinaryEvalBuiltin(akl_or, eval_or, "or")
BinaryEvalBuiltin(akl_xor, eval_xor, "xor")
BinaryEvalBuiltin(akl_shl, eval_shl, "shl")
BinaryEvalBuiltin(akl_shr, eval_shr, "shr")
BinaryEvalBuiltin(akl_atan2, eval_atan2, "atan2")
BinaryEvalBuiltin(akl_pow, eval_pow, "pow")

UnaryEvalBuiltin(akl_neg, eval_neg, "neg")
UnaryEvalBuiltin(akl_plus, eval_plus, "plus")
UnaryEvalBuiltin(akl_integer_case, eval_integer, "integer")
UnaryEvalBuiltin(akl_com, eval_com, "com")
UnaryEvalBuiltin(akl_float_case, eval_float, "float")
UnaryEvalBuiltin(akl_acos, eval_acos, "acos")
UnaryEvalBuiltin(akl_asin, eval_asin, "asin")
UnaryEvalBuiltin(akl_atan, eval_atan, "atan")
#ifndef NOASINH
UnaryEvalBuiltin(akl_acosh, eval_acosh, "acosh")
UnaryEvalBuiltin(akl_asinh, eval_asinh, "asinh")
UnaryEvalBuiltin(akl_atanh, eval_atanh, "atanh")
#endif
#ifdef NOASINH
bool akl_acosh(Arg)
     Argdecl;
{
  SYSTEM_ERROR("acosh unavailable")
}
bool akl_asinh(Arg)
     Argdecl;
{
  SYSTEM_ERROR("asinh unavailable")
}
bool akl_atanh(Arg)
     Argdecl;
{
  SYSTEM_ERROR("atanh unavailable")
}
#endif
UnaryEvalBuiltin(akl_ceil, eval_ceil, "ceil")
UnaryEvalBuiltin(akl_cos, eval_cos, "cos")
UnaryEvalBuiltin(akl_cosh, eval_cosh, "cosh")
UnaryEvalBuiltin(akl_exp, eval_exp, "exp")
UnaryEvalBuiltin(akl_fabs, eval_fabs, "fabs")
UnaryEvalBuiltin(akl_floor, eval_floor, "floor")
UnaryEvalBuiltin(akl_log, eval_log, "log")
UnaryEvalBuiltin(akl_log10, eval_log10, "log10")
UnaryEvalBuiltin(akl_sin, eval_sin, "sin")
UnaryEvalBuiltin(akl_sinh, eval_sinh, "sinh")
UnaryEvalBuiltin(akl_sqrt, eval_sqrt, "sqrt")
UnaryEvalBuiltin(akl_tan, eval_tan, "tan")
UnaryEvalBuiltin(akl_tanh, eval_tanh, "tanh")

UnaryEvalBuiltin(akl_inc, eval_inc, "inc")
UnaryEvalBuiltin(akl_dec, eval_dec, "dec")


void initialize_is() {

  functor_add = store_functor(store_atom("+"),2);
  functor_sub = store_functor(store_atom("-"),2);
  functor_mul = store_functor(store_atom("*"),2);
  functor_div = store_functor(store_atom("/"),2);
  functor_idiv = store_functor(store_atom("//"),2);
  functor_mod = store_functor(store_atom("mod"),2);
  functor_min = store_functor(store_atom("min"),2);
  functor_max = store_functor(store_atom("max"),2);
  functor_and = store_functor(store_atom("/\\"),2);
  functor_or = store_functor(store_atom("\\/"),2);
  functor_xor = store_functor(store_atom("#"),2);
  functor_shl = store_functor(store_atom("<<"),2);
  functor_shr = store_functor(store_atom(">>"),2);
  functor_neg = store_functor(store_atom("-"),1);
  functor_plus = store_functor(store_atom("+"),1);
  functor_integer = store_functor(store_atom("integer"),1);
  functor_com = store_functor(store_atom("\\"),1);
  functor_float = store_functor(store_atom("float"),1);

  functor_acos = store_functor(store_atom("acos"),1);
  functor_asin = store_functor(store_atom("asin"),1);
  functor_atan = store_functor(store_atom("atan"),1);
  functor_atan2 = store_functor(store_atom("atan2"),2);
  functor_acosh = store_functor(store_atom("acosh"),1);
  functor_asinh = store_functor(store_atom("asinh"),1);
  functor_atanh = store_functor(store_atom("atanh"),1);
  functor_ceil = store_functor(store_atom("ceil"),1);
  functor_cos = store_functor(store_atom("cos"),1);
  functor_cosh = store_functor(store_atom("cosh"),1);
  functor_exp = store_functor(store_atom("exp"),1);
  functor_fabs = store_functor(store_atom("fabs"),1);
  functor_floor = store_functor(store_atom("floor"),1);
  functor_log = store_functor(store_atom("log"),1);
  functor_log10 = store_functor(store_atom("log10"),1);
  functor_pow = store_functor(store_atom("pow"),2);
  functor_sin = store_functor(store_atom("sin"),1);
  functor_sinh = store_functor(store_atom("sinh"),1);
  functor_sqrt = store_functor(store_atom("sqrt"),1);
  functor_tan = store_functor(store_atom("tan"),1);
  functor_tanh = store_functor(store_atom("tanh"),1);

  define("is", akl_is, 2);

  define("=:=", akl_eval_equal, 2);
  define("=\\=", akl_eval_not_equal, 2);
  define("<", akl_eval_less, 2);
  define(">", akl_eval_greater, 2);
  define("=<", akl_eval_less_equal, 2);
  define(">=", akl_eval_greater_equal, 2);

  define("add", akl_add, 3);
  define("sub", akl_sub, 3);
  define("mul", akl_mul, 3);
  define("div", akl_div, 3);
  define("idiv", akl_idiv, 3);
  define("mod", akl_mod, 3);
  define("min", akl_min, 3);
  define("max", akl_max, 3);
  define("and", akl_and, 3);
  define("or", akl_or, 3);
  define("xor", akl_xor, 3);
  define("shl", akl_shl, 3);
  define("shr", akl_shr, 3);

  define("neg", akl_neg, 2);
  define("plus", akl_plus, 2);
  define("integer", akl_integer_case, 2);
  define("com", akl_com, 2);
  define("float", akl_float_case, 2);
  define("acos", akl_acos, 2);
  define("asin", akl_asin, 2);
  define("atan", akl_atan, 2);
  define("atan2", akl_atan2, 3);
  define("acosh", akl_acosh, 2);
  define("asinh", akl_asinh, 2);
  define("atanh", akl_atanh, 2);
  define("ceil", akl_ceil, 2);
  define("cos", akl_cos, 2);
  define("cosh", akl_cosh, 2);
  define("exp", akl_exp, 2);
  define("fabs", akl_fabs, 2);
  define("floor", akl_floor, 2);
  define("log", akl_log, 2);
  define("log10", akl_log10, 2);
  define("pow", akl_pow, 3);
  define("sin", akl_sin, 2);
  define("sinh", akl_sinh, 2);
  define("sqrt", akl_sqrt, 2);
  define("tan", akl_tan, 2);
  define("tanh", akl_tanh, 2);

  define("inc", akl_inc, 2);
  define("dec", akl_dec, 2);

  define("equal", akl_equal, 2);
  define("not_equal", akl_not_equal, 2);
  define("less", akl_less, 2);
  define("greater", akl_greater, 2);
  define("not_greater", akl_not_greater, 2);
  define("not_less", akl_not_less, 2);

}
