/* $Id: compare.c,v 1.21 1994/05/06 13:55:28 secher Exp $ */

#include <errno.h>
#include <string.h>
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
#include "compare.h"

static Term compres_atom[3];
static Functor functor_cons;

/* int, big, rat, flt, atm, fun, arity + lexical */

static bool akl_compare(Arg)
     Argdecl;
{
  register Term X0, X1;
  bool res;
  compres_val compres;

  res = akl_compare_aux(A(1), A(2), exs, &compres);

  if (res == TRUE) {
    Deref(X0, A(0));
    return unify(X0, compres_atom[compres], exs->andb, exs);
  } else if (res == FALSE) {
    Deref(X1, A(1));
    if (!(IsFLT(X1) || IsINT(X1) || IsATM(X1) || IsLST(X1) || IsSTR(X1)))
      DOMAIN_ERROR("compare", 3, 2, "tree")
    else
      DOMAIN_ERROR("compare", 3, 3, "tree")
  } else
    return res;
}

bool compare_functors(X0f,X1f,exs,compres)
     Functor X0f,X1f;
     exstate *exs;
     compres_val *compres;
{
  bool res;
  int strcmpval;

  if (FnkArity(X0f) == 0) {
    if (FnkArity(X1f) == 0) {
      Term nx,ny;
      functor0 *fx = (functor0*)X0f;
      functor0 *fy = (functor0*)X1f;

      GetArg(nx,&(fx->name));
      GetArg(ny,&(fy->name));
      res = akl_compare_aux(nx, ny, exs, compres);
      if (res != TRUE)
	return res;
      else return TRUE; /*if (*compres != CMP_EQUAL)
	return TRUE; */
    } else {
      	*compres = CMP_LESS;
	return TRUE;
      }
  } else {
    if (FnkArity(X1f) == 0) {
      	*compres = CMP_GREATER;
	return TRUE;
      }
  }
  strcmpval = strcmp(AtmPname(FnkName(X0f)), AtmPname(FnkName(X1f)));
    
  if (strcmpval > 0)
    *compres = CMP_GREATER;
  else
    if (strcmpval < 0 )
      *compres = CMP_LESS;
    else
      if (FnkArity(X0f) < FnkArity(X1f)) 
	*compres = CMP_LESS;
      else 
	if (FnkArity(X0f) > FnkArity(X1f))
	  *compres = CMP_GREATER;
	else
	  if (FnkNums(X0f) < FnkNums(X1f)) 
	    *compres = CMP_GREATER;
	  else 
	    if (FnkNums(X0f) > FnkNums(X1f))
	      *compres = CMP_LESS;
	    else {
	      int i;
	      Term *X1names,*X0names;
	      
	      X0names = FnkNames(X0f);
	      X1names = FnkNames(X1f);
	      
	      for (i=0;i < FnkFeatures(X0f); i++) {
		res = akl_compare_aux(X0names[i],X1names[i], exs, compres);
		if (res != TRUE)
		  return res;
		else if (*compres != CMP_EQUAL)
		  return TRUE;
	      }
	    }
  return TRUE;
}


