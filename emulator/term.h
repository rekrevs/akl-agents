/* $Id: term.h,v 1.52 1994/05/27 09:17:58 bd Exp $ */

/* Tagged pointer scheme.
 *
 * Currently we use primary and secondary tags in low bits, small integers
 * in pointers, and one high GC mark bit.
 *
 * The layout looks like this for machines with 32-bit words:
 *
 * Reference:				0:::::::::::::::::::::::::::::00
 * Unbound unconstrained variable:	0:::::::::::::::::::::::::::::01
 * Unbound constrained variable:	0:::::::::::::::::::::::::::::11
 * List cell:				0:::::::::::::::::::::::::::0010
 * Structure:				0:::::::::::::::::::::::::::1010
 * Generic, float, or bignum:		0:::::::::::::::::::::::::::1110
 * Small integer:			0::::::::::::::::::::::::::00110
 * Atom:				0::::::::::::::::::::::::::10110
 *
 * Pointers with 4-bit tags need to be downshifted before they can be used.
 * Atoms are currently double-word aligned pointers, but could just as well
 * be atom numbers.
 */


/* "sysdeps.h" contains definitions that are ifdef'ed by the system type.
 * Some of those definitions override definitions in this file.
 */
#include "sysdeps.h"


/* #define TADTURMS will define "Term" to be a "struct turm" which
 * encapsulates the tagged pointer. If you leave TADTURMS undefined,
 * "Term" will be an integer type. This allows for more optimizations,
 * but the struct representation will let the compiler catch more
 * errors, such as attempting to cast a Term into a pointer.
 */

/* #define TADTURMS */


/* #define INDIRECTVARS to disable having one-word variables implicit in the
 * arguments of lists and structures. If you define INDIRECTVARS one-word
 * variables will still be created, but always as separate objects
 * in the heap (there will be no pointers into structures).
 */

/* #define INDIRECTVARS */


/* #define STRUCT_ENV to include an environment field in all structures.
 * This is used when copying, so that copying of non-local structures is
 * avoided. The environment field is located in the word preceding the
 * structure or list.
 */

/* #define STRUCT_ENV */


typedef unsigned long uword;
typedef long sword;

typedef uword tagged_address;

/* TADBITS is the number of bits in a tagged address word.
 * You must change this and the bitmasks below, if you have a different
 * word length than 32 bits (like on the DEC Alpha).
 */

#ifndef TADBITS
#define TADBITS 32
#endif

#ifndef PTagMask
#define PTagMask	((uword)0x00000003)
#endif

/* The assumed alignment of heap pointers: */
#ifndef WORDALIGNMENT
#define WORDALIGNMENT	(TADBITS/8)
#endif

#define STagMask	((uword)0x0000000f)
#define IMM_ATM_BIT	((uword)0x10)
#define ITagMask	(STagMask + IMM_ATM_BIT)
#define SMALLNUMBITS	(TADBITS-6)

/* PTR_ORG is a base address for pointers, which can be subtracted in order
 * to get rid of high address bits that clash with the tag scheme.
 * PTR_ORG is normally zero, but it needs to be defined for
 * some machines (for example the HP-PA series).
 */

#ifndef PTR_ORG
#define PTR_ORG 0
#endif

#define AddressMask	~PTagMask
#define MarkBitMask	((uword)0x1 << (TADBITS-1))

#define TagOffset(T)	((tagged_address)(T))
#define CollectPTagBits(x)	((x)&PTagMask)
#define CollectSTagBits(x)	((x)&STagMask)

/* This is the "official" tagged pointer type */

#ifdef TADTURMS
typedef struct turm { tagged_address tad; } Turm;
#define Tad(X)		((X).tad)
#define tad_to_term(A)	tad_to_turm(A)
extern Turm tad_to_turm PROTO((tagged_address A));
typedef Turm Term;
#else
typedef tagged_address Term;
#define Tad(X)		(X)
#define tad_to_term(A)	(A)
#endif

#define PTagOf(x)	CollectPTagBits((tagged_address)Tad(x))
#define STagOf(x)	CollectSTagBits((tagged_address)Tad(x))
#define AddressOf(x)	(((tagged_address)Tad(x))&AddressMask)


#define Eq(X, Y)	(Tad(X) == Tad(Y))

typedef Term *Reference;

