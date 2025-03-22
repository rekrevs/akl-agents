/* $Id: ghash.c,v 1.18 1994/05/17 13:45:41 bd Exp $ */

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
#include "compare.h"
#include "ghash.h"

typedef Term HashKey;
typedef Term HashValue;

#include "hash.h"


/* ######################################################################## */
/* 			HASH DEFINITION FOR TERMS 			    */
/* ######################################################################## */

/* Initial settings */

#define DEFAULT_TABLE_SIZE 16

/* ######################################################################## */

extern uword bignum_hash_value();

static bool ground_hash_func(exs,term,hval)
     exstate *exs;
     Term term;
     uword *hval;
{
  DerefSwitchTerm(term,varlbl,snlbl,atmlbl,bnlbl,fltlbl,lstlbl,strlbl,genlbl);

  /* uva falls through */

 varlbl:
  IfVarSuspend(term);		/* now it suspends */

 snlbl:
  *hval += (*hval << 4) + NumVal(Num(term));
  return TRUE;

 atmlbl:
  *hval += (*hval << 4) + AtmHash(Atm(term));  
  return TRUE;

 bnlbl:
  *hval += (*hval << 4) + bignum_hash_value(term);
  return TRUE;

 fltlbl:
  {
    uword *ip;
    double *dp;
    dp = &FltVal(Flt(term));
    ip = (uword *)dp;
    *hval += (*hval << 4) + *ip;
    return TRUE;
  }

 lstlbl:
  {
    bool err;
    Term car, cdr;
    
    GetLstCar(car,Lst(term));
    err = ground_hash_func(exs, car, hval);
    if (err != TRUE)
      return err;		/* Give up because suspended */
    
    GetLstCdr(cdr,Lst(term));
    err = ground_hash_func(exs, cdr, hval);
    if (err != TRUE)
      return err;		/* Give up because suspended */
    
    return TRUE;
  }

 strlbl:
  {
    int i;
    bool err;
    Term tmp_term;

    for (i = 0; i < StrArity(Str(term)); i++)
      {
	GetStrArg(tmp_term,Str(term),i);
	err = ground_hash_func(exs, tmp_term, hval);
	if (err != TRUE)
	  return err;		/* Give up because suspended */
      }
    
    *hval += (*hval << 4) + AtmHash(StrName(Str(term))) + StrArity(Str(term));
    return TRUE;
  }

 genlbl:
/*
  *hval += (*hval << 4) + (uword)(Gen(term)->method);
  return TRUE;
*/
  return FALSE;
}

#define GHashValue(Name, Arity, ArgNo, Object) \
{ \
  bool err; \
  ht_hash = 0; \
  err = ground_hash_func(exs, ht_key, &ht_hash); \
  if (err == FALSE) \
    METHOD_DOMAIN_ERROR((Name), (Arity), (ArgNo), "pure tree", (Object)) \
  else if (err != TRUE) \
    return err; \
}



/* ######################################################################## */

/* Check if two ground terms are equal */

static Functor functor_cons;



/* ######################################################################## */

#define HashAllocate \
{ \
  if (ht_size > HLIMIT) \
    ht_memory = malloc(ht_size); \
  else \
    { \
      ht_memory = (char *) heapcurrent; \
      heapcurrent += ht_size; \
      if(heapcurrent >= heapend) \
	{\
	  SaveHeapRegisters(); \
	  reinit_heap(ht_memory,ht_size); \
	  RestoreHeapRegisters(); \
	  ht_memory = (char *) heapcurrent; \
	  heapcurrent += ht_size; \
	}\
    } \
}

#define HashFree \
{ \
  if (ht_size > HLIMIT) \
    free(ht_memory);		/* Then allocated by malloc */ \
}


/* ######################################################################## */
/* 			GENERIC HASH OBJECT	 			    */
/* ######################################################################## */


typedef struct {
  gvainfo	header;
  HashTable	*table;		/* Pointer to actual table and info */
} GenHTable;

static Gvainfo	newhash(GenHTable *old);
static bool 	unifyhash(Term x0, Term x1, andbox *andb, exstate *exs);
static int 	printhash(FILE *file, GenHTable *table, int tsiz);
static Gvainfo	copyhash(GenHTable *old,GenHTable *new);
static Gvainfo	gchash(GenHTable *old,GenHTable *new, gcstatep gcst);
static bool	sendhash(Term message, Term self, exstate *exs);
static int 	uncopiedhash(GenHTable *gtable);
static envid	*deallocatehash(GenHTable *gtable);
static int	killhash(GenHTable *gtable);


