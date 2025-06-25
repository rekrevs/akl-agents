/* $Id: array.c,v 1.28 1994/04/18 13:22:35 rch Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "storage.h"
#include "unify.h"
#include "copy.h"
#include "gc.h"
#include "initial.h"
#include "config.h"
#include "names.h"
#include "display.h"
#include "error.h"
#include "array.h"
#include "inout.h"

static bool method_typeof();
static bool method_sizeof();
static bool method_get_array();
static bool method_set_array();
static bool method_swap_array();
static bool method_sub_array();

static Gvainfo	newarray();
static bool 	unifyarray();
static int 	printarray();
static Gvainfo	copyarray();
static Gvainfo	gcarray();
static int	uncopiedarray();
static envid	*deallocatearray();
static int	killarray();
static bool	sendarray();

typedef struct array {
  gvainfo	header;
  Term		data;		/* Pointer to structure with content */
  Term		original;	/* Port to original array object or NullTerm */
  int		size;		/* Number of cells in array */
  int		offset;		/* Start of array in data */
  int		start;		/* subtract index */
  int		scale;		/* multiply index */
} Array;

/* This fileds gives us some more information. We call the memory
 * content in an array for 'cells' and the cells that belongs to
 * an array for 'elements'. Number of 'elements' <= number of 'cells'.
 *
 *   offset			First element in array
 *   index - start		Zero based index (zindex)
 *   zindex * scale + offset	Indexed cell
 *   size * scale + offset	Points out cell after last cell
 *   
 *   newoff = oldoff + oldscale * offset
 *   
 *   newscale = oldscale * scale
 *   
 *   
 */   
static gvamethod arraymethods = {
  newarray,
  unifyarray,
  printarray,
  copyarray,
  gcarray,
  uncopiedarray,  
  deallocatearray,
  killarray,
  NULL,				/* Close only called if in close list */
  sendarray,
  NULL,
  NULL
};


static Atom atom_nil;
static Term term_array;
static Functor functor_typeof;
static Functor functor_sizeof;
static Functor functor_subarray_3;
static Functor functor_subarray_4;
static Functor functor_subarray_5;
static Functor functor_get_2;
static Functor functor_set_2;
static Functor functor_set_3;

#define GvaArr(g)  ((Array*)(g))
#define Arr(x)     GvaArr(RefGva(Ref(x)))


/* ------------------------------------------------------------------------ */
/* LOW LEVEL OBJECT METHODS */

static Gvainfo newarray(old)
     Array *old;
{
  Array *new;
  NEW(new,Array);		/* Called before copy and gc */

  new->size   = old->size;	/* Copy all but terms */
  new->offset = old->offset;
  new->start    = old->start;
  new->scale    = old->scale;

  return (Gvainfo) new;
}


/* If we are the original object we copy the actual data */
/* If we are a subarray we copy the original. If the original */
/* is already copied we get a pointer to the new object anyway */

static Gvainfo copyarray(old,new)
     Array *old, *new;
{
  new->original   = old->original;
  new->data   = old->data;

  if (!Eq(new->original, NullTerm))
    copy_location(&new->original);
    
  copy_location(&new->data);

  return (Gvainfo) new;
}


static Gvainfo gcarray(old,new,gcst)
     Array *old, *new;
     gcstatep gcst;
{
  new->original   = old->original;
  new->data   = old->data;

  if (!Eq(new->original, NullTerm))
    gc_location(&new->original, gcst);
    
  gc_location(&new->data, gcst);
    
  return (Gvainfo) new;
}


static envid *deallocatearray(prt)
     Array *prt;
{
  /* dispose the storage */
  return NULL;
}


static int uncopiedarray(prt)
     Array *prt;
{
  return 0;
}
    

static bool unifyarray(x0,x1,andb,exs)
     Term x0, x1;
     andbox *andb;
     exstate *exs;
{
  if (Eq(x0, x1))		/* Not as simple as I thought */
    return TRUE;		/* so just test for equality */
  return FALSE;
}


static int killarray(prt)
     Array *prt;
{
  return 1;
}


