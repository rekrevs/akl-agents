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
#include "builtin.h"
#include "display.h"
#include "abstraction.h"
#include "functor.h"
#include "compare.h"

void ftoa PROTO((double,char *));
void itoa PROTO((long,int,char *));
static bool list_to_cstring PROTO((Argproto,Term,char *,int));

extern char *ecvt PROTO((double, int, int *, int *));

Term term_list;

/************************************************************************/
/***************    					*****************/
/***************    	     TERM TESTS			*****************/
/***************    					*****************/
/************************************************************************/


bool akl_var_descriptor(Arg)
     Argdecl;
{
  register Term v, t, d;

  Deref(v, A(0));
  MakeIntegerTerm(t, variable_descriptor(v)); /* Loses bits... */

  Deref(d, A(1));
  return unify(d, t, exs->andb, exs);
}

/* -------------------------------------------------------------------- */

bool akl_var(Arg)
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  if(IsVar(X0)) return TRUE;
  else return FALSE;
}

/* -------------------------------------------------------------------- */

bool akl_eq(Arg)
    Argdecl;
{
  register Term X0, X1;

  Deref(X0, A(0));
  Deref(X1, A(1));
  if(X0 == X1) return TRUE;
  else return FALSE;
}

/* -------------------------------------------------------------------- */

bool akl_data(Arg)		/* MAKE WAM INSTRUCTION /KB */
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  if(!IsCvaTerm(X0))
    IfVarSuspend(X0);
  return TRUE;
}

/* -------------------------------------------------------------------- */

bool akl_atom(Arg)		/* MAKE WAM INSTRUCTION /KB */
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsATM(X0);
}

/* -------------------------------------------------------------------- */

bool akl_integer(Arg)		/* MAKE WAM INSTRUCTION /KB */
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsINT(X0);
}

/* -------------------------------------------------------------------- */

bool akl_float(Arg)		/* MAKE WAM INSTRUCTION /KB */
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsFLT(X0);
}

/* -------------------------------------------------------------------- */

bool akl_number(Arg)
    Argdecl;
{
  register Term X0;
    
  Deref(X0, A(0));    
  IfVarSuspend(X0);
  return (IsINT(X0) || IsFLT(X0));
}

/* -------------------------------------------------------------------- */

bool akl_atomic(Arg)		/* MAKE WAM INSTRUCTION /KB */
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsAtomic(X0);
}

/* -------------------------------------------------------------------- */

bool akl_compound(Arg)		/* MAKE WAM INSTRUCTION /KB */
     Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsCompound(X0);
}

/* -------------------------------------------------------------------- */

bool akl_tree(Arg)
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return (IsAtomic(X0) || IsCompound(X0));
}

/* -------------------------------------------------------------------- */

bool akl_sys_generic(Arg)
    Argdecl;
{
  register Term X0;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsGEN(X0);
}

/* ------------------------------------------------------- */

andbox tempa;

bool akl_dif(Arg)
    Argdecl;
{
  Term X0, X1;
  bool res;
  int no_entries;
  trailentry *tr, *stop;

  Deref(X0, A(0));
  Deref(X1, A(1));

  no_entries = (exs->trail.current - exs->context.current->trail);

  res = unify(X0, X1, &tempa, exs);

  stop = (exs->context.current->trail + no_entries);

  if(res == FALSE) {
    tr = exs->trail.current;
    for(; tr != stop ; tr--) {
	Bind(Ref(tr->var),tr->val);
    }
    exs->trail.current = stop;
    return TRUE;
  }

  /* assert(res == TRUE) */

  tr = exs->trail.current;

  if(tr == stop)
    return FALSE;

  for(; tr != stop ; tr--) {
    Term t;
    *(++(exs->suspend.current)) = Ref(tr->var);
    if(exs->suspend.current == exs->suspend.end) {
      reinit_suspend(exs);
    }
    Deref(t,RefTerm(Ref(tr->var)));
    if(IsVar(t)) {
      *(++(exs->suspend.current)) = Ref(t);
      if(exs->suspend.current == exs->suspend.end) {
	reinit_suspend(exs);
      }
    }
    Bind(Ref(tr->var),tr->val);
  }
  exs->trail.current = stop;
  return SUSPEND;
}


/************************************************************************/
/***************    					*****************/
/***************    	    TERM MANIPULATION		*****************/
/***************    					*****************/
/************************************************************************/


bool akl_atom_char(Arg)
     Argdecl;
{
  register Term X0, X1, X2, tmp;
  int i;
  char *name;

  Deref(X0, A(0));
  if (!IsINT(X0))
    {
      IfVarSuspend(X0);
      DOMAIN_ERROR("atom_char", 3, 1, "integer")
    }
  if (!IsMachineInteger(X0))
    i = -1;
  else
    i = GetInteger(X0);

  Deref(X1, A(1));
  if (!IsATM(X1))
    {
      IfVarSuspend(X1);
      DOMAIN_ERROR("atom_char", 3, 2, "atom")
    }
  name = AtmPname(Atm(X1));

/* begin klooge to accomodate format/[1-4] */
/*

If i == strlen(name), then simply fail, rather than raising a domain
error.

*/
  if (i == strlen(name))
    return FALSE;
/* end klooge to accomodate format/[1-4] */
  if ((i >= strlen(name)) || (i < 0)) {
    char s[32];
    sprintf(s, "integer in 0..%d", strlen(name)-1);
    DOMAIN_ERROR("atom_char", 3, 1, s)
  }

  tmp = MakeSmallNum(name[i]);
  Deref(X2, A(2));
  return unify(X2, tmp, exs->andb, exs);
}

/* -------------------------------------------------------------------- */