typedef struct atom 	*Atom;
typedef struct fp 	*Fp;
typedef struct bigstub 	*Bigstub;
typedef struct list 	*List;
typedef struct structure *Structure;
typedef struct generic	*Generic;
typedef struct gvainfo	*Gvainfo;
typedef struct functor  *Functor;

#ifndef Bignum
#define Bignum	Bigstub
#endif

#define DEFINE_TAG(N)	(N)

#define OTH DEFINE_TAG(2)

typedef enum {
    REF = DEFINE_TAG(0),
    UVA = DEFINE_TAG(1),
    GVA = DEFINE_TAG(3),
    IMM = DEFINE_TAG(6),
    LST = DEFINE_TAG(2),
    STR = DEFINE_TAG(0xa),
    GEN = DEFINE_TAG(0xe)
} tval;

/* For the scavenger */
#define REVERSED_LST	DEFINE_TAG(0)
#define REVERSED_STR	DEFINE_TAG(1)

#define Oth(X)		((X) >> 2)
#define Tho(X)		((X) << 2)
#define OthTad(X)	Oth(Tad(X))
#define OthTagOffset(T)	(Oth(TagOffset(T)) - PTR_ORG)
#define PTR_ATM_BIT	Oth(IMM_ATM_BIT)
#define AddressOth(X)	(AddressOf(Oth(X)) + PTR_ORG)

/* This is a tag word used in Bignum and Float objects.
 * It is needed in order to reserve a mark bit for forwarding-pointers
 * during gc. It is also overlayed with the method table pointer in
 * generic objects, due to tag overloading.
 */
#define BIG_TAG2	((uword)0x00000000)
#define FLT_TAG2	((uword)0x00000004)
/* Method tables must be above this value: */
#define GEN_TAG2	((uword)0x00000008)

#define TadPtr(X, T)	(((tagged_address)(X))+TagOffset(T))
#define TadOth(X, T)	(Tho((tagged_address)(X))-Tho(PTR_ORG)+TagOffset(T))

#define code_to_tad(c)	((tagged_address)(c))

#define TagPtr(X, T)	tad_to_term(TadPtr(X, T))
#define TagOth(X, T)	tad_to_term(TadOth(X, T))

#define swordNum(N)	(PTR_ORG + (((uword)(((sword)(N)) << 6)) >> 3))

#define TagAtm(A)	(TagOth(A, IMM) + IMM_ATM_BIT)
#define TagNum(I)	TagOth(I, IMM)
#define TagRef(X)	TagPtr(X, REF)
#define TagUva(X)	TagPtr(X, UVA)
#define TagGva(X)	TagPtr(X, GVA)
#define TagBig(X)	TagOth(X, GEN)
#define TagFlt(X)	TagOth(X, GEN)
#define TagLst(X)	TagOth(X, LST)
#define TagStr(X)	TagOth(X, STR)
#define TagGen(X)	TagOth(X, GEN)


#define code_to_term(c)	tad_to_term(code_to_tad(c))
#define term_to_code(X)	((code)(Tad(X)))


/* Macros for handling of marking of forward pointers and
 * for the threaded stack scavenger in gc and copy.
 * [These really deserve a header file of their own!]
 */
#define ForeWord(A)	(*(tagged_address*)(A))
#define ForwOf(FW)	((void*)((FW)-MarkBitMask))
#define IsForwMarked(FW) (((tagged_address)(FW)) & MarkBitMask)

#define IsCopied(X)	IsForwMarked(ForeWord(X))

#define Link(Old,New) \
	(*(tagged_address*)(Old)) = (tagged_address)(New) + MarkBitMask

#define Forw(Old)	ForwOf(ForeWord(Old))

#define ScavMark(X)	tad_to_term((tagged_address)Tad(X) + MarkBitMask)

#define ScavUnMark(X)	tad_to_term((tagged_address)Tad(X) - MarkBitMask)

#define IsScavMarked(X)	((tagged_address)Tad(X) & MarkBitMask)

#define Home(E)    ((andbox *)(E - OffsetToEnv))

#define NewEnv(E)  (envid*)&((Home(E)->previous->env))

#define InCopyEnv(E) (Home(E)->father == NULL)


/* Type-test and access macros for terms */

#define IMMtermIsATM(I)	((((tagged_address)Tad(I)) & IMM_ATM_BIT) != 0)
#define IMMtermIsNUM(I)	((((tagged_address)Tad(I)) & IMM_ATM_BIT) == 0)


#define VarIsUVA(V)	IsUVA(RefTerm(V))
#define VarIsGVA(V)	IsGVA(RefTerm(V))

