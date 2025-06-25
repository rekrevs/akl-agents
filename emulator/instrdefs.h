/* $Id: instrdefs.h,v 1.6 1994/03/22 13:29:59 jm Exp $ */

/* This file contains macros that describe classes of instructions.
 * The descriptions can be used either for encoding instructions,
 * for decoding them, for displaying them, or for whatever you like
 * depending on how you define the basic hook macros that the descriptions
 * are made up of. See "decodeinstr.h" and "encodeinstr.h" for examples.
 */

/* This sets the number of bits to use for index operand fields */

#define INDEX_BITS	16


#define OffsetWord(n,x)		IndexWord(n,x)
#define OffsetField(x,n)	IndexField(x,n)


#define Instr_None(Op) \
	StartInstruction(Op); \
	EndInstruction(1);

#define Instr_Index(Op,i) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	EndInstruction(2);

#define Instr_Index_Index(Op,i,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
        EndInstruction(2);



/* Special version of above without opcode prefetch (only used in engine.c) */

#define Instr_Index_Index_No_Prefetch(Op,i,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
	pc += 2;

#define Instr_Indices_3(Op,i1,i2,i3) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i1,0); \
	IndexField(i2,1); \
	EndCodeWord(1); \
	IndexWord(2,i3); \
	EndInstruction(3);

#define Instr_Index_Label(Op,i,L) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	LabelWord(2,L); \
	EndInstruction(3);

#define Instr_Index_Labels_5(Op,i,L1,L2,L3,L4,L5) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	LabelWord(2,L1); \
	LabelWord(3,L2); \
	LabelWord(4,L3); \
	LabelWord(5,L4); \
	LabelWord(6,L5); \
	EndInstruction(7);

#define Instr_Index_Label_Label(Op,i,L1,L2) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	LabelWord(2,L1); \
	LabelWord(3,L2); \
	EndInstruction(4);

#define Instr_Index_Index_Label_Label(Op,i,j,L1,L2) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
	LabelWord(2,L1); \
	LabelWord(3,L2); \
	EndInstruction(4);

#define Instr_Index_Offset(Op,i,offset) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	OffsetField(offset,1); \
	EndCodeWord(1); \
	EndInstruction(2);

#define Instr_Index_Index_Offset(Op,i,j,offset) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
	OffsetWord(2,offset); \
	EndInstruction(3);

#define Instr_Functor(Op,f) \
	StartInstruction(Op); \
	FunctorWord(1,f); \
	EndInstruction(2);

#define Instr_Functor_Index(Op,f,i) \
	StartInstruction(Op); \
	FunctorWord(1,f); \
	IndexWord(2,i); \
	EndInstruction(3);

#define Instr_Term(Op,x) \
	StartInstruction(Op); \
	TermWord(1,x); \
	EndInstruction(2);

#define Instr_Term_Index(Op,x,i) \
	StartInstruction(Op); \
	TermWord(1,x); \
	IndexWord(2,i); \
        EndInstruction(3);

#define Instr_Index_Term(Op,i,x) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	TermWord(2,x); \
	EndInstruction(3);

#define Instr_Label(Op,L) \
	StartInstruction(Op); \
	LabelWord(1,L); \
	EndInstruction(2);

#define Instr_Size(Op,size) \
	StartInstruction(Op); \
	CodeWord(1,size); \
	EndInstruction(2);

/* Special version of above without opcode prefetch (only used in engine.c) */

#define Instr_Size_No_Prefetch(Op,size) \
	StartInstruction(Op); \
	CodeWord(1,size); \
	pc += 2;

#define Instr_Index_Size(Op,i,size) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	CodeWord(2,size); \
	EndInstruction(3);

#define Instr_Index_Label_Size(Op,i,L,size) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	LabelWord(2,L); \
	CodeWord(3,size); \
	EndInstruction(4);

/* Special version of above without opcode prefetch (only used in engine.c) */

#define Instr_Index_Label_Size_No_Prefetch(Op,i,L,size) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	LabelWord(2,L); \
	CodeWord(3,size); \
	pc += 4;

#define Instr_Index_Term_Label_Label(Op,i,x,L1,L2) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	TermWord(2,x); \
	LabelWord(3,L1); \
	LabelWord(4,L2); \
	EndInstruction(5);