bool akl_atom_to_chars(Arg)
    Argdecl;
{
  Term X0, X1, lst;

  Deref(X0, A(0));
  if (!IsATM(X0))
    {
      IfVarSuspend(X0);
      DOMAIN_ERROR("atom_to_chars", 2, 1, "atom")
    }

  lst = make_string_list(AtmPname(Atm(X0)),exs);
  Deref(X1, A(1));
  return unify(X1,lst,exs->andb,exs);
}

/* -------------------------------------------------------------------- */

bool akl_integer_to_chars(Arg)
    Argdecl;
{
  Term X0, X1, lst;

  Deref(X0, A(0));
  if (!IsINT(X0))
    {
      IfVarSuspend(X0);
      DOMAIN_ERROR("integer_to_chars", 2, 1, "integer")
    }

  {
    char temp[MAXNUMLEN];
    char *buf;
    long size = integer_sizeinbase(X0, 10) + 2;

    if (size > MAXNUMLEN)
      buf = salloc(size);
    else
      buf = temp;

    term_itoa(X0, 10, buf);

    lst = make_string_list(buf,exs);

    if (size > MAXNUMLEN)
      free(buf);			/* should use sfree or something */

    Deref(X1, A(1));
    return unify(X1,lst,exs->andb,exs);
  }
}

/* -------------------------------------------------------------------- */

bool akl_float_to_chars(Arg)
    Argdecl;
{
  Term X0, X1, lst;

  Deref(X0, A(0));
  if (!IsFLT(X0))
    {
      IfVarSuspend(X0);
      DOMAIN_ERROR("float_to_chars", 2, 1, "float")
    }

  {
    char temp[MAXNUMLEN];
    ftoa(FltVal(Flt(X0)),temp);
    lst = make_string_list(temp,exs);
    Deref(X1, A(1));
    return unify(X1,lst,exs->andb,exs);
  }
}

/* -------------------------------------------------------------------- */

bool akl_chars_to_atom(Arg)
     Argdecl;
{
  char temp[MAXATOMLEN];
  Term X0, X1;
  bool res;

  Deref(X0, A(0));

  IfVarSuspend(X0);

  res = list_to_cstring(Arg,X0,temp,MAXATOMLEN);

  if (res == TRUE)
    {
      Deref(X1, A(1));
      return unify(X1,TagAtm(store_atom(temp)),exs->andb,exs);
    }
  if (res == FALSE)
    DOMAIN_ERROR("chars_to_atom", 2, 1, "string")
  return res;
}

/* -------------------------------------------------------------------- */

bool akl_chars_to_integer(Arg)
    Argdecl;
{
  char temp[MAXNUMLEN];
  char *buf;
  int dofree = 0;
  Term X0, X1, num;
  bool res;

  Deref(X0, A(0));

  IfVarSuspend(X0);

  if (Eq(X0, NIL))
    return FALSE;

  buf = temp;

  res = list_to_cstring(Arg,X0,buf,MAXNUMLEN);

  if (res == FALSE) {		/* Maybe it was buffer overflow? */
    Term lst = X0;
    long len;

    for (len = 0; IsLST(lst); len++)
      {
	GetLstCdr(lst,Lst(lst));
	Deref(lst,lst);
      }

    IfVarSuspend(lst);

    if (lst != NIL)
      DOMAIN_ERROR("chars_to_integer", 2, 1, "string")

    buf = salloc(len+1);
    dofree = 1;
    res = list_to_cstring(Arg,X0,buf,len+1);	/* Try again */

    if (res == FALSE) {
      free(buf);
      DOMAIN_ERROR("chars_to_integer", 2, 1, "string")
    }
  }

  if (res == TRUE) {
    errno = 0;
    num = atoi_term(buf);
    if (dofree)
      free(buf);

    if (errno)
      DOMAIN_ERROR("chars_to_integer", 2, 1, "string")

    Deref(X1, A(1));
    return unify(X1,num,exs->andb,exs);
  }

  if (dofree)
    free(buf);

  return res;
}

/* -------------------------------------------------------------------- */

bool akl_chars_to_float(Arg)
    Argdecl;
{
  char temp[MAXNUMLEN];
  Term X0, X1, num;
  bool res;

  Deref(X0, A(0));

  IfVarSuspend(X0);
  if (Eq(X0, NIL))
    return FALSE;

  res = list_to_cstring(Arg,X0,temp,MAXNUMLEN);

  if (res == TRUE)
    {
      errno = 0;
      MakeFloatTerm(num, atof(temp));
      if (errno != 0 && errno != ERANGE)
	DOMAIN_ERROR("chars_to_float", 2, 1, "string")

      Deref(X1, A(1));
      return unify(X1,num,exs->andb,exs);
    }
  if (res == FALSE)
    DOMAIN_ERROR("chars_to_float", 2, 1, "string")

  return res;
}

/* -------------------------------------------------------------------- */
/* tree_to_functor(+Term, -Functor) */

bool akl_tree_to_functor(Arg)
     Argdecl;
{
  register Term X0, X1, func;

  Deref(X0, A(0));
  IfVarSuspend(X0);

  Deref(X1,A(1));

  if (IsLST(X0))
    {
      Functor f;
      f = store_functor(Atm(term_list),2);
      return unify(X1,TagGen(f),exs->andb,exs);
    } else
      if (IsSTR(X0)) {
	func = TagGen(StrFunctor(Str(X0)));
	return unify(X1,func,exs->andb,exs);
      } else 
	if (IsATM(X0) || IsINT(X0) || IsFLT(X0)) {
	  functor0 *f;
	  NewHeapFunctor(f); 
	  FnkName(f) = X0;
	  return unify(X1,TagGen(f),exs->andb,exs);
	} else
	  DOMAIN_ERROR("tree_to_functor", 2, 1, "tree")
}