bool akl_compare_aux(x0, x1, exs, compres)
     Term x0, x1;
     exstate *exs;
     compres_val *compres;
{
  Term X0, X1;
  bool res;

  Deref(X0, x0);
  IfVarSuspend(X0);

  Deref(X1, x1);
  IfVarSuspend(X1);

  if (IsINT(X0)) {
    if (IsINT(X1)) {
      if (IsNUM(X0) && IsNUM(X1)) {
	if (GetSmall(X0) == GetSmall(X1))
	  *compres = CMP_EQUAL;
	else if (GetSmall(X0) < GetSmall(X1))
	  *compres = CMP_LESS;
	else
	  *compres = CMP_GREATER;
      } else {
	int tmp = bignum_compare(X0, X1);
	if (tmp == 0)
	  *compres = CMP_EQUAL;
	else if (tmp < 0)
	  *compres = CMP_LESS;
	else
	  *compres = CMP_GREATER;
      }
    }
    else if (IsFLT(X1))
      *compres = CMP_GREATER;
    else if (IsATM(X1) || IsLST(X1) || IsSTR(X1) || IsFUNC(X1))
      *compres = CMP_LESS;
    else
      return FALSE;
  }
  else if (IsFLT(X0)) {
    if (IsFLT(X1)) {
      if (FltVal(Flt(X0)) == FltVal(Flt(X1)))
	*compres = CMP_EQUAL;
      else if (FltVal(Flt(X0)) < FltVal(Flt(X1)))
	*compres = CMP_LESS;
      else
	*compres = CMP_GREATER;
    }
    else {
      if (IsINT(X1) || IsATM(X1) || IsLST(X1) || IsSTR(X1) || IsFUNC(X1))
	*compres = CMP_LESS;
      else
	return FALSE;
    }
  }
  else if (IsATM(X0)) {
    if (IsATM(X1)) {
      if (Eq(X0,X1))
	*compres = CMP_EQUAL;
      else {
	int strcmpval = strcmp(AtmPname(Atm(X0)), AtmPname(Atm(X1)));
	if (strcmpval > 0)
	  *compres = CMP_GREATER;
	else
	  *compres = CMP_LESS;
      }
    }
    else {
      if (IsINT(X1) || IsFLT(X1))
	*compres = CMP_GREATER;
      else if (IsLST(X1) || IsSTR(X1) || IsFUNC(X1))
	*compres = CMP_LESS;
      else
	return FALSE;
    }
  }
  else if (IsLST(X0) || IsSTR(X0)) {
    if (IsLST(X0) && IsLST(X1)) {
      Term temp0, temp1;
      GetLstCar(temp0,Lst(X0));
      GetLstCar(temp1,Lst(X1));
      res = akl_compare_aux(temp0, temp1, exs, compres);
      if (res != TRUE)
	return res;
      else if (*compres == CMP_EQUAL) {
	GetLstCdr(temp0,Lst(X0));
	GetLstCdr(temp1,Lst(X1));
	return akl_compare_aux(temp0, temp1, exs, compres);
      }
      else
	return TRUE;
    }
    else if (IsSTR(X0) && IsSTR(X1)) {
      if (StrFunctor(Str(X0)) == StrFunctor(Str(X1))) {
	int i;
	Term temp0, temp1;

	for (i = 0; i < StrArity(Str(X0)); i++) {
	  GetStrArg(temp0,Str(X0),i);
	  GetStrArg(temp1,Str(X1),i);
	  res = akl_compare_aux(temp0, temp1, exs, compres);
	  if (res != TRUE)
	    return res;
	  else if (*compres != CMP_EQUAL)
	    return TRUE;
	}
	return TRUE;
      }
      else {
	Functor X0f = StrFunctor(Str(X0));
	Functor X1f = StrFunctor(Str(X1));
	res = compare_functors(X0f,X1f,exs,compres);
	if (res != TRUE)
	  return res;
	else if (*compres != CMP_EQUAL)
	  return TRUE;
	return TRUE;
      }
    }
    else if (IsLST(X1) || IsSTR(X1)) {
      char *name0, *name1;
      int arity0, arity1;
      int cmp;

      if (IsLST(X0)) {
	name0 = ".";
	arity0 = 2;
      }
      else {
	name0 = AtmPname(StrName(Str(X0)));
	arity0 = StrArity(Str(X0));
      }

      if (IsLST(X1)) {
	name1 = ".";
	arity1 = 2;
      }
      else {
	name1 = AtmPname(StrName(Str(X1)));
	arity1 = StrArity(Str(X1));
      }

      if ((cmp = strcmp(name0, name1)) == 0) {
	if (arity0 < arity1)
	  *compres = CMP_LESS;
	else
	  *compres = CMP_GREATER;
      }
      else if (cmp < 0)
	*compres = CMP_LESS;
      else
	*compres = CMP_GREATER;
    }
    else 
      if (IsINT(X1) || IsFLT(X1) || IsATM(X1))
	*compres = CMP_GREATER;
      else
	if (IsFUNC(X1))
	  *compres = CMP_LESS;
	else
	  return FALSE;
  }
  else
    if (IsFUNC(X0)) 
      if (IsFUNC(X1)) {
	Functor X0f = Fnk(X0);
	Functor X1f = Fnk(X1);
	res = compare_functors(X0f,X1f,exs,compres);
	if (res != TRUE)
	  return res;
	else if (*compres != CMP_EQUAL)
	  return TRUE;
	return TRUE;
      } 
      else
      *compres = CMP_GREATER;
    else
      return FALSE;
  return TRUE;
}

#define CompareTerm(Name, Test, UserName) \
bool Name(Arg) \
     Argdecl; \
{ \
  register Term X0; \
  bool res; \
  compres_val compres; \
 \
  res = akl_compare_aux(A(0), A(1), exs, &compres); \
  if(res == TRUE) { \
    if(Test) return TRUE; \
    else return FALSE; \
  } else if (res == FALSE) { \
    Deref(X0, A(0)); \
    if (!(IsFLT(X0) || IsINT(X0) || IsATM(X0) || IsLST(X0) || IsSTR(X0))) \
      DOMAIN_ERROR(UserName, 2, 1, "tree") \
    else \
      DOMAIN_ERROR(UserName, 2, 2, "tree") \
  } else \
    return res; \
}
    
CompareTerm(akl_term_equal, compres == CMP_EQUAL, "==")
CompareTerm(akl_term_not_equal, compres != CMP_EQUAL, "\\==")
CompareTerm(akl_term_less, compres == CMP_LESS, "@<")
CompareTerm(akl_term_less_equal, compres != CMP_GREATER, "@=<")
CompareTerm(akl_term_greater, compres == CMP_GREATER, "@>")
CompareTerm(akl_term_greater_equal, compres != CMP_LESS, "@>=")

void initialize_compare() {

  functor_cons = store_functor(store_atom("."),2);

  compres_atom[CMP_LESS] = TagAtm(store_atom("<"));
  compres_atom[CMP_EQUAL] = TagAtm(store_atom("="));
  compres_atom[CMP_GREATER] = TagAtm(store_atom(">"));

  define("compare", akl_compare, 3);

  define("@<", akl_term_less, 2);
  define("@=<", akl_term_less_equal, 2);
  define("@>", akl_term_greater, 2);
  define("@>=", akl_term_greater_equal, 2);
  define("==", akl_term_equal, 2);
  define("\\==", akl_term_not_equal, 2);
}
