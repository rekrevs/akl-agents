/* $Id: code.h,v 1.17 1994/03/22 13:29:31 jm Exp $ */

extern int	codesize;
extern code 	*codespace;
extern code 	*codecurrent;
extern code 	*codeend;
extern code	*codebase;
extern code	labelrefs[];
extern char	labeltag[];

typedef enum {
    LABELREF_NULLTAG = 0,
    LABELREF_TAG_LST,
    LABELREF_TAG_STR,
    LABELREF_TAG_LBL
} labelref_tag;

#define BamBindingConst(x) \
	TermWord(0,x); \
	EndInstruction(1);

#define BamBindingLabel(x,tag) \
	if ((x) < MAXLABELS) labeltag[x] = (tag); \
	LabelWord(0,x); \
	EndInstruction(1);

#define BamBindingFunctor(f) \
	FunctorWord(0,f); \
	EndInstruction(1);

#define NOLABEL (-1)

/* Macros used by the parser to allocate instructions in the codearea */

#define Define_Predicate(Name,Arity)\
{ Atom A;\
  A = store_atom(Name);\
  define_e_predicate(A,Arity,codecurrent);\
  reset_label();\
}

#define End_Definition\
{ fix_labelrefs();\
  if(codecurrent > codeend){\
     FatalError("out of code space");\
  }\
}

#define Define_Abstraction(Number,Arity,Ext)\
{ define_abstraction(Number,Arity,Ext,codecurrent);\
  reset_label();\
}


#define IndexToOffset(x) \
	((x) >= MAX_AREGS ? index_overflow() : (x)*sizeof(Term))

#define Bindex(i) ((i)-1)

#define Label(Label) store_label(Label)

extern void sort_switch_table();

extern void store_label();
extern void reset_label();


extern void init_code();

extern int index_overflow();
extern int label_overflow();

extern void fix_labelrefs();

extern void store_abstraction();
extern struct predicate *get_abstraction();