#define Instr_Index_Functor_Label_Number_Number_Index(Op,i1,f,L,lev,offs,i2) \
	StartInstruction(Op); \
	IndexWord(1,i1); \
	FunctorWord(2,f); \
	LabelWord(3,L); \
	CodeWord(3,lev); \
	CodeWord(4,offs); \
	EndInstruction(7);

#define Instr_Pred(Op,Pred)\
	StartInstruction(Op); \
	PredWord(1,Pred); \
        EndInstruction(2);

#define Instr_Pred_Index(Op,Pred,Index) \
	StartInstruction(Op); \
	PredWord(1,Pred); \
	IndexWord(2,Index); \
	EndInstruction(3);

#define Instr_Pred_Bitmask(Op,Pred)\
	StartInstruction(Op); \
	PredWord(1,Pred); \
	/* CodeWord(2,Bitmask); */ \
	EndInstruction(3);

#define Instr_Index_Offset_Term(Op,i,offset,x) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	OffsetField(offset,1); \
	EndCodeWord(1); \
	TermWord(2,x); \
	EndInstruction(3);	

#define Instr_Index_Functor_Label_Offset_Index(Op,i,f,L,offset,j) \
	StartInstruction(Op); \
	IndexWord(1,i); \
	FunctorWord(2,f); \
	LabelWord(3,L); \
	BeginCodeWord(4); \
	OffsetField(offset,0); \
	IndexField(j,1); \
	EndCodeWord(4); \
	EndInstruction(5);	

#define Instr_Index_Functor_Label_Level_Offset_Index(Op,i,f,L,lev,offset,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	CodeField(lev,INDEX_BITS,INDEX_BITS); \
	EndCodeWord(1); \
	FunctorWord(2,f); \
	LabelWord(3,L); \
	BeginCodeWord(4); \
	OffsetField(offset,0); \
	IndexField(j,1); \
	EndCodeWord(4); \
	EndInstruction(5);	

#define Instr_Index_Offset_Functor_Label_Offset_Index(Op,i,o1,f,L,o2,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	OffsetField(o1,1); \
	EndCodeWord(1); \
	FunctorWord(2,f); \
	LabelWord(3,L); \
	BeginCodeWord(4); \
	OffsetField(o2,0); \
	IndexField(j,1); \
	EndCodeWord(4); \
	EndInstruction(5);	

#define Instr_Index_Offset_Functor_Label_Level_Offset_Index(Op,i,o1,f,L,lev,o2,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	OffsetField(o1,1); \
	EndCodeWord(1); \
	FunctorWord(2,f); \
	LabelWord(3,L); \
	CodeWord(4,lev); \
	BeginCodeWord(5); \
	OffsetField(o2,0); \
	IndexField(j,1); \
	EndCodeWord(5); \
	EndInstruction(6);	

#define Instr_Index_Label_Index(Op,i,L,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
	LabelWord(2,L); \
	EndInstruction(3);

#define Instr_Index_Label_Level_Index(Op,i,L,lev,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	IndexField(j,1); \
	EndCodeWord(1); \
	LabelWord(2,L); \
	CodeWord(3,lev); \
	EndInstruction(4);

#define Instr_Index_Offset_Label_Index(Op,i,offset,L,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	OffsetField(offset,1); \
	EndCodeWord(1); \
	LabelWord(2,L); \
	IndexWord(3,j); \
	EndInstruction(4);

#define Instr_Index_Offset_Label_Level_Index(Op,i,offset,L,lev,j) \
	StartInstruction(Op); \
	BeginCodeWord(1); \
	IndexField(i,0); \
	OffsetField(offset,1); \
	EndCodeWord(1); \
	LabelWord(2,L); \
	BeginCodeWord(3); \
	CodeField(lev,0,INDEX_BITS); \
	IndexField(j,1); \
	EndCodeWord(3); \
	EndInstruction(4);

#define Instr_Offset(Op,offset) \
	StartInstruction(Op); \
	OffsetWord(1,offset); \
	EndInstruction(4);

#define Instr_Label_Index(Op,L,i) \
	StartInstruction(Op); \
	LabelWord(1,L); \
	IndexWord(2,i); \
	EndInstruction(3);