static gvamethod hashmethods = {
  newhash,
  unifyhash,
  printhash,
  copyhash,
  gchash,
  uncopiedhash,			/* Only used if in close list */
  deallocatehash,		/* Only used if in close list */
  killhash,			/* Only used if in close list */
  NULL,
  sendhash,
  NULL,
  NULL
};



/* If a hash table bigger than a limit it is */
/* allocated with malloc() outside the heap */
/* The limit is in number of cells in table */

#define HLIMIT 10000		/* Limit in bytes of heap hash table */

#define IsGenHTable(Prt) (IsCvaTerm(Prt) && \
			  RefCvaMethod(Ref(Prt)) == &(hashmethods))

#define GvaHash(g)	((GenHTable*)(g))
#define Hash(x)		GvaHash(RefGva(Ref(x)))

/* ######################################################################## */

/* LOW LEVEL OBJECT METHODS */

/* Called before copy and gc */
/* Allocates space for the generic object itself, NOT the array */

static Gvainfo newhash(old)
     GenHTable *old;
{
  GenHTable *new;

  NEW(new,GenHTable);

  return (Gvainfo) new;
}


/* ------------------------------------------------------------------------ */

#define HashElemCopy \
{ \
  ht_newentry->key = ht_entry->key; \
  ht_newentry->value = ht_entry->value; \
  copy_location(&ht_newentry->key); \
  copy_location(&ht_newentry->value); \
}

/* Copy the actual data */

static Gvainfo copyhash(old,new)
     GenHTable *old, *new;
{
  HashTable *oldtab, *newtab;

  oldtab = old->table;

  CopyHashTable(newtab,oldtab,HashAllocate,HashElemCopy)

  if (newtab == NULL)
    FatalError("Could not malloc() for copy of hash table");

  new->table = newtab;

  return (Gvainfo) new;
}


/* ------------------------------------------------------------------------ */

#define HashElemGc \
{ \
  ht_newentry->key = ht_entry->key; \
  ht_newentry->value = ht_entry->value; \
  gc_location(&ht_newentry->key,gcst); \
  gc_location(&ht_newentry->value,gcst); \
}

/* GC: Don't copy if allocated with malloc() */

static Gvainfo gchash(old,new,gcst)
     GenHTable *old, *new;
     gcstatep gcst;
{
  HashTable *oldtab, *newtab;

  oldtab = old->table;

  CopyHashTable(newtab,oldtab,HashAllocate,HashElemGc)

  new->table = newtab;

  return (Gvainfo) new;
}


static bool unifyhash(x0,x1,andb,exs)
     Term x0, x1;
     andbox *andb;
     exstate *exs;
{
  if (Eq(x0, x1))		/* Not as simple as I thought */
    return TRUE;		/* so just test for equality */
  return FALSE;
}


static int printhash(file,table,tsiz)
     FILE *file;
     GenHTable *table;
     int tsiz;
{
/*  fprintf(file,"{hash_table[%d]}",table->table->size);*/
  char s[2];

  fprintf(file, "{hash_table: ");
  s[0] = 0;
  s[1] = 0;
  HashEntryLoop(table->table,
		{
		  fprintf(file, "%s(", s);
		  fdisplay_term(file, tad_to_term(ht_entry->key), tsiz-1);
		  fputc(',', file);
		  fdisplay_term(file, tad_to_term(ht_entry->value), tsiz-1);
		  fputc(')', file);
		  s[0] = ',';
		});
  fputc('}', file);

  return 1;
}


/* ------------------------------------------------------------------------ */
/* LOW LEVEL OBJECT SEND METHOD */

static Atom atom_nil;
static Atom atom_yes;
static Atom atom_no;
static Atom atom_hash_table;
static Functor functor_typeof;
static Functor functor_sizeof;
static Functor functor_indomain_2;
static Functor functor_get_2;
static Functor functor_set_2;
static Functor functor_set_3;
static Functor functor_delete_1;
static Functor functor_delete_2;
static Functor functor_domain_1;
static Functor functor_range_1;
static Functor functor_attributes_1;
static bool method_indomain_hash();
static bool method_get_hash();
static bool method_set_hash();
static bool method_swap_hash();
static bool method_delete_hash();
static bool method_domain();
static bool method_range();
static bool method_attributes();