/* -------------------------------------------------------------------- */
/* functor_to_tree(+F,-T) */

bool akl_functor_to_tree(Arg)
     Argdecl;
{
  register Term X0, X1;
  int j;
  Structure strct;
  Functor f;

  Deref(X0, A(0));
  IfVarSuspend(X0);

  if (!IsFUNC(X0)) {
    DOMAIN_ERROR("functor_to_tree", 2, 1, "functor")
    } else {
      Deref(X1,A(1));

      f = Fnk(X0);

      if(Eq(FnkName(f),Atm(term_list)) && (FnkArity(f) == 2))
	{
	  Term l;

	  MakeListTerm(l,exs->andb);

	  InitVariable(LstCarRef(Lst(l)),exs->andb);
	  InitVariable(LstCdrRef(Lst(l)),exs->andb);
	  return unify(X1,l,exs->andb,exs);
	} else {
	  if (FnkArity(f) == 0) {
	    Term temp;
	    functor0 *fu = (functor0*)f;
	    GetArg(temp,&(fu->name));
	    return unify(X1,temp,exs->andb,exs);
	  } else {
	    MakeStruct(strct,f,exs->andb);
	    StrFunctor(strct) = f;

	    for(j = 0; j < FnkArity(f); j++) {
	      InitVariable(StrArgRef(strct,j),exs->andb);
	    }
	    return unify(X1,TagStr(strct),exs->andb,exs);
	  }
	}
    }
}

/* -------------------------------------------------------------------- */
/* functor(+F) */

bool akl_functor(Arg)
     Argdecl;
{
  register Term X0;
  Deref(X0, A(0));
  IfVarSuspend(X0);

  return IsFUNC(X0);
}

/* -------------------------------------------------------------------- */
/* functor_to_feature(+N,+Functor,-Feature); */

bool akl_functor_feature(Arg)
     Argdecl;
{
  register Term X0,X1,X2;
  register int i;
  Term *names,trm;
  char buf[20];

  Deref(X1,A(1));
  if (!IsFUNC(X1)) {
    IfVarSuspend(X1);
    DOMAIN_ERROR("functor_feature", 3, 2, "functor")
    }

  Deref(X0,A(0));
  if (!IsINT(X0)) {
    IfVarSuspend(X0); 
    DOMAIN_ERROR("functor_feature", 3, 1, "nonnegative integer")
    }
  
  Deref(X2,A(2));
  i = GetInteger(X0);

  if (FnkArity(Fnk(X1)) == 0)
    return FALSE;		/* X1 has no arguments at all */
  else {
    if (i > 0 ) 
      if ( i <= FnkNums(Fnk(X1))) {
	return unify(X2,X0,exs->andb,exs);
      } else {
	if (i <= FnkArity(Fnk(X1))) {
	  names = FnkNames(Fnk(X1));
	  trm = names[GetInteger(X0)-FnkNums(Fnk(X1))-1];
	  return unify(X2,trm,exs->andb,exs);
	}
      }
  }
  sprintf(buf,"integer in range 1 ... %d",FnkArity(Fnk(X1)));
  DOMAIN_ERROR("functor_feature", 3, 1, buf)
}
/* -------------------------------------------------------------------- */
/* functor_arity(+Functor,-Aritylist) */