static int printarray(file,p,tsiz,dict)
     FILE *file;
     Array *p;
     int tsiz;
     struct ptrdict *dict;
{
  int i;
  Term *x = StrArgRef(Str(p->data),p->offset);

  fprintf(file, "{array[%d..%d]: ",p->start,p->start+p->size-1);

  fdisplay_cyc_term(file, x[0], tsiz-1, dict);

  for (i = 1; (i != tsiz-1 && i < p->size); i++)
    {
      fputc(',', file);
      fdisplay_cyc_term(file, x[i*p->scale], tsiz-1, dict);
    }

  if (i == tsiz-1 && i != p->size)
    fprintf(file,",...}");
  else
    fprintf(file,"}");

  return 1;
}


/* ------------------------------------------------------------------------ */
/* LOW LEVEL OBJECT SEND METHOD */

static bool sendarray(message, self, exs)
     Term message, self;
     exstate *exs;
{
  Deref(message, message);

  if(IsSTR(message)) {
    Functor op = StrFunctor(Str(message));
    Term X0, X1, X2, X3, X4;
    
    if (op == functor_get_2)
      {
	GetStrArg(X0,Str(message),0);
	GetStrArg(X1,Str(message),1);
	return method_get_array(X0,X1,self,exs);
      }
    else
      if(op == functor_set_2)
	{
	  GetStrArg(X0,Str(message),0);
	  GetStrArg(X1,Str(message),1);
	  return method_set_array(X0,X1,self,exs);
	}
      else
	if(op == functor_set_3)
	  {
	    GetStrArg(X0,Str(message),0);
	    GetStrArg(X1,Str(message),1);
	    GetStrArg(X2,Str(message),2);
	    return method_swap_array(X0,X1,X2,self,exs);
	  }
	else
	  if(op == functor_subarray_3)
	    {
	      GetStrArg(X0,Str(message),0);
	      GetStrArg(X1,Str(message),1);
	      GetStrArg(X2,Str(message),2);
	      return method_sub_array(X0,X1,NULL,NULL,self,X2,3,exs);
	    }
	  else
	    if(op == functor_subarray_4)
	      {
		GetStrArg(X0,Str(message),0);
		GetStrArg(X1,Str(message),1);
		GetStrArg(X2,Str(message),2);
		GetStrArg(X3,Str(message),3);
		return method_sub_array(X0,X1,X2,NULL,self,X3,4,exs);
	      }
	    else
	      if(op == functor_subarray_5)
		{
		  GetStrArg(X0,Str(message),0);
		  GetStrArg(X1,Str(message),1);
		  GetStrArg(X2,Str(message),2);
		  GetStrArg(X3,Str(message),3);
		  GetStrArg(X4,Str(message),4);
		  return method_sub_array(X0,X1,X2,X3,self,X4,5,exs);
		}
	      else
		if(op == functor_typeof)
		  {
		    GetStrArg(X0,Str(message),0);
		    return method_typeof(X0,exs);
		  }
		else
		  if(op == functor_sizeof)
		    {
		      GetStrArg(X0,Str(message),0);
		      return method_sizeof(X0,self,exs);
		    }
		  else
		    METHOD_EXISTENCE_ERROR(message, self)
    
  }
  
  IfVarSuspend(message);
  METHOD_EXISTENCE_ERROR(message, self)
}

/* ------------------------------------------------------------------------ */