static bool sendhash(message, self, exs)
     Term message, self;
     exstate *exs;
{
  Deref(message, message);

  if(IsSTR(message))
    {
      Functor op = StrFunctor(Str(message));
      Term X0, X1, X2;

      if (op == functor_indomain_2)
	{
	  GetStrArg(X0,Str(message),0);
	  GetStrArg(X1,Str(message),1);
	  return method_indomain_hash(X0,X1,self,exs);
	}
      else
	if (op == functor_get_2)
	  {
	    GetStrArg(X0,Str(message),0);
	    GetStrArg(X1,Str(message),1);
	    return method_get_hash(X0,X1,self,exs);
	  }
	else
	  if(op == functor_set_2)
	    {
	      GetStrArg(X0,Str(message),0);
	      GetStrArg(X1,Str(message),1);
	      return method_set_hash(X0,X1,self,exs);
	    }
	  else
	    if(op == functor_set_3)
	      {
		GetStrArg(X0,Str(message),0);
		GetStrArg(X1,Str(message),1);
		GetStrArg(X2,Str(message),2);
		return method_swap_hash(X0,X1,X2,self,exs);
	      }
	    else
	      if(op == functor_delete_1)
		{
		  GetStrArg(X0,Str(message),0);
		  return method_delete_hash(X0,NullTerm,self,exs);
		}
	      else
		if(op == functor_delete_2)
		  {
		    GetStrArg(X0,Str(message),0);
		    GetStrArg(X1,Str(message),1);
		    return method_delete_hash(X0,X1,self,exs);
		  }
		else
		  if(op == functor_domain_1)
		    {
		      GetStrArg(X0,Str(message),0);
		      return method_domain(self,X0,exs);
		    }
		  else
		    if(op == functor_range_1)
		      {
			GetStrArg(X0,Str(message),0);
			return method_range(self,X0,exs);
		      }
		    else
		      if(op == functor_attributes_1)
			{
			  GetStrArg(X0,Str(message),0);
			  return method_attributes(self,X0,exs);
			}
		      else
			if(op == functor_typeof)
			  {
			    GetStrArg(X0,Str(message),0);
			    Deref(X0,X0);
			    return unify(X0, TagAtm(atom_hash_table), exs->andb, exs);
			  }
			else
			  if(op == functor_sizeof)
			    {
			      Term tmp;

			      GetStrArg(X0,Str(message),0);
			      MakeIntegerTerm(tmp, Hash(self)->table->size);
			      Deref(X0,X0);
			      return unify(X0, tmp, exs->andb, exs);
			    }

      METHOD_EXISTENCE_ERROR(message, self)
    }

  IfVarSuspend(message);
  METHOD_EXISTENCE_ERROR(message, self)
}

/* ######################################################################## */

/* These are used if in close list */

static int uncopiedhash(gtable)
     GenHTable *gtable;
{
  return 0;
}

static envid *deallocatehash(gtable)
     GenHTable *gtable;
{
  free(gtable->table);
  return NULL;
}

static int killhash(gtable)
     GenHTable *gtable;
{
  free(gtable->table);
  return 1;
}

/* ######################################################################## */

/* hash(+GenHTable) */

static bool akl_hash_table(Arg)
     Argdecl;
{
  Term X0;
  Deref(X0,A(0));

  if(IsGenHTable(X0))
    return TRUE;
  IfVarSuspend(X0);
  return FALSE;
}

/* ######################################################################## */

/* hash_value(+Key,-Value) */

static bool akl_hash_value(Arg)
     Argdecl;
{
  Term Key, Value, X;
  uword hv = 0;
  bool err;

  Deref(Key,A(0));

  err = ground_hash_func(exs, Key, &hv);

  if (err != TRUE)
    return err;			/* suspend */

  MakeIntegerTerm(X, hv);
  Deref(Value,A(1));

  return unify(Value,X,exs->andb,exs);
}

/* ######################################################################## */

static bool new_hash_table(exs,Size,New)
     exstate *exs;
     Term Size, New;
{
  int size;
  GenHTable *new;
  Term tmpv;

  if (Eq(Size, NullTerm))
    size = DEFAULT_TABLE_SIZE;
  else
    {
      Deref(Size,Size);		/* The size of the new hash */
      IfVarSuspend(Size);

      if (!IsINT(Size) || CompareIntegers(<, Size, MakeSmallNum(1)))
	DOMAIN_ERROR("new_hash_table", 2, 1, "positive integer")
      else if (!IsMachineInteger(Size))
	REPRESENTATION_ERROR("new_hash_table", 2, 1, "exceeds maximum")
      else
	size = GetInteger(Size);
    }

  MakeGvainfo(new,GenHTable,&hashmethods,exs->andb);

  NewHashTable(new->table,size,HashAllocate);

  if (new->table == NULL)
    SYSTEM_ERROR("new_hash_table/[1,2]: unable to allocate memory")

  if (SizeOfHashTable(size) > HLIMIT)
    add_gvainfo_to_close((Gvainfo)new,exs);

  Deref(New,New);
  MakeCvaTerm(tmpv, (Gvainfo)new);
  return unify(New,tmpv,exs->andb,exs);
}

