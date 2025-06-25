/* $Id: bam.h,v 1.8 1993/10/14 18:45:30 bd Exp $ */

#ifdef BAM
#define EmptyTryStack (try_top==try_bottom)
#define DumpConsStack {cons_top=cons_bottom;}
#define EmptyConsStack (cons_bottom==cons_top)

#define PopTryStack(X) { try_top--; X= try_top->code; cons_top=try_top->cons_stack;}
#define PushTryStack(X,Y) {try_top->code=X; try_top->cons_stack=Y; try_top ++;}
#define PushConsStack(X,Y) {cons_top->cons1=X; cons_top->cons2.pseudo_term=Y; cons_top++;}
#define DumpTryStack {try_top=try_bottom;}

#define ZeroOrOneEntryConsStack (EmptyConsStack || (cons_top==cons_bottom + 1))

/* Constraint Stack stuff */

#define ConsTagMask  ((uword) 0x3)

#define Cons_Tag(T)           ((cons_tag) ((uword)T & ConsTagMask))

#define MakeCons_Reg_Ps(Reg)   (Reg<<2 | CONS_TAG_REG_PS)
#define MakeCons_Reg_Var(Reg)  (Reg<<2 | CONS_TAG_REG_VAR)
#define MakeCons_Reg_NonVar(Reg)  (Reg<<2 | CONS_TAG_REG_NONVAR)
#define MakeCons_Test           (CONS_TAG_TEST)

#define ConsGetReg(C)        (indx)((C & ~(ConsTagMask)) >> 2)

typedef enum {
    CONS_TAG_REG_PS   = 0x0,
    CONS_TAG_REG_VAR  = 0x1,
    CONS_TAG_REG_NONVAR = 0x2,
    CONS_TAG_TEST     = 0x3
  } cons_tag;

#define ConsToTerm(T) ((Ref) (T & ~CONS_TAG_TEST))

#define ALIAS_CHECK_FAIL -2
#define ALIAS_DEFAULT -1

#define ShortCutAliasFactor 4

/* pseudo term tags */

#define IsPseudoLST(X)             IsLST(X)
#define IsPseudoSTR(X)             IsSTR(X)
#define IsPseudoIMM(X)             IsIMM(X)
#define IsPseudoREG(X)             IsREF(X)
#define IsPseudoTYPE(X)            IsUVA(X) 
#define IsPseudoLABELED(X)	   IsGVA(X)

#define PSEUDO_VOID	(MAX_AREGS * WORDALIGNMENT)

typedef enum {
    TYPED_T		= 0,
    TYPED_INTEGER	= 1*WORDALIGNMENT,
    TYPED_ATOM		= 2*WORDALIGNMENT,
    TYPED_STRUCT	= 3*WORDALIGNMENT
} bam_typed_type;

#define TagPseudoLST(X)	   	   TagLst(X)
#define TagPseudoSTR(X)	   	   TagStr(X)
#define TagPseudoREG(X)	   	   TagRef(X)
#define TagPseudoTYPE(X)	   TagUva(X)
#define TagPseudoLABELED(X)	   TagGva(X)

#define PseudoImmImm(X)		   ((Term)(X))
#define PseudoRegReg(X)		   ((indx)Ref(X))
#define PseudoTypeType(X)	   ((bam_typed_type)Uva(X))
#define PseudoLabeledReg(X)	   ((indx)(*(pseudo_term*)Gva(X)))
#define PseudoLabeledPseudo(X)	   (*(((pseudo_term*)Gva(X))+1))

#define PseudoGetArity(X)	StrArity(Str(X))

#define EqualVar(A,B)         (A==B)

/* Choicebox stuff */

typedef enum {
  NON_BAM_CHOICE = 0,
  UNCOND_SUSP_CHOICE,
  NONDET_SUSP_CHOICE,
  DEAD_CHOICE
} choiceboxtype;

#define BamMakeChoicebox(Chb,Def,Andb,Type) \
{\
  MakeChoicebox(Chb,Def,Andb);\
  Chb->type=Type;\
}

#define BamMakeChoiceCont(Chb,Pc,Areg,Arity) \
{\
   MakeChoiceCont(Chb,Pc,Areg,Arity);\
}
   
#define MakeRegSuspend(Reg,Chb,Andb) \
{\
   if(VarIsUVA(Ref(Xb(Reg)))) {MakeBigVar(Ref(Xb(Reg)))};\
   MakeChoiceWake(Ref(Xb(Reg)),Chb,Andb);\
   }

#define MakeChoiceWake(V,Chb,Andb) \
{\
    gvainfo *ZZZg = RefGva(V);\
    if (GvaIsSva(ZZZg)) {\
      suspension *susp;\
      svainfo *ZZZs = GvaSva(ZZZg);\
      DerefGvaEnv(ZZZg);\
      NewBamSuspension(susp);\
      susp->type= CHB;\
      susp->suspended.chb=Chb;\
      susp->next=SvaSusp(ZZZs);\
      SvaSusp(ZZZs)= susp;\
      Mark(Andb,Home(SvaEnv(ZZZs)));\
    } else {\
      FatalError("MakeChoiceWake() is only supported for SVAs");\
    }\
}

#define NewBamSuspension(ret)\
{\
    (ret) = (suspension *) heapcurrent; \
    heapcurrent += sizeof(suspension); \
}

#define RevInternalDeref(X, DOIFNOTVAR) \
{\
  for(;;) {\
    if (!IsREF(X)) { DOIFNOTVAR; }\
    else {\
      Register Reference ZZZr = Ref(X);\
      if (IsUnBoundRef(ZZZr)) break;\
      (X) = RefTerm(ZZZr);\
    }\
  }\
}

/* making terms from pseudo-term templates */

#define ToByteOffset(x)	((x)*sizeof(Term))

#define ListPushb(offset) {\
  *(Term*)heapcurrent = TagLst(heapcurrent+(offset));\
  heapcurrent += sizeof(Term);\
}

#define ListPush(offset)	ListPushb(ToByteOffset(offset))

#define StrPushb(offset) {\
  *(Term*)heapcurrent = TagStr(heapcurrent+(offset));\
  heapcurrent += sizeof(Term);\
}

#define StrPush(offset)	StrPushb(ToByteOffset(offset))

#define ValuePush(value) {\
  *(Term*)heapcurrent = (value);\
  heapcurrent += sizeof(Term);\
}

#define VoidPush() {\
  InitReference((Reference)heapcurrent,andb);\
  heapcurrent += sizeof(Term);\
}

extern andbox *dummy_local_andbox;

#define NotDummyLocal(V)   IsLocalUVA(V,dummy_local_andbox)

extern int alias_check_stack();
extern void initialize_bam();
extern cons_stack_cell *reinit_cons_stack();
extern try_stack_cell *reinit_try_stack();

#define CONS_STACK_SIZE  8096
#define TRY_STACK_SIZE   8096

#define CONS_STACK_BUFFER 1024
#define TRY_STACK_BUFFER 1024

#define Cons_Stack_Overflow_Check(W,Cons) \
{if(Cons>W->cons_stack_head.end) {\
    Cons=reinit_cons_stack(W,Cons)}}

#define Try_Stack_Overflow_Check(W,Try) \
{if(Try>W->cons_stack_head.end) {\
    Try=reinit_try_stack(W,Try)}}

#endif