static bool akl_new_array(Size,Origin,New,arity,exs)
     Term Size, Origin, New;
     int arity;
     exstate *exs;
{
  int size, i;
  int start = 0;
  Array *new;
  Term tmpv;

  Deref(Size,Size);		/* The size of the new array */
  IfVarSuspend(Size);

  if (!IsINT(Size) || CompareIntegers(<, Size, MakeSmallNum(1)))
    DOMAIN_ERROR("new_array", arity, 1, "positive integer")
  else if (!IsMachineInteger(Size))
    REPRESENTATION_ERROR("new_array", arity, 1, "exceeds maximum")
  else
    size = GetInteger(Size);

  if (!Eq(Origin,NullTerm))
    {
      Deref(Origin,Origin);		/* The start index of the new array */
      IfVarSuspend(Origin);

      if (!IsINT(Origin))
	DOMAIN_ERROR("new_array", arity, 2, "integer")
      else if (!IsMachineInteger(Origin))
	REPRESENTATION_ERROR("new_array", arity, 2, "magnitude exceeds maximum")
      else
	start = GetInteger(Origin);
    }

  MakeGvainfo(new,Array,&arraymethods,exs->andb);
  MakeStructTerm(new->data,store_functor(atom_nil, size),exs->andb);

  /* CHECK OVERFLOW */

  new->original = NullTerm;	/* Not a subarray */
  new->size = size;
  new->offset = 0;		/* original has the whole array */
  new->start = start;
  new->scale = 1;

  {
    Term *x = StrArgRef(Str(new->data),0);
    for (i = 0; (i < size); i++)
      *(x++) = term_nil;
  }

/*  add_gvainfo_to_close((Gvainfo)new,exs);   ONLY IF MALLOC THE DATA */

  Deref(New,New);
  MakeCvaTerm(tmpv, (Gvainfo)new);
  return unify(New,tmpv,exs->andb,exs);
}

/* ------------------------------------------------------------------------ */

/* new_array(+Size, -Array) */

bool akl_new_array_2(Arg)
     Argdecl;
{
  return akl_new_array(A(0),NullTerm,A(1),2,exs);
}


/* ------------------------------------------------------------------------ */

/* new_array(+Size, +Start, -Array) */

bool akl_new_array_3(Arg)
     Argdecl;
{
  return akl_new_array(A(0),A(1),A(2),3,exs);
}


/* ------------------------------------------------------------------------ */

/* subarray(+Index, +Size, -New)@ +Old */
/* subarray(+Index, +Size, +Start, -New)@ +Old */
/* subarray(+Index, +Size, +Start, +Scale, -New)@ +Old */

static bool method_sub_array(Index, Size, Origin, Scale, Old, New, arity, exs)
     Term Index, Size, Origin, Scale, Old, New;
     int arity;
     exstate *exs;
{
  Array *old, *new;
  int size, offset, start, scale, end, old_end;
  Term tmpv, A[5];

  A[0] = Index;
  A[1] = Size;
  if (arity == 3)
    A[2] = New;
  else if (arity == 4) {
    A[2] = Origin;
    A[3] = New;
  } else /* if (arity == 5) */ {
    A[2] = Origin;
    A[3] = Scale;
    A[4] = New;
  }

  /* Old is this object and already dereferenced */
  /* and known to be a CVA variable */

  if(!IsLocalGVA(Ref(Old),exs->andb))
    {
      /* The array is not local to the current andbox
       * A constraint is constructed without a suspension.
       * The constraint will be retried when promoted.
       */
      return SUSPEND;
    }

  old = Arr(Old);

  Deref(Index, Index);	/* The index of the sub array */
  IfVarSuspend(Index);

  if (!IsINT(Index))
    METHOD_DOMAIN_ERROR("subarray", arity, 1, "integer", Old)
  else if (!IsMachineInteger(Index))
    offset = -1;
  else
    offset = GetInteger(Index)-old->start;

  if ((offset >= old->size) || (offset < 0)) {
    char s[32];
    sprintf(s, "integer in %d..%d", old->start, old->start+old->size-1);
    METHOD_DOMAIN_ERROR("subarray", arity, 1, s, Old)
  }

  offset = (offset*old->scale)+old->offset; /* True offset in data */

  Deref(Size, Size);		/* The size of the sub array */
  IfVarSuspend(Size);

  if (!IsINT(Size) || CompareIntegers(<, Size, MakeSmallNum(1)))
    METHOD_DOMAIN_ERROR("subarray", arity, 2, "positive integer", Old)
  else if (!IsMachineInteger(Size))
    METHOD_REPRESENTATION_ERROR("subarray", arity, 2, "exceeds maximum", Old)
  else
    size = GetInteger(Size);

  if (!Eq(Origin,NullTerm))
    {
      Deref(Origin, Origin);	/* The first index */
      IfVarSuspend(Origin);

      if (!IsINT(Origin))
	METHOD_DOMAIN_ERROR("subarray", arity, 3, "integer", Old)
      else if (!IsMachineInteger(Origin))
	METHOD_REPRESENTATION_ERROR("subarray", arity, 3, "magnitude exceeds maximum", Old)
      else
	start = GetInteger(Origin);
    }
  else
    start = old->start;

  if (!Eq(Scale,NullTerm))
    {
      Deref(Scale, Scale);	/* The first index */
      IfVarSuspend(Scale);

      if (!IsINT(Scale))
	scale = 0;
      else if (!IsMachineInteger(Scale))
	METHOD_REPRESENTATION_ERROR("subarray", arity, 4, "magnitude exceeds maximum", Old)
      else
	scale = GetInteger(Scale);

      if (scale == 0)
	METHOD_DOMAIN_ERROR("subarray", arity, 4, "nonzero integer", Old)

      scale *= old->scale;
    }
  else
    scale = old->scale;

  end = offset+(size-1)*scale;
  old_end = old->offset+(old->size-1)*old->scale;
  if (((scale > 0) && (old->scale > 0) &&
       ((offset < old->offset) || (end > old_end))) ||
      ((scale > 0) && (old->scale < 0) &&
       ((offset < old_end) || (end > old->offset))) ||
      ((scale < 0) && (old->scale > 0) &&
       ((end < old->offset) || (offset > old_end))) ||
      ((scale < 0) && (old->scale < 0) &&
       ((end < old_end) || (offset > old->offset)))) /* crude but correct */
    METHOD_PERMISSION_ERROR("subarray", arity, "create", "subarray", NullTerm, "out of bounds", Old)

  MakeGvainfo(new,Array,&arraymethods,exs->andb);
  new->size = size;
  new->offset = offset;
  new->start = start;
  new->scale = scale;
  new->data = old->data;
  new->original = Old;

/*  add_gvainfo_to_close((Gvainfo)new,exs);   ONLY IF MALLOC THE DATA */

  Deref(New, New);		/* The term unified with the array */
  MakeCvaTerm(tmpv, (Gvainfo)new);
  return unify(New,tmpv,exs->andb,exs);
}