/* ------------------------------------------------------------------------ */


static bool akl_new_hash_table_1(Arg)
     Argdecl;
{
  return new_hash_table(exs,NullTerm,A(0));
}


static bool akl_new_hash_table_2(Arg)
     Argdecl;
{
  return new_hash_table(exs,A(0),A(1));
}

/* ######################################################################## */

/* indomain(+Key, -YesOrNo)@ +GenHTable */

static bool method_indomain_hash(Key, Value, Self, exs)
     Term Key, Value, Self;
     exstate *exs;
{
  HashTable *table;
  GenHTable *gen;
  uword val;
  Atom res = atom_yes;
  Term A[2];

  A[0] = Key;
  A[1] = Value;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  Deref(Key, Key);

  gen = Hash(Self);
  table = gen->table;

  HashFind(table, Key, val, GHashValue("indomain", 2, 1, Self),
	   {
	     compres_val cres;
	     ht_result = ((akl_compare_aux(ht_key1,ht_key2,exs,&cres) == TRUE)
			  && (cres == CMP_EQUAL));
	   },
	   {
	     res = atom_no;
	     goto ending;
	   })

 ending:
  Deref(Value, Value);
  return unify(Value,TagAtm(res),exs->andb,exs);
}

/* ######################################################################## */

/* get(+Key, -Term)@ +GenHTable */

static bool method_get_hash(Key, Value, Self, exs)
     Term Key, Value, Self;
     exstate *exs;
{
  HashTable *table;
  GenHTable *gen;
  uword val;
  Term A[2];

  A[0] = Key;
  A[1] = Value;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  Deref(Key, Key);

  gen = Hash(Self);
  table = gen->table;

  HashFind(table, Key, val, GHashValue("get", 2, 1, Self),
	   {
	     compres_val cres;
	     ht_result = ((akl_compare_aux(ht_key1,ht_key2,exs,&cres) == TRUE)
			  && (cres == CMP_EQUAL));
	   },
	   {return FALSE;})

  Deref(Value, Value);
  return unify(Value,tad_to_term(val),exs->andb,exs);
}

/* ######################################################################## */

/* set(+Key, +Term)@ +GenHTable */

static bool method_set_hash(Key, Value, Self, exs)
     Term Key, Value, Self;
     exstate *exs;
{
  HashTable *table;
  GenHTable *gen;
  Term A[2];

  A[0] = Key;
  A[1] = Value;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  Deref(Key, Key);

  gen = Hash(Self);
  table = gen->table;

  HashEnter(table, Key, Value, GHashValue("set", 2, 1, Self),
	    {
	      compres_val cres;
	      ht_result = ((akl_compare_aux(ht_key1,ht_key2,exs,&cres) == TRUE)
			   && (cres == CMP_EQUAL));
	    },
	    HashAllocate,
	    HashFree)

  gen->table = table;		/* Could have grown and deallocated */

  return TRUE;
}

/* ######################################################################## */

/* set(+Key, -OldTerm, +Term)@ +GenHTable */

static bool method_swap_hash(Key, OldValue, Value, Self, exs)
     Term Key, OldValue, Value, Self;
     exstate *exs;
{
  HashTable *table;
  GenHTable *gen;
  HashEntry *entry;
  uword hval = 0;
  bool err;
  Term A[3];

  A[0] = Key;
  A[1] = OldValue;
  A[2] = Value;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  Deref(Key, Key);

  gen = Hash(Self);
  table = gen->table;

  {
    err = ground_hash_func(exs, Key, &hval);

    if (err == FALSE)
      METHOD_DOMAIN_ERROR("set", 3, 1, "pure tree", Self)
    else if (err != TRUE)
      return err;		/* suspend */

    HashLookupLoop(table, Key, hval, entry,
		   {
		     compres_val cres;
		     ht_result = 
		       ((akl_compare_aux(ht_key1,ht_key2,exs,&cres) == TRUE)
			&& (cres == CMP_EQUAL));
		   },
		   {},
		   {return FALSE;});
  }

  Deref(OldValue, OldValue);

  if (!unify(OldValue,tad_to_term(entry->value),exs->andb,exs))
    return FALSE;

  entry->value = (uword)Value;	/* destructive set value */
  return TRUE;
}

/* ######################################################################## */

/* delete(+Key)@ +GenHTable */
/* delete(+Key, -Term)@ +GenHTable */