#define IsREF(V) (PTagOf(V) == REF)
#define IsUVA(V) (PTagOf(V) == UVA)
#define IsGVA(V) (PTagOf(V) == GVA)
#define IsIMM(V) (STagOf(V) == IMM)
#define IsNUM(V) (IsIMM(V) && IMMtermIsNUM(V))
#define IsATM(V) (IsIMM(V) && IMMtermIsATM(V))
#define IsBIG(V) (STagOf(V) == GEN && (uword)Gen(V)->method == BIG_TAG2)
#define IsFLT(V) (STagOf(V) == GEN && (uword)Gen(V)->method == FLT_TAG2)
#define IsLST(V) (STagOf(V) == LST)
#define IsSTR(V) (STagOf(V) == STR)
#define IsGEN(V) (STagOf(V) == GEN && (uword)Gen(V)->method >= GEN_TAG2)


#define IsVar(V) IsREF(V)
#define IsNonVar(V) (!IsVar(V))

#define IsCompound(A) (IsLST(A) || IsSTR(A))
#define IsAtomic(A) (IsIMM(A) || IsBIG(A) || IsFLT(A))

/* Multi-way switches */

#define SwitchTag(X,uva,gva,imm,big,flt,lst,str,gen) \
{\
  if (!IsREF(X)) {\
    if (IsIMM(X)) goto imm;\
    if (IsLST(X)) goto lst;\
    if (IsSTR(X)) goto str;\
    if (IsUVA(X)) goto uva;\
    if (IsGVA(X)) goto gva;\
    if (IsBIG(X)) goto big;\
    if (IsFLT(X)) goto flt;\
    if (IsGEN(X)) goto gen;\
  }\
}

#define InternalDeref(X, DOIFVAR) \
{\
  for(;;) {\
    if (IsREF(X)) {\
      register Reference ZZZr = Ref(X);\
      if (IsUnBoundRef(ZZZr)) { DOIFVAR; }\
      (X) = RefTerm(ZZZr);\
    } else break;\
  }\
}

#define DerefJumpIfVar(X,VARCASE) InternalDeref(X, goto VARCASE)

#define Deref(Xderef, X) \
{\
  register Term ZZZv = (X);\
  InternalDeref(ZZZv, break);\
  (Xderef) = ZZZv;\
}

#define DerefSwitchTerm(X,refgva,small,atm,big,flt,cons,str,gen) \
{\
  register Term ZZZt = (X);\
  InternalDeref(ZZZt, break);\
  (X) = ZZZt;\
  if (!IsREF(ZZZt)) {\
    if (IsIMM(ZZZt)) {\
      if (IMMtermIsNUM(ZZZt)) goto small;\
      else goto atm;\
    }\
    if (IsLST(ZZZt)) goto cons;\
    if (IsSTR(ZZZt)) goto str;\
    if (IsBIG(ZZZt)) goto big;\
    if (IsFLT(ZZZt)) goto flt;\
    if (IsGEN(ZZZt)) goto gen;\
  } else if (VarIsGVA(Ref(ZZZt))) goto refgva;\
}

#define Ref(R)		((Reference) (Tad(R)-TagOffset(REF)))
#define Env(H)		((envid*) (Tad(H)-TagOffset(UVA)))
#define Gva(C)		((gvainfo*) (Tad(C)-TagOffset(GVA)))
#define Atm(A)		((Atom) (OthTad(A)-(OthTagOffset(IMM)+PTR_ATM_BIT)))
#define Num(I)		(OthTad(I)-OthTagOffset(IMM))
#define Big(B)		((Bignum) (OthTad(B)-OthTagOffset(GEN)))
#define Flt(F)		((Fp) (OthTad(F)-OthTagOffset(GEN)))
#define Lst(L)		((List) (OthTad(L)-OthTagOffset(LST)))
#define Str(F)		((Structure) (OthTad(F)-OthTagOffset(STR)))
#define Gen(G)		((Generic) (OthTad(G)-OthTagOffset(GEN)))

#define RefSize		sizeof(Term)
#define RefTerm(R)	(*(R))
#define RefEnv(R)	(VarIsUVA(R) ? Env(RefTerm(R)) : GvaEnv(Gva(RefTerm(R))))
#define RefGva(R)	Gva(RefTerm(R))