/* ------------------------------------------------------------------------ */

/* get(+Index, -Term)@ +Array */

static bool method_get_array(Off, ArgOld, ArrIn, exs)
     Term Off, ArgOld, ArrIn;
     exstate *exs;
{
  Term ArgTmp, A[2];
  Array *p;
  int i;

  A[0] = Off;
  A[1] = ArgOld;

  /* ArrIn is self and already dereferenced and checked */

  if(!IsLocalGVA(Ref(ArrIn),exs->andb))
    {
      /* The array is not local to the current andbox
       * A constraint is constructed without a suspension.
       * The constraint will be retried when promoted.
       */
      return SUSPEND;
    }

  p = Arr(ArrIn);

  Deref(Off, Off);
  IfVarSuspend(Off);

  if (!IsINT(Off))
    METHOD_DOMAIN_ERROR("get", 2, 1, "integer", ArrIn)
  else if (!IsMachineInteger(Off))
    i = -1;
  else
    i = GetInteger(Off)-p->start;

  if ((i >= p->size) || (i < 0)) {
    char s[32];
    sprintf(s, "integer in %d..%d", p->start, p->start+p->size-1);
    METHOD_DOMAIN_ERROR("get", 2, 1, s, ArrIn)
  }

  i = (i*p->scale)+p->offset; /* True offset in data */

  ArgTmp = StrArgument(Str(p->data),i);

  Deref(ArgOld, ArgOld);
  Deref(ArgTmp, ArgTmp);

  return unify(ArgOld,ArgTmp,exs->andb,exs);
}

/* ------------------------------------------------------------------------ */

/* set(+Index, +Term)@ +Array */