/* Note: no opcode prefetch (only used in engine.c) */

#define Instr_Label_Index_Pseudo_No_Prefetch(Op,L,i,pt) \
	StartInstruction(Op); \
	LabelWord(1,L); \
	IndexWord(2,i); \
	TermWord(3,pt); \
	EndInstruction(4);

#define Instr_Label_Label(Op,L1,L2) \
	StartInstruction(Op); \
	LabelWord(1,L1); \
	LabelWord(2,L2); \
	EndInstruction(3);

#define Instr_Label_Size(Op,L,size) \
	StartInstruction(Op); \
	LabelWord(1,L); \
	CodeWord(2,size); \
	EndInstruction(3);

/* Special version of above without opcode prefetch (only used in engine.c) */

#define Instr_Label_Size_No_Prefetch(Op,L,size) \
	StartInstruction(Op); \
	LabelWord(1,L); \
	CodeWord(2,size); \
	pc += 3;

#define Instr_Pred_Size(Op,Pred,size) \
	StartInstruction(Op); \
	PredWord(1,Pred); \
	CodeWord(2,size); \
	EndInstruction(3);

#define Instr_Level_Label(Op,lev,L) \
	StartInstruction(Op); \
	CodeWord(1,lev); \
	LabelWord(2,L); \
	EndInstruction(3);

#define Instr_Label_Size_Size(Op,L,size1,size2) \
	StartInstruction(Op); \
	LabelWord(1,L); \
	CodeWord(2,size1); \
	CodeWord(3,size2); \
	EndInstruction(4);

#define Instr_Labels_7(Op,Lv,La,Li,Ld,Lb,Ll,Ls) \
	StartInstruction(Op); \
	LabelWord(1,Lv); \
	LabelWord(2,La); \
	LabelWord(3,Li); \
	LabelWord(4,Ld); \
	LabelWord(5,Lb); \
	LabelWord(6,Ll); \
	LabelWord(7,Ls); \
	EndInstruction(8);


/* Some special cases: */

/* This is used for the else branch of a switch_on_constant/structure table */

#define Next_Label(Label)\
	LabelWord(0,Label); \
	EndInstruction(1);

/* This is used for the next pair of Y register indices in some instruction */

#define Next_Index_Index(Index1,Index2) \
	BeginCodeWord(0); \
	IndexField(Index1,0); \
	IndexField(Index2,1); \
	EndCodeWord(0); \
	EndInstruction(1);

/* These are only used when encoding switch_on_constant/structure tables.
 * Note that CodeWord rather than LabelWord is used for the labels.
 * This is because the label reference linking must be postponed until
 * after the entire table is sorted.
 */
#define ConstantTableEntry(Const,Label) \
	TermWord(0,Const); \
	CodeWord(1,Label); \
	EndInstruction(2);

#define FunctorTableEntry(F,Label) \
	FunctorWord(0,F); \
	CodeWord(1,Label); \
	EndInstruction(2);

#define BamConstTableEntry(Const,Label)	ConstantTableEntry(Const,Label)
#define BamFunctorTableEntry(F,Label)	FunctorTableEntry(F,Label)

/* These are only used in "engine.c", but live here since they contain
 * information about the format of the "try" class of instructions.
 */
#define FetchTryBranchLabel(Label)	LabelWord(1,Label)
#define TryInstructionSize		2

#define BTryInstructionSize		4

/* These are only used in "engine.c", but live here since they contain
 * information about the format of various branch instructions.
 */

/* SWITCH_ON_TERM */
#define SotLvOffset		1
#define SotLaOffset		2
#define SotLiOffset		3
#define SotLdOffset		4
#define SotLbOffset		5
#define SotLlOffset		6
#define SotLsOffset		7

/* JUMP */
#define JumpLabelOffset		1

/* LIST_SWITCH */
#define LsNilOffset		2
#define LsVarOffset		3
#define LsInstructionSize	4

/* EQ_REG_REG */
#define EqRegRegSkipOffset	2
#define EqRegRegFailOffset	0
#define EqRegRegSuspendOffset	1

/* FLAT_QUIET_CUT */
#define FlatQuietCutInstructionSize	3