#define GvaEnv(G)	((G)->env)
#define GvaSva(G)	((svainfo*)(G))
#define SvaSize		(sizeof(svainfo))
#define SvaEnv(S)	((S)->env)
#define SvaSusp(S)	((S)->susp)

#define AtmSize 	sizeof(struct atom)
#define AtmPname(X) 	((X)->pname)
#define AtmHash(X) 	((X)->hashval)

#define NumVal(I)	((((sword)(I))<<3)>>6)

#define FltSize 	sizeof(struct fp)
#define FltVal(F)	((F)->val)

#define MaxSmallNum	(1UL << (SMALLNUMBITS-1))

#define GetSmall(X)	NumVal(Num(X))
#define MakeSmallNum(I)	TagNum(swordNum(I))

#define IsINT(X)	(IsNUM(X) || IsBIG(X))

#define GetInteger(X)	(IsNUM(X) ? GetSmall(X) : get_bignum_integer(X))
#define IsMachineInteger(X) (IsNUM(X) || is_machine_integer(X))

#define GetUnsigned(X)	(IsNUM(X)? (uword)GetSmall(X) : get_bignum_unsigned(X))
#define IsMachineUnsigned(X) \
    (IsNUM(X) ? GetSmall(X) >= 0 : is_machine_unsigned(X))

#define IntIsSmall(I)	((I) >= -MaxSmallNum && (I) < MaxSmallNum)

#define CompareIntegers(Op, X, Y) \
    (IsNUM(X) && IsNUM(Y) \
     ?  GetSmall(X) Op GetSmall(Y) \
     :  bignum_compare((X), (Y)) Op 0)

/* The following is for comparing constants in switch tables.
 * Atoms, small numbers, and functors (which look like References)
 * are sorted by address. Bignums are higher than everything else.
 * Floats are not handled.
 */

#define CompareConstants(X, Y) \
    (IsBIG(X) \
     ?  (IsBIG(Y) ? bignum_compare((X), (Y)) : 1) \
     :  (IsBIG(Y) ? -1 : ThreeWayCompare(Tad(X), Tad(Y))))

#define ThreeWayCompare(X, Y) ((X) < (Y) ? -1 : ((X) == (Y) ? 0 : 1))

extern sword	get_bignum_integer PROTO((Term X));
extern bool	is_machine_integer PROTO((Term X));
extern uword	get_bignum_unsigned PROTO((Term X));
extern bool	is_machine_unsigned PROTO((Term X));
extern int	bignum_compare PROTO((Term X, Term Y));

/* Compatibility... */
#if 0
#define IntVal(X)	GetInteger(X)
#define Int(X)		(X)
#endif

#define NIL		term_nil

#define NullTerm	TagPtr(0, 0)

/* Note: ArgOf evaluates R twice! */
#define ArgOf(R)	(IsUnBoundRef(R) ? TagRef(R) : RefTerm(R))

#define GetArg(X,R) \
{\
  register Term *ZZZga = (R);\
  (X) = ArgOf(ZZZga);\
}

#define LstSize 	(sizeof(struct list) + ENV_OFFSET)
#define LstCar(L)	((L)->car)
#define LstCdr(L)	((L)->cdr)

#define LstCarRef(L)    &((L)->car)
#define LstCdrRef(L)    &((L)->cdr)

#define GetLstCar(X,L)	GetArg(X,LstCarRef(L))
#define GetLstCdr(X,L)	GetArg(X,LstCdrRef(L))

#define FnkSize 	sizeof(struct functor)
#define FnkArity(F) 	((F)->arity)
#define FnkName(F) 	((F)->name)
#define FnkPname(F) 	((F)->name->pname)

/* New macros for functors */
#define FnkNums(ex_func)      ((ex_func)->nums)
#define FnkFeatures(ex_func)  ((ex_func)->features)
#define FnkLength(ex_func)    ((ex_func)->length)
#define FnkHash(ex_func)      ((ex_func)->entries)
#define FnkNames(ex_func)     (Term*)(&((ex_func)-> \
			           entries[(ex_func)->length]))

#define StrSize(Arity) \
	(sizeof(struct structure)+((Arity) - ANY)*sizeof(Term) + ENV_OFFSET)

#define StrFunctor(S) 	((S)->functor)
#define StrArity(S) 	((S)->functor->arity)
#define StrName(S) 	((S)->functor->name)
#define StrPname(S)	((S)->functor->name->pname)
#define StrArgument(S,N) ((S)->arg[N])
#define StrArgRef(S,N)  &((S)->arg[N])
#define GetStrArg(X,S,N) GetArg(X,StrArgRef(S,N))