static bool method_set_array(Off, ArgNew, ArrIn, exs)
     Term Off, ArgNew, ArrIn;
     exstate *exs;
{
  Term *x, A[2];
  Array *p;
  int i;

  A[0] = Off;
  A[1] = ArgNew;

  /* ArrIn is self and already dereferenced and checked */

  if(!IsLocalGVA(Ref(ArrIn),exs->andb))
    {
      /* The array is not local to the current andbox
       * A constraint is constructed without a suspension.
       * The constraint will be retried when promoted.
       */
      return SUSPEND;
    }

  p = Arr(ArrIn);

  Deref(Off, Off);
  IfVarSuspend(Off);

  if (!IsINT(Off))
    METHOD_DOMAIN_ERROR("set", 2, 1, "integer", ArrIn)
  else if (!IsMachineInteger(Off))
    i = -1;
  else
    i = GetInteger(Off)-p->start;

  if ((i >= p->size) || (i < 0)) {
    char s[32];
    sprintf(s, "integer in %d..%d", p->start, p->start+p->size-1);
    METHOD_DOMAIN_ERROR("set", 2, 1, s, ArrIn)
  }

  i = (i*p->scale)+p->offset; /* True offset in data */

  x = StrArgRef(Str(p->data),i);
  *x = ArgNew;			/* Destructive set */
	      
  return TRUE;
}

/* ------------------------------------------------------------------------ */

/* set(+Index, -OldTerm, +Term)@ +Array */

static bool method_swap_array(Off, ArgOld, ArgNew, ArrIn, exs)
     Term Off, ArgOld, ArgNew, ArrIn;
     exstate *exs;
{
  Term ArgTmp, *x, A[3];
  Array *p;
  int i;

  A[0] = Off;
  A[1] = ArgOld;
  A[2] = ArgNew;

  /* ArrIn is self and already dereferenced and checked */

  if(!IsLocalGVA(Ref(ArrIn),exs->andb))
    {
      /* The array is not local to the current andbox
       * A constraint is constructed without a suspension.
       * The constraint will be retried when promoted.
       */
      return SUSPEND;
    }

  p = Arr(ArrIn);

  Deref(Off, Off);
  IfVarSuspend(Off);

  if (!IsINT(Off))
    METHOD_DOMAIN_ERROR("set", 3, 1, "integer", ArrIn)
  else if (!IsMachineInteger(Off))
    i = -1;
  else
    i = GetInteger(Off)-p->start;

  if ((i >= p->size) || (i < 0)) {
    char s[32];
    sprintf(s, "integer in %d..%d", p->start, p->start+p->size-1);
    METHOD_DOMAIN_ERROR("set", 3, 1, s, ArrIn)
  }

  i = (i*p->scale)+p->offset; /* True offset in data */

  x = StrArgRef(Str(p->data),i);
  ArgTmp = *x;
  *x = ArgNew;
	      
  Deref(ArgOld, ArgOld);
  Deref(ArgTmp, ArgTmp);

  return unify(ArgOld,ArgTmp,exs->andb,exs);
}

/* ------------------------------------------------------------------------ */

/* typeof(-Type)@ +Array */

static bool method_typeof(type, exs)
     Term type;
     exstate *exs;     
{
  Deref(type,type);
  return unify(type, term_array, exs->andb, exs);
}

/* ------------------------------------------------------------------------ */

/* sizeof(-Size)@ +Array */

static bool method_sizeof(size, self, exs)
     Term size, self;
     exstate *exs;     
{
  Term tmp;
  Array *arr = Arr(self);

  MakeIntegerTerm(tmp, arr->size);
  Deref(size,size);
  return unify(size, tmp, exs->andb, exs);
}

/* ------------------------------------------------------------------------ */

void initialize_array() {

  atom_nil = store_atom("[]");
  term_array = TagAtm(store_atom("array"));

  functor_typeof = store_functor(store_atom("typeof"),1);
  functor_sizeof = store_functor(store_atom("sizeof"),1);

  functor_subarray_3 = store_functor(store_atom("subarray"),3);
  functor_subarray_4 = store_functor(store_atom("subarray"),4);
  functor_subarray_5 = store_functor(store_atom("subarray"),5);
  functor_get_2 = store_functor(store_atom("get"),2);
  functor_set_2 = store_functor(store_atom("set"),2);
  functor_set_3 = store_functor(store_atom("set"),3);

  define("new_array",akl_new_array_2,2);
  define("new_array",akl_new_array_3,3);
}