static bool method_delete_hash(Key, Value, Self, exs)
     Term Key, Value, Self;
     exstate *exs;
{
  HashTable *table;
  GenHTable *gen;
  HashEntry *entry;
  uword hval = 0;
  bool err;
  Term A[2];

  A[0] = Key;
  A[1] = Value;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  Deref(Key, Key);

  gen = Hash(Self);
  table = gen->table;

  {
    err = ground_hash_func(exs, Key, &hval);

    if (err == FALSE)
      METHOD_DOMAIN_ERROR("delete", 2, 1, "pure tree", Self)
    else if (err != TRUE)
      return err;		/* suspend */

    HashLookupLoop(table, Key, hval, entry,
		   {
		     compres_val cres;
		     ht_result = 
		       ((akl_compare_aux(ht_key1,ht_key2,exs,&cres) == TRUE)
			&& (cres == CMP_EQUAL));
		   },
		   {},
		   {return FALSE;});
  }

  if (!Eq(Value, NullTerm))
    {
      Deref(Value, Value);
      if (!unify(Value,tad_to_term(entry->value),exs->andb,exs))
	return FALSE;
    }

  entry->key = DELETED_HASH_ENTRY;	/* clear entry */

  return TRUE;
}

/* ######################################################################## */

/* domain(-DomainList)@Self */

static bool method_domain(Self,X0,exs)
     Term Self,X0;
     exstate *exs;
{
  HashTable *table;
  Term lst = NIL;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  table = Hash(Self)->table;

  HashEntryLoop(table,
		{
		  Term l;

		  MakeListTerm(l,exs->andb);
		  LstCdr(Lst(l)) = lst;
		  lst = l;
		  LstCar(Lst(lst)) = tad_to_term(ht_entry->key);
		});

  Deref(X0, X0);
  return unify(X0,lst,exs->andb,exs);
}

/* ######################################################################## */

/* range(-RangeList)@Self */

static bool method_range(Self,X0,exs)
     Term Self,X0;
     exstate *exs;
{
  HashTable *table;
  Term lst = NIL;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  table = Hash(Self)->table;

  HashEntryLoop(table,
		{
		  Term l;

		  MakeListTerm(l,exs->andb);
		  LstCdr(Lst(l)) = lst;
		  lst = l;
		  LstCar(Lst(lst)) = tad_to_term(ht_entry->value);
		});

  Deref(X0, X0);
  return unify(X0,lst,exs->andb,exs);
}

/* ######################################################################## */

static Functor functor_comma_2;

/* attributes(-AttributeList)@Self */

static bool method_attributes(Self,X0,exs)
     Term Self,X0;
     exstate *exs;
{
  HashTable *table;
  Term lst = NIL;

  /* Self is already dereferenced and checked */

  if(!IsLocalGVA(Ref(Self),exs->andb))
    return SUSPEND;

  table = Hash(Self)->table;

  HashEntryLoop(table,
		{
		  Term l;
		  Structure s;

		  MakeStruct(s,functor_comma_2,exs->andb);
		  StrArgument(s,0) = tad_to_term(ht_entry->key);
		  StrArgument(s,1) = tad_to_term(ht_entry->value);

		  MakeListTerm(l,exs->andb);
		  LstCdr(Lst(l)) = lst;
		  lst = l;
		  LstCar(Lst(lst)) = TagStr(s);
		});

  Deref(X0, X0);
  return unify(X0,lst,exs->andb,exs);
}

/* ######################################################################## */

void initialize_hash() {

  atom_nil = store_atom("[]");
  atom_yes = store_atom("yes");
  atom_no = store_atom("no");
  atom_hash_table = store_atom("hash_table");

  functor_typeof = store_functor(store_atom("typeof"),1);
  functor_sizeof = store_functor(store_atom("sizeof"),1);

  functor_indomain_2 = store_functor(store_atom("indomain"),2);
  functor_get_2 = store_functor(store_atom("get"),2);
  functor_set_2 = store_functor(store_atom("set"),2);
  functor_set_3 = store_functor(store_atom("set"),3);
  functor_set_3 = store_functor(store_atom("set"),3);
  functor_delete_1 = store_functor(store_atom("delete"),1);
  functor_delete_2 = store_functor(store_atom("delete"),2);
  functor_domain_1 = store_functor(store_atom("domain"),1);
  functor_range_1 = store_functor(store_atom("range"),1);
  functor_attributes_1 = store_functor(store_atom("attributes"),1);
  functor_comma_2 = store_functor(store_atom(","),2);
  functor_cons = store_functor(store_atom("."),2);

  define("new_hash_table",akl_new_hash_table_1,1);
  define("new_hash_table",akl_new_hash_table_2,2);
  define("hash_table",akl_hash_table,1);
  define("hash_value",akl_hash_value,2);
}