/* New macros for functors */
#define StrNums(str)     ((str)->functor->nums)
#define StrFeatures(str) ((str)->functor->features)
#define StrLength(str)   ((str)->functor->length)
#define StrHash(str)     ((str)->functor->entries)
#define StrNames(str)    (Term*)(&((str)->functor-> \
			       entries[(str)->functor->length]))

/* NOTE: this depends on the values of the UVA and GVA tags! */
#define IsUnBoundRef(V) (Tad(RefTerm(V)) & 0x1)

#define IsLocalEnv(E,Ab) ( (E) == (envid*)&((Ab)->env) )

#define CheapIsLocalUVA(V,Ab) Eq(RefTerm(Ref(V)),TagUva(&((Ab)->env)))

/* #define IsLocalUVA(V,Ab) IsLocalEnv(Env(RefTerm(V)),Ab) */
#define IsLocalUVA(V,Ab) CheapIsLocalUVA(V,Ab)
#define IsLocalGVA(V,Ab) IsLocalEnv(GvaEnv(RefGva(V)),Ab)


/* [BD] This is actually never used. */
#define IsExternal(V,Ab) (!IsLocal(V,Ab))


/* operations on terms */

/* #define Bind(X,Y)	RefTerm((Reference)(X)) = (Y) */
#define Bind(X,Y)	RefTerm(X) = (Y)

#if 0
/* For parallel implementation only? (I.e. not needed for GVAs?) */
#define BindCheck(X,Y,OnFail) \
{\
  RefTerm(X) = (Y); \
  if (!Eq(RefTerm(X), Y)) { \
    OnFail; \
  } \
}
#else
#define BindCheck(X,Y,OnFailDummy)	RefTerm(X) = (Y)
#endif

#define UnBindRef(X,U)	RefTerm((Reference)(X)) = (U)

#define InitReference(V,Ab) \
{\
  RefTerm(V) = TagUva((envid*)&((Ab)->env));\
}

#define InitSvainfo(S,Ab) \
{\
  (S)->method = NULL;\
  (S)->env =  (envid*)&((Ab)->env);\
  (S)->susp = NULL;\
}

#define DerefUvaEnv(V)\
{\
  register envid *I;\
  for(I = Env(RefTerm(V)); I->env != NULL; I = I->env);\
  RefTerm(V) = TagUva(I);\
}

#define DerefGvaEnv(S)\
{\
  register envid *I;\
  for(I = (S)->env; I->env != NULL; I = I->env);\
  (S)->env = I;\
}

#define DerefVarEnv(V) \
  if (VarIsUVA(V)) {\
    DerefUvaEnv(V);\
  } else {\
    DerefGvaEnv(RefGva(V));\
  }

#define DerefGenEnv(G)\
{\
  register envid *I;\
  for(I = (G)->env; I->env != NULL; I = I->env);\
  (G)->env = I;\
}

#ifdef STRUCT_ENV
#define StructEnvRef(S)	(((envid*)(S))-1)
#define InitStructEnv(S,Env)	(StructEnvRef(S)->env = (Env))
#define DerefStrEnv(S)	DerefGenEnv(StructEnvRef(S))
#define ENV_OFFSET	sizeof(envid)
#else
#define InitStructEnv(S,Env)	/* Nothing */
#define ENV_OFFSET	0
#endif

typedef struct envid {
    struct envid *env;
} envid;

typedef struct fp {
  uword		tag2;
  double 	val;
} fp;

typedef struct bigstub {
  uword		tag2;
} bigstub;

typedef struct atom {
  char 		*pname;
  int		hashval;
} atom;

typedef struct list {
  Term		car;
  Term 		cdr;
} list;