bool akl_functor_arity(Arg)
     Argdecl;
{
  register Term X0,X1,lst,new_cons,tmp_lst;
  Term *names;
  register int i,rest=0;


  Deref(X0,A(0));
  if (!IsFUNC(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("functor_arity", 2, 1, "functor")
    }


  if (FnkArity(Fnk(X0)) != 0) {
    MakeListTerm(new_cons,exs->andb); 
    lst = new_cons;
    tmp_lst = new_cons;
    names = FnkNames(Fnk(X0));
  
    for(i=0 ; i < FnkFeatures(Fnk(X0)); i++) {
      if (IsINT(names[i])) 
	if (GetInteger(names[i]) < 1) {
	  LstCar(Lst(new_cons)) = names[i];
	  tmp_lst = new_cons; 
	  MakeListTerm(new_cons,exs->andb); 
	  LstCdr(Lst(tmp_lst)) = new_cons; 
	  rest++;
	}
    }

    for (i=1; i <= FnkNums(Fnk(X0)); i++) {
      MakeIntegerTerm(LstCar(Lst(new_cons)),i);
      tmp_lst = new_cons; 
      MakeListTerm(new_cons,exs->andb); 
      LstCdr(Lst(tmp_lst)) = new_cons; 
    }

    for (i=rest; i < FnkFeatures(Fnk(X0)); i++) {
      LstCar(Lst(new_cons)) = names[i];
      tmp_lst = new_cons; 
      MakeListTerm(new_cons,exs->andb); 
      LstCdr(Lst(tmp_lst)) = new_cons; 
    }

    LstCdr(Lst(tmp_lst)) = NIL;  
  } else {
    lst = NIL;
  }
  Deref(X1,A(1));
  return unify(X1,lst,exs->andb,exs);
}

/* -------------------------------------------------------------------- */
/* functor_size(+Functor,-Size) */

bool akl_functor_size(Arg)
     Argdecl;
{
  register Term X0,X1;
  Term trm;

  Deref(X0,A(0));
  if (!IsFUNC(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("functor_size", 2, 1, "functor")
    }

  Deref(X1,A(1));
  MakeIntegerTerm(trm,FnkArity(Fnk(X0)));
  return unify(X1,trm,exs->andb,exs);
}

/* -------------------------------------------------------------------- */
/* functor_name(+Functor,-Name) */

bool akl_functor_name(Arg)
     Argdecl;
{
  register Term X0,X1;

  Deref(X0,A(0));
  if (!IsFUNC(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("functor_name", 2, 1, "functor")
    }

  Deref(X1,A(1));

  if (FnkArity(Fnk(X0)) == 0) {
    Term temp;
    functor0 *fu = (functor0*)Fnk(X0);
    GetArg(temp,&(fu->name));
    return unify(X1,temp,exs->andb,exs);
  } else 
    return unify(X1,TagAtm(FnkName(Fnk(X0))),exs->andb,exs);
}

/* -------------------------------------------------------------------- */
/* functor_features(+Functor,-Features) */

bool akl_functor_features(Arg)
     Argdecl;
{
  register Term X0,X1;
  Term trm;

  Deref(X0,A(0));
  if (!IsFUNC(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("functor_features", 2, 1, "functor")
    }

  Deref(X1,A(1));

  if (FnkArity(Fnk(X0)) == 0) {
    MakeIntegerTerm(trm,0);
  } else {
    MakeIntegerTerm(trm,FnkFeatures(Fnk(X0)));
  }
  return unify(X1,trm,exs->andb,exs);
}
/* -------------------------------------------------------------------- */
/* Help for name_arity_to_functor(N,F,Fu) 
   It checks if element of feature list is either integer or atom. */

bool legal_elmt(elmt,previous,exs)
     Term elmt,previous;
     exstate *exs;
{

  if (IsINT(elmt)) {
    if (!IsIMM(elmt)) 
      REPRESENTATION_ERROR("name_arity_to_functor", 3, 2, "exceeds maximum")
  } else  
    if (!IsATM(elmt))
      DOMAIN_ERROR("arity_name_to_functor", 3, 2, "list containing: atoms or small numbers")


  if (IsINT(previous)) {
    if (IsATM(elmt)) 
      return TRUE;		/* elmt > previous */
    else {
      if (GetInteger(elmt) < GetInteger(previous)) 
	DOMAIN_ERROR("name_arity_to_functor", 3, 2, "sorted list")  
    }
  } else {
    if (IsATM(elmt)) {
      if (strcmp(AtmPname(Atm(elmt)), AtmPname(Atm(previous))) < 0)
	DOMAIN_ERROR("name_arity_to_functor", 3, 2, "sorted list")  
    } else
      DOMAIN_ERROR("name_arity_to_functor", 3, 2, "sorted list")  
  }
  return TRUE;
}


/* -------------------------------------------------------------------- */
/* name_arity_to_functor(+Name,+Features,-Functor) */

bool akl_name_arity_to_functor(Arg)
     Argdecl;
{
  register Term X0, X1, X2, lst, car,previous,*tbl;
  int arity=0,reading_numbers=0,numbers=0,hv=0,features,i=0;
  Functor f;


  Deref(X0, A(0));
 
  if (!IsATM(X0)) {
    IfVarSuspend(X0); 
    DOMAIN_ERROR("name_arity_to_functor", 3, 1, "atom")
    }

  Deref(X1, A(1));

  if (!IsLST(X1) && !IsIMM(X1)) {
    IfVarSuspend(X1);
    DOMAIN_ERROR("name_arity_to_functor", 3, 2, "list or positive integer")
    }

  if (IsLST(X1)) {
    lst = X1;
    GetLstCar(car,Lst(lst));
    Deref(previous,car);      
    do {			/* Count arity and check if list is legal */
      GetLstCar(car,Lst(lst));
      Deref(car,car);      
      IfVarSuspend(car);	               
      if (legal_elmt(car,previous,exs)) {
	arity++;
	GetLstCdr(lst,Lst(lst));
	Deref(lst,lst);
	IfVarSuspend(lst);
	previous = car;
      } else
	return FALSE;
    } while (IsLST(lst));
    
    tbl = (Term*)(malloc(arity * sizeof(Term)));

    lst = X1;
    do {
      GetLstCar(car,Lst(lst));
      Deref(car,car);      

      if (IsATM(car)) {
	reading_numbers = 0;
      } else {
	if (( GetInteger(car) == 1) && !numbers)
	  reading_numbers = 1;
	if (reading_numbers && ( GetInteger(car) == numbers +1))
	  reading_numbers = 1;  
	else
	  reading_numbers = 0;
      }

      if (reading_numbers) {
	  numbers++;
	} else {
	  tbl[i++] = car;
	}

      GetLstCdr(lst,Lst(lst));
      Deref(lst,lst);
      build_hashvalue(car,reading_numbers,&hv); /*Build hash value */
    } while (IsLST(lst));
  
    features = arity - numbers;

    if (features == 0)
      hv = HashNameArity(Atm(X0),arity);
    else 
      hv = hv + numbers;

    if ((f = create_functor(hv,X0,features,arity,tbl)) == NULL) {
      free(tbl);
      DOMAIN_ERROR("name_arity_to_functor", 3, 2, "list without duplicates")
      }
    free(tbl);

  } else {
    if (IsINT(X1)) {
      if (!IsIMM(X1)) {		/* Not IMM => Bignum */
	REPRESENTATION_ERROR("name_arity_to_functor", 3, 2, "exceeds maximum")
	}
      if (GetInteger(X1) < 0) {
	REPRESENTATION_ERROR("name_arity_to_functor", 3, 2, "nonnegative integer")
	}
      if (GetInteger(X1) == 0) {
	functor0 *f;
	NewHeapFunctor(f); 
	FnkName(f) = X0;
	Deref(X2,A(2));
	return unify(X2,TagGen(f),exs->andb,exs);
      } else
	f = store_functor(Atm(X0),GetInteger(X1));
    } else 
      if (Eq(X1,NIL)) {
	functor0 *f;
	NewHeapFunctor(f); 
	FnkName(f) = X0;
	Deref(X2,A(2));
	return unify(X2,TagGen(f),exs->andb,exs);
      } else 
	DOMAIN_ERROR("name_arity_to_functor", 3, 2, "list or nonnegative integer")
	}	
  Deref(X2,A(2));
  return unify(X2,TagGen(f),exs->andb,exs);
}

/*----------------------------------------------------------------------
  adjoin(+Record,+Record,-New_Record)  */


bool akl_adjoin(Arg)
     Argdecl;
{
  register  Term X0,X1,X2;
  
  int       i,index,num_x,num_y,rest,features=0,arity,
            hv=0,res,next=0,numbers,j,flag=0;
  Functor   fnk_x,fnk_y,f;
  Term      value,feat,num,feat_temp,value_temp;
  Term      *atms_x,*atms_y,*end_x,*end_y,*feat_table,
             *value_table;

  Structure new_strct;
  Deref(X0,A(0));
  Deref(X1,A(1));

  IfVarSuspend(X0);
  IfVarSuspend(X1);

  Deref(X2,A(2));    

  if(!(IsSTR(X0)) && !IsATM(X0) && !IsINT(X0) && !IsFLT(X0))
    DOMAIN_ERROR("adjoin", 3, 1, "tree")


  if(!(IsSTR(X1)) && !IsATM(X1) && !IsINT(X1) && !IsFLT(X1))
    DOMAIN_ERROR("adjoin", 3, 2, "tree")


  if(!IsSTR(X0) && !IsATM(X0)) {
    if(!IsSTR(X1) && !IsATM(X1)) {
      bool res;
      compres_val compres; 

      res = akl_compare_aux(A(0), A(1), exs, &compres); 
      if(res == TRUE) { 
	if(compres == CMP_EQUAL) 
	  return unify(X2,X1,exs->andb,exs);
	else return FALSE; 
     } else
	DOMAIN_ERROR("adjoin", 3, 2, "tree in both arguments 1 and 2")
    } else
       return FALSE;
  } else
    if(!IsSTR(X1) && !IsATM(X1)) 
      return FALSE;
      
  /*Special case if X0 or X1 is an atom */      
  if(IsATM(X0)) {
    if (IsATM(X1))
      if (Eq(X0,X1))
	return unify(X2,X1,exs->andb,exs);
      else
	return FALSE;
    if (Atm(X0) == StrName(Str(X1)))
      return unify(X2,X1,exs->andb,exs);
    else
      return FALSE;
  } else
    if (IsATM(X1))
      if (Atm(X1) == StrName(Str(X0)))
	return unify(X2,X0,exs->andb,exs);
      else
	return FALSE;

  /*Both are structures */
  if (StrName(Str(X0)) != StrName(Str(X1)))
    return FALSE;
  
  fnk_x = StrFunctor(Str(X0));
  num_x = FnkNums(fnk_x);
  atms_x = FnkNames(fnk_x);
  end_x = atms_x + FnkFeatures(fnk_x);
  
  fnk_y = StrFunctor(Str(X1));
  num_y = FnkNums(fnk_y);
  atms_y = FnkNames(fnk_y);
  end_y = atms_y + FnkFeatures(fnk_y);
  
  i = (FnkArity(fnk_x) + FnkArity(fnk_y));
  feat_table = (Term*)(malloc(i * sizeof(Term)));
  value_table = (Term*)(malloc(i * sizeof(Term)));

  /* Merge numbers and values */
  if (num_y > num_x) {
    rest = 0;
  } else {
    rest = num_x - num_y;
  }

  for (index=0;index < num_y; index++) {
    GetStrArg(value,Str(X1),index);
    Deref(value,value);
    MakeIntegerTerm(num,index+1);
    value_table[next] = (value);  
    feat_table[next++] = (num); 
  }
  for (i=0;i < rest; i++) {
    GetStrArg(value,Str(X0),index+i);
    Deref(value,value);
    MakeIntegerTerm(num,index+i+1);
    value_table[next] = (value);  
    feat_table[next++] = (num); 
  }

  numbers = i+index;

  /* Merge features and their values */

  while ((atms_x < end_x) && (atms_y < end_y)) {
 
    if (IsINT(*atms_y)) {
      if (GetInteger(*atms_y) <= numbers && GetInteger(*atms_y) > 0 ) {
	lookup_feature_in_functor(*atms_y,FnkHash(fnk_y),FnkLength(fnk_y),&index);
	GetStrArg(value,Str(X1),index);
	Deref(value,value);
	value_table[GetInteger(*atms_y)-1] = (value);  
	atms_y++;
	flag = 1;		/* Don't compare *atms_x with *atms_y */
      }
    }      
    if (IsINT(*atms_x)) {
      if (GetInteger(*atms_x) <= numbers && GetInteger(*atms_x) > 0 ) {
	flag = 1;		/* Don't compare *atms_x with *atms_y */
	atms_x++;
      }
    }
    if (flag == 0) {
      if (IsINT(*atms_y)) {
	if (IsINT(*atms_x))
	  if (Eq(*atms_x,*atms_y))
	    res = 0;	/* x == y */
	  else
	    res = (2*(GetInteger(*atms_x) > GetInteger(*atms_y))) -1;
	else
	  res = 1;	/* x > y (x is atom) */
      } else {
	if(IsINT(*atms_x)) 
	  res = -1;		/* x < y (y is atom) */
	else
	  res = strcmp(AtmPname(Atm(*atms_x)), AtmPname(Atm(*atms_y))); /* atoms */
      }

      if (res >= 0) {	/* x >= y  */
	feat = *atms_y;
	lookup_feature_in_functor(feat,FnkHash(fnk_y),FnkLength(fnk_y),&index);
	GetStrArg(value,Str(X1),index);
	Deref(value,value);
	if(res==0)
	  atms_x++;
	atms_y++;
	value_table[next] = (value);  
	feat_table[next++] = (feat); 
	features++;
      } else {
	feat = *atms_x;
	lookup_feature_in_functor(feat,FnkHash(fnk_x),FnkLength(fnk_x),&index);
	GetStrArg(value,Str(X0),index);
	Deref(value,value);
	atms_x++;
	feat_table[next] = (feat); 
	value_table[next++] = (value);  
	features++;
      }
    } else 
      flag = 0;
  }
  /* Merge rest */
  if (atms_x == end_x) {
    while (atms_y < end_y) {
      lookup_feature_in_functor(*atms_y,FnkHash(fnk_y),FnkLength(fnk_y),&index);
      GetStrArg(value,Str(X1),index);
      Deref(value,value);
      if (IsINT(*atms_y)) {
	if (GetInteger(*atms_y) <= numbers && GetInteger(*atms_y) > 0 )  
	  value_table[GetInteger(*atms_y)-1] = (value);  
	else {
	  feat_table[next] = (*atms_y); 
	  value_table[next++] = (value);  
	  features++;
	}
      } else {
	feat_table[next] = (*atms_y); 
	value_table[next++] = (value);  
	features++;
      }
      atms_y++;
    }
  } else {
    while (atms_x < end_x) {
      lookup_feature_in_functor(*atms_x,FnkHash(fnk_x),FnkLength(fnk_x),&index);
      GetStrArg(value,Str(X0),index);
      Deref(value,value);
      if (IsINT(*atms_x)) {
	if (GetInteger(*atms_x) <= numbers && GetInteger(*atms_x) > 0 ) 
	  {}
	else {
	  feat_table[next] = (*atms_x); 
	  value_table[next++] = (value);  
	  features++;
	}
      } else {
	feat_table[next] = (*atms_x); 
	value_table[next++] = (value);  
	features++;
      }
      atms_x++;
    }
  
  }
  
  /* Count numbers and move values to appropriate position */
  for(i = numbers; i < next; i++) {
    if (IsINT(feat_table[i])) {
      if (GetInteger(feat_table[i]) == numbers + 1) {
	feat_temp = feat_table[i];
	value_temp = value_table[i];
	for (j = i; j > numbers; j--) {
	  feat_table[j] = feat_table[j-1];
	  value_table[j] = value_table[j-1];
	}
	feat_table[numbers] = feat_temp;
	value_table[numbers] = value_temp;	  
	numbers ++;
	features--;
      } else 
	build_hashvalue(feat_table[i],0,&hv);
    } else
      build_hashvalue(feat_table[i],0,&hv);
  }
  
  arity = features +   numbers;

  if (features == 0)
    hv = HashNameArity(StrName(Str(X0)),arity);
  else 
    hv = hv + numbers;

  if (( f = create_functor(hv,TagAtm(StrName(Str(X0))),features,arity,
			   (feat_table+numbers))) == NULL) {
    free(feat_table);
    free(value_table);
    return FALSE;
  }  else {
    free(feat_table);
    free(value_table);

    MakeStruct(new_strct,f,exs->andb);

    /* Fill in values in new structure */

    for (i = 0 ; i < arity; i++) {
      new_strct->arg[i] = value_table[i];
    }
    return unify(X2,TagStr(new_strct),exs->andb,exs);
  }
} 

/* -------------------------------------------------------------------- */

/* islist(+List) */

bool akl_islist(Arg)
    Argdecl;
{
  Term lst;

  Deref(lst, A(0));

  while (IsLST(lst))
    {
      GetLstCdr(lst,Lst(lst));
      Deref(lst,lst);
    }

  if (lst == NIL)
    return TRUE;

  IfVarSuspend(lst);

  return FALSE;
}

/* -------------------------------------------------------------------- */

/* concat(+List1, +List2, -NewList) */

bool akl_concat(Arg)
    Argdecl;
{
  Term lst, newlst, l, ln, car;

  Deref(lst, A(0));

  if (lst == NIL)		/* concat([], X, X) */
    {
      Deref(lst, A(1));
      Deref(newlst, A(2));
      return unify(lst,newlst,exs->andb,exs);
    }

  /* Check that the first list is a list and ready built */

  for (l = lst; IsLST(l); )
    {
      GetLstCdr(l,Lst(l));
      Deref(l,l);
    }

  IfVarSuspend(l);

  if (l != NIL)
    return FALSE;

  GetLstCar(car,Lst(lst));
  GetLstCdr(lst,Lst(lst));
  Deref(lst,lst);

  MakeListTerm(l,exs->andb);
  newlst = l;
  LstCar(Lst(l)) = car;

  while (IsLST(lst))
    {
      GetLstCar(car,Lst(lst));
      GetLstCdr(lst,Lst(lst));
      Deref(lst,lst);

      MakeListTerm(ln,exs->andb);
      LstCar(Lst(ln)) = car;
      LstCdr(Lst(l)) = ln;
      l = ln;
    }

  Deref(ln, A(1));
  LstCdr(Lst(l)) = ln;

  Deref(lst, A(2));
  return unify(lst,newlst,exs->andb,exs);
}

/* -------------------------------------------------------------------- */

/* memberchk(+Element, +List) */

bool akl_memberchk(Arg)
    Argdecl;
{
  Term lst, car, el;

  Deref(lst, A(1));
  Deref(el, A(0));

  while (IsLST(lst))
    {
      GetLstCar(car,Lst(lst));
      Deref(car,car);
      if (unify(el,car,exs->andb,exs))
	return TRUE;
      GetLstCdr(lst,Lst(lst));
      Deref(lst,lst);
    }

  IfVarSuspend(lst);

  return FALSE;
}

/* -------------------------------------------------------------------- */

bool akl_list_to_length(Arg)
     Argdecl;
{
  Term lst, length, X1;
  int i;

  Deref(lst, A(0));
  for (i = 0; IsLST(lst); i++)
    {
      GetLstCdr(lst,Lst(lst));
      Deref(lst,lst);
    }
  IfVarSuspend(lst);
  if (lst != NIL)
    DOMAIN_ERROR("list_to_length", 2, 1, "list")

  MakeIntegerTerm(length, i);
  Deref(X1, A(1));
  return unify(X1,length,exs->andb,exs);
}

/* -------------------------------------------------------------------- */

bool akl_length_to_list(Arg)
     Argdecl;
{
  Term length, lst, l, X1;
  int i;

  Deref(length, A(0));
  if (!IsINT(length) || CompareIntegers(<, length, MakeSmallNum(0)))
    {
      IfVarSuspend(length);
      DOMAIN_ERROR("length_to_list", 2, 1, "nonnegative integer")
    }
  if (!IsMachineInteger(length))
    REPRESENTATION_ERROR("length_to_list", 2, 1, "exceeds maximum")

  lst = NIL;
  for (i = GetInteger(length); i > 0; i--)
    { 
      MakeListTerm(l,exs->andb);
      LstCdr(Lst(l)) = lst;
      lst = l;

      InitVariable(LstCarRef(Lst(lst)),exs->andb);
    }
  Deref(X1, A(1));
  return unify(lst,X1,exs->andb,exs);
}

/*------------------------------------------------------------------------ */
bool akl_arg(Arg)
    Argdecl;
{
  register Term X0, X1, X2, tmp;
  int i;

  Deref(X0, A(0));

  if (!IsINT(X0))
    {
      IfVarSuspend(X0);

      Deref(X1, A(1));
      IfVarSuspend(X1);

      if (IsATM(X0) && IsSTR(X1)) {
	if (lookup_feature_in_functor(X0,StrHash(Str(X1)),StrLength(Str(X1)),&i)) {
	  GetStrArg(tmp,Str(X1),i);
	  Deref(tmp,tmp);
	  Deref(X2, A(2));
	  return unify(X2,tmp,exs->andb,exs);
	} else {
	  DOMAIN_ERROR("arg", 3, 1, "valid feature")
	}
      } else {
	DOMAIN_ERROR("arg", 3, 1, "atom or nonegative integer")
	}
    }

  if (!IsMachineInteger(X0))
    i = 0;
  else
    i = GetInteger(X0);

  Deref(X1, A(1));

  if (IsLST(X1))
    {
      if(i == 1) {
	GetLstCar(tmp, Lst(X1));
	Deref(tmp, tmp);
	Deref(X2, A(2));
	return unify(X2,tmp,exs->andb,exs);
      }
      if(i == 2) {
	GetLstCdr(tmp, Lst(X1));
	Deref(tmp, tmp);
	Deref(X2, A(2));
	return unify(X2,tmp,exs->andb,exs);
      }
      DOMAIN_ERROR("arg", 3, 1, "integer in 1..2")
    }

  if (IsSTR(X1))
    {
      if (IsINT(X0) && IsIMM(X0)) {
	if((i > (StrArity(Str(X1)) - StrFeatures(Str(X1)))) || (i <= 0)) {
	  if (lookup_feature_in_functor(X0,StrHash(Str(X1)),StrLength(Str(X1)),&i)) {
	    GetStrArg(tmp,Str(X1),i);
	    Deref(tmp,tmp);
	    Deref(X2, A(2));
	    return unify(X2,tmp,exs->andb,exs);
	  } else 
	    DOMAIN_ERROR("arg", 3, 1, "valid feature")
	} else {
	  i--;
	  GetStrArg(tmp,Str(X1),i);
	  Deref(tmp, tmp);
	  Deref(X2, A(2));
	  return unify(X2,tmp,exs->andb,exs);
	}
      } else 
	DOMAIN_ERROR("arg", 3, 1, "atom or small integer")
	}
  IfVarSuspend(X1);
  DOMAIN_ERROR("arg", 3, 2, "compound tree")
  }

/* -------------------------------------------------------------------- */

/* sys:setarg/3,  destructive update of a structure argument */

bool akl_sys_setarg(Arg)
     Argdecl;
{
  register Term X0, X1, X2;

  Deref(X0,A(0));
  Deref(X1,A(1));
  Deref(X2,A(2));

  if(IsINT(X0) && IsSTR(X1) ) {
    int i;
    i = GetInteger(X0);
    if(IsMachineInteger(X0) && i > 0 && i <= StrArity(Str(X1))) {
        i--;
    	StrArgument(Str(X1),i) = X2;
    	return TRUE;
      } else
	MINOR_FAULT("sys:setarg segmentation fault");
  }
  IfVarSuspend(X0);
  IfVarSuspend(X1);
  return FALSE;
}


/* -------------------------------------------------------------------- */

Term make_string_list(str, exs)
    char *str;
    exstate *exs;
{
    register Term res;
    register Term val;

    if(*str == '\0')
	return NIL;

    MakeListTerm(res,exs->andb);
    MakeIntegerTerm(val, ((uword) *(str++)));
    LstCar(Lst(res)) = val;
    LstCdr(Lst(res)) = make_string_list(str,exs);

    return res;
}

/* -------------------------------------------------------------------- */

void itoa(l, base, cbuf)
     long l;
     int base;
     char *cbuf;
{
  char hibase = 'a'-10;
  int sx = (l<0);
  long digit;
  register char *c0, *c, d;

  c = cbuf;
  if (base<0)
    hibase = 'A'-10,
    base = -base;
  if (sx)
    *c++ = '-',
    l = -l;

  do
    digit = l % base,
    l /= base,
    *c++ = (digit<10 ? '0'+digit : hibase+digit);
  while (l>0);

  *c++ = 0;
  for (c0=cbuf+sx, c-=2; c0<c; c0++, c--)
    d = *c0, *c0 = *c, *c = d;
}

/* -------------------------------------------------------------------- */

/* Float to chars using just enough digits to ensure that distinct numbers
   come out differently.  Reasonable machines use between 52 and 56 mantissa
   bits, yielding 15-17 significant digits.  Check first if 15 digits
   suffice; if not, use 16 or 17 digits.
*/

void ftoa(f, cbuf)
     double f;
     char *cbuf;
{
  double ff;
  register char *str, *str2;
  register int i, j;

  snprintf(str=cbuf, sizeof(cbuf), "%.15g", f);
  if (str[0]=='-') str++;
  switch (str[0])
    {
    case '+':			/* hpux */
    case '-':			/* hpux */
    case 'I':
      strcpy(str, "inf");
      return;
    case '?':			/* hpux */
    case 'N':
      strcpy(str, "nan");
      return;
    }
  ff = atof(cbuf);
  if (f != ff)
    snprintf(str=cbuf, sizeof(cbuf), "%.16g", f);
  ff = atof(cbuf);
  if (f != ff)
    snprintf(str=cbuf, sizeof(cbuf), "%.17g", f);

				/* add ".0" if need be */
  while (TRUE)
    {
      switch (*str++)
	{
	case 0:
	case 'e':
	case 'E':
	  i = str-cbuf-1;
	  j = strlen(cbuf);
	  str = cbuf;
	  while (j>=i)
	    str[j+2] = str[j], --j;
	  str[i++] = '.';
	  str[i++] = '0';
	  goto strip;
	case '.':
	  switch (*str)
	    {
	    case 0:
	    case 'e':
	    case 'E':
	      *str++ = '0',
	      *str++ = 0;
	    }
	  goto strip;
	default:
	  ;
	}
    }

 strip:			/* strip trailing zeros for some machines */
  str = cbuf;
  while (TRUE)
    switch (*str++)
      {
      case '.':
	i = -1;
	break;
      case '0':
	i++;
	break;
      case 0:
      case 'e':
      case 'E':
	goto move;
      default:
	i = 0;
      }
 move:
  if (i>0)
    {
      --str;
      str2 = str-i;
      while ((*str2++ = *str++))
	;
    }
}

static bool list_to_cstring(Arg,lst,string,maxlen)
     Argdecl;
     Term lst;
     char *string;
     int maxlen;
{
  int i;
  Term next;

  for (i = 0; IsLST(lst); )
    {
      GetLstCar(next,Lst(lst));
      Deref(next,next);
      if(IsMachineInteger(next))
	{
	  string[i++] = (char) GetInteger(next);
	  if(i == maxlen)
	    {
	      return FALSE;
	    }
	  GetLstCdr(lst,Lst(lst));	/* move to next element */
	  Deref(lst,lst);
	}
      else
	{
	  IfVarSuspend(next);	/* not bound yet */
	  return FALSE;		/* not a valid list element */
	}
    }

  IfVarSuspend(lst);		/* not bound yet */
  if(!Eq(lst, NIL))
    return FALSE;
  string[i] = '\0';
  return TRUE;
}


void initialize_builtin() {

  term_list = TagAtm(store_atom("."));

  define("$var_descriptor",akl_var_descriptor,2);
  define("$var",akl_var,1);
  define("$eq",akl_eq,2);

  define("data",akl_data,1);
  define("atom",akl_atom,1);
  define("integer",akl_integer,1);
  define("float",akl_float,1);
  define("atomic",akl_atomic,1);
  define("compound",akl_compound,1);
  define("tree",akl_tree,1);
  define("sys_generic",akl_sys_generic,1);

  define("atom_char",akl_atom_char,3);
  define("atom_to_chars",akl_atom_to_chars,2);
  define("integer_to_chars",akl_integer_to_chars,2);
  define("float_to_chars",akl_float_to_chars,2);
  define("chars_to_atom",akl_chars_to_atom,2);
  define("chars_to_integer",akl_chars_to_integer,2);
  define("chars_to_float",akl_chars_to_float,2);

  define("list_to_length",akl_list_to_length,2);
  define("length_to_list",akl_length_to_list,2);

  define("tree_to_functor",akl_tree_to_functor,2); 
  define("functor_to_tree",akl_functor_to_tree,2); 

  define("functor",akl_functor,1);
  define("functor_size",akl_functor_size,2); 
  define("functor_arity",akl_functor_arity,2); 
  define("functor_name",akl_functor_name,2); 
  define("functor_features",akl_functor_features,2); 
  define("functor_feature",akl_functor_feature,3); 
  define("name_arity_to_functor",akl_name_arity_to_functor,3); 
  define("adjoin",akl_adjoin,3);

  define("arg",akl_arg,3);
  define("sys_setarg",akl_sys_setarg,3);

  define("$islist",akl_islist,1);
  define("$concat",akl_concat,3);
  define("$memberchk",akl_memberchk,2);

  /* somewhat experimental [SJ] */

  define("dif",akl_dif,2);
  
  tempa.env = NULL;
  tempa.status = STABLE;
  tempa.tried = NULL;
  tempa.cont = NULL;
  tempa.constr = NULL;
  tempa.unify = NULL;
  tempa.father = NULL;
  tempa.previous = NULL;
  tempa.next = NULL;
#ifdef TRACE
  tempa.id = 0;
  tempa.trace = 0;
#endif

}