/*
 * GENERIC OBJECT METHODS
 *
 
  new:		Called by GC and copy.  This allocates a template
		during copying, to which the original is linked to allow
		copying of circular structures.  Copying of contents is
		done by differently by copy and GC.

  unify:	Called on unification.

  print:	Prints out a character string representation.

  copy:		Copy (for nondeterministic promotion) in such a way that it
		can be restored if necessary (by reset).  It should use the
		copy-procedures for copying terms.

  gc:		Copy for GC, using the gc-procedures for terms.

The following methods need only be defined by objects that enter
themselves on a close-list, which is inspected after copy and GC.

  uncopied:	Notifies object that it has not been copied by nondeterministic
		promotion.  (Currently not used to its proper potential
		by port-type objects.)

  deallocate:	Notifies object that it is no longer referenced (after GC).
		It may choose to perform its closing-action at this point,
		if no variables are affected.  To do this, the method should
		return a non-NULL environment pointer identifying a live
		andbox. The close-method will then be scheduled for execution
		in this andbox as a builtin operation on the generic object
		(and may then bind variables).
		
  kill:		Notifies object that it is no longer referenced (after GC),
		and that it belongs to a "dead" environment, which will not be
		interested in close-actions on variables.  (This could
		be merged with deallocate.)

  close:	Builtin operation called by GC (see deallocate).

*/

typedef struct generic {
  struct method *method;
} generic;

#define GENERIC_METHODS(GTYPE) \
  GTYPE		(*new)(); \
  bool		(*unify)(); \
  int		(*print)(); \
  GTYPE		(*copy)(); \
  GTYPE		(*gc)(); \
  int		(*uncopied)(); \
  envid		*(*deallocate)(); \
  int		(*kill)(); \
  bool		(*close)();

typedef struct method {
  GENERIC_METHODS(Generic)
} method;


/*
 * GENERIC VARIABLE METHODS
 *
 * Variables have all of the methods that generic objects have,
 * plus the following ones:
 *

  send:		Called from engine when sending AKL messages to a
		port-type object.

  copy_external: Called by copy to allow a constraint variable to duplicate
		any suspensions of copied goals.
		
  gc_external:	Called by GC to allow a constraint variable to duplicate
		any suspensions of copied goals.

*/

typedef struct gvainfo {
  struct gvamethod	*method;
  envid 		*env;
} gvainfo;

typedef struct gvamethod {
  GENERIC_METHODS(Gvainfo)
  bool		(*send)();
  void		(*copy_external)();
  void		(*gc_external)();

  void		(*wakeall)();
  void		(*makewake)();
  void		(*makerecall)();
} gvamethod;

/* Special case for Herbrand/Rational Tree constraints */

#define GvaIsSva(x)	((x)->method == NULL)

/* The complementary case is called CVA (Constrained VAriable) */

#define GvaIsCva(x)	(!GvaIsSva(x))

#define VarIsCVA(x)	(VarIsGVA(x) && GvaIsCva(RefGva(x)))

#define IsCvaTerm(x)	(IsVar(x) && VarIsCVA(Ref(x)))

#define RefCvaMethod(V)	(RefGva(V)->method)

typedef struct svainfo {
  struct gvamethod	*method;	/* NOTE: This field is always NULL */
  envid 		*env;
  struct suspension	*susp;
} svainfo;


/*
 * For want of a better place...
 *  - assumes dereferenced arguments
 *  - the macro is syntactically an "if(...) ... else"
 *
 */

#define CheapUnify_ELSE(X,Y,A,R) \
      if(Eq(X,Y)) {\
	(R)=TRUE; \
      } else if(IsREF(X) && IsLocalUVA(Ref(X),A)) { \
	(R)=TRUE; \
	BindCheck(Ref(X),Y,(R)=FALSE); \
      } else if(IsREF(Y) && IsLocalUVA(Ref(Y),A)) { \
	(R)=TRUE; \
	BindCheck(Ref(Y),X,(R)=FALSE); \
      } else

/*
 *  - assumes existence of exs in context
 *  - assumes simple arguments
 *
 */

#define UNIFY(X,Y,R) \
  { \
    CheapUnify_ELSE(X,Y,exs->andb,R) \
      (R) = unify_internal(X,Y,exs->andb,exs); \
  }


/* Functor related stuff... */

typedef struct fnk_hash_entry {
  Term     name;
  int      index;
  struct fnk_hash_entry *next;
} fnk_hash_entry;

#define FeatEntrySize (sizeof(struct fnk_hash_entry))

/*This is a functor that may contain features */
typedef struct functor {
  generic       generic;
  Atom 		name;
  int 		arity;
  int           nums;
  int           features;
  int           length;
  fnk_hash_entry entries[ANY];
} functor ;

typedef struct structure {
  Functor 	functor;
  Term	 	arg[ANY];
} structure;

#define OffsetToArgs	1

/*This is a functor that does not have any arguemnts */
typedef struct functor0 {
  generic       generic;
  Term 		name;
  int           arity;
} functor0 ;

