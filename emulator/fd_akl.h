#include "include.h"
#include "term.h"
#include "error.h"
#include "instructions.h"
#include "tree.h"
#include "predicate.h"
#include "initial.h"
#include "exstate.h"
#include "debug.h"
#include "trace.h"
#include "time.h"
#include "storage.h"
#include "names.h"
#include "config.h"
#include "engine.h"
#include "unify.h"
#include "copy.h"
#include "gc.h"
#include "display.h"
#include "examine.h"
#include "abstraction.h"
#include "port.h"
#include "fd.h"
#include "fd_instr.h"

#define N 32	 /* What is the best way of getting rid of the explicit 32? */
#define MaxL 8	 /* maximal domain = 0..(N*MaxL-1) */
#define NrDoms 10
#define CodeArea 1000
#define CodeTable 200
#define FdStack 100
#define VStack 50
#define BHeap MaxL*NrDoms

#define IsFinite(X) (IsCvaTerm(X) && RefCvaMethod(Ref(X)) == &finitemethod)
#define Finite(X) ((findom *)RefGva(Ref(X)))

#define DVSize(S) (IsAtomic(S) ? 0 : StrArity(Str(S)))

#define EnvSize(E) (IsAtomic(E) ? 0 : StrArity(Str(E)))
#define EnvArg(N,E) StrArgument(Str(E),N)
#define GET_ARG(N) v_stack[v++] = (long)(EnvArg(N,args));
#define NextInstr(C) byte_code[C++]

#define MakeGlobalIn(C,Cns)\
{\
  NEW(C,fd_constraint);\
  C->next = NULL;\
  C->method = &(fd_in_method);\
  C->f = Cns;\
  Cns->constr = (constraint *)C;\
}
#define MakeInC(C,X,Code,V,Args,Mon,AT,Andb)\
{\
  NEW(C,fd_function);\
  C->info = 0;\
  SetTell(C,AT);\
  C->env = (envid *)&(Andb->env);\
  C->d = X;\
  C->c = Code;\
  C->ent = V;\
  C->args = Args;\
  C->moninfo = Mon;\
}
#define SuspendMinC(X,Y,Andb)\
{\
  min_constraint *C;\
  NEW(C,min_constraint);\
  C->next = NULL;\
  C->method = &(min_method);\
  C->d = X;\
  C->n = Y;\
  SuspendAndb(Finite(X),Andb);\
  LinkConstr(Andb,(constraint *)C);\
  Mark(Andb,Home(Finite(X)->env));\
}
#define PushInSuspension(C,D,FD)\
{\
   fd_suspension *L;\
   NEW(L,fd_suspension);\
   L->dep = D;\
   L->f = C;\
   L->next = FD->isusp;\
   FD->isusp = L;\
}
#define GlobalWakeIn(F,E)\
{\
  andbox *a = Home(F->env);\
  Wake(E,a);\
}

#define Bottom(D) (D == BOTTOM)
#define Growing(D) (D == GROWING)
#define Shrinking(D) (D == SHRINKING)
#define Constant(D) (D == CONSTANT)

#define InitQueue(ZZ,C,D) SetChange(ZZ,C); SetTime(ZZ,D); QH=ZZ; QT=&(ZZ->next)
#define Enqueue(ZZ,F,C,D)\
SetChange(ZZ,C); SetTime(ZZ,D); *QT=ZZ; QT=&(ZZ->next)
#define Dequeue(ZZ) QH=(findom *)QH->next; ZZ->next=NULL; SetTime(ZZ,0); ZZ=QH
#define UpdateQueued(ZZ,C,D) SetChange(ZZ,Change(ZZ)|C); SetTime(ZZ,D)
#define Queued(ZZ) (ZZ->next || QT == &(ZZ->next))
#define QueueDecl findom *QH, **QT

#define DerefForward(C) Forw(C)
#define Forwarded(C) IsCopied(C)

#define SuspendAndb(X,A) {\
  suspension *susp;\
  NewSuspension(susp);\
  susp->type = ANDB;\
  susp->suspended.andb = A;\
  susp->next = X->tsusp;\
  X->tsusp = susp;\
}

#define Change(W) ((W)->info & 0x3)
#define SetChange(W,C) (W)->info = (C) | ((W)->info & ~0x3)
#define LastMask(W) (((W)->info>>2) & 0xF)
#define SetLastMask(W,L) (W)->info = ((L)<<2) | ((W)->info & ~0x3C)
#define IsInterval(W) (((W)->info>>6) & 1)
#define SetInterval(W) (W)->info = ((W)->info | 0x40)
#define UnSetInterval(W) (W)->info = ((W)->info & ~0x40)
#define Time(W) ((W)->info>>8)
#define SetTime(W,D) (W)->info = ((D)<<8) | ((W)->info & 0xFF)
#define Min(W) (W)->min
#define SetMin(W,Min) (W)->min = Min
#define Max(W) (W)->max
#define SetMax(W,Max) (W)->max = Max
#define Min0(D) (D[0])
#define Max0(D) (D[1])
#define SetMin0(D,M) D[0] = M
#define SetMax0(D,M) D[1] = M

#define Ancestor(A) ((A)->status == UNSTABLE || (A)->status == STABLE)
#define LocalEnv(E,A) IsLocalEnv(E,A)
#define Local(E,A) (LocalEnv(*E,A) ? 1 : local(E,A))
#define LocalObj(ZZ,A) Local(&((ZZ)->env),A)
#define ExternalObj(ZZ,A) !LocalObj(ZZ,A)
#define Locality(E,A)\
(LocalEnv(*E,A) ? SELF : (Ancestor(Home(*E)) ? ANCESTOR : locality(E,A)))
#define DerefEnv(E) DerefGenEnv((envid *)E)

#define Trailed(ZZ,A) fd_trailed(ZZ,A)
#define DerefTrail(ZZ) if ((ZZ)->trailed) DerefEnv(&((ZZ)->trailed));
#define NewTrail(ZZ) (!(ZZ)->trailed ? NULL : NewEnv((ZZ)->trailed))
#define MustTrail(ZZ,A)\
(ExternalObj(ZZ,A) && !Trailed(ZZ,A))
#define SetDeadTrail(C) { Min(C) = 1; Max(C) = 0; }
#define IsDeadTrail(C) (Min(C) > Max(C))

#define NewEnv(E)  (envid*)&((Home(E)->previous->env))
#define InCopy(E) !Home(E)->father
#define InGc(E) !Home(E)->father

/*
#define IsDetermined(ZZ) (IsINT(ZZ) ? 1 : IsConstant(Finite(ZZ)))
#define Determine(ZZ) SetConstant(ZZ)
*/
#define IsDetermined(X) is_determined(X)
#define Determine(V,n,Andb,Exs) {\
   Term val;\
   MakeIntegerTerm(val,n);\
   bind_variable(Exs, Andb, V, val);\
}

#define FdVal(ZZ) MaxVal(ZZ)
#define SetFdVal(ZZ,V) SetMax(ZZ,V)
#define MinVal(ZZ) (IsINT(ZZ) ? GetInteger(ZZ) : Min(Finite(ZZ)))
#define MaxVal(ZZ) (IsINT(ZZ) ? GetInteger(ZZ) : Max(Finite(ZZ)))

#define Domain(ZZ) ZZ->d
#define DomainWord(ZZ,I) ZZ->d[I]

#define Tell(F) (F->info & 0x1) /* 1 = only tell, 0 = ask and tell */
#define SetTell(F,AT) F->info |= (AT)
#define NoMore(F) (F->info & 0x4)
#define SetNoMore(F) F->info |= 0x4
#define LastExec(F) (F->info>>8)
#define SetLastExec(F,D) F->info = ((D)<<8) | (F->info & 0xFF)

#define Entailed(F) fd_entailed(F)
#define SetEntailed(V,Ab,Exs) bind_variable(Exs,Ab,V,ZERO)
#define IndexicalEntailed(F) F->constr->method = NULL;
#define CopyEntailed(F) copy_fd_entailed(F)
#define GcEntailed(F) copy_fd_entailed(F)

#define BindVar(ZZ,Y) RefTerm(ZZ) = (Y)

#define PropV(M,Min,Max) (Min==M ? P_MAX : Max==M ? P_MIN : P_MINMAX)

#define FdTrailC 6001
#define FdTrailP(X,CP) ((int)X->trailed < CP)

#define PushFdTrail(A) { assert(fd_cp+2<FdTrailC); \
 fd_trail[fd_cp++] = (uword)A; fd_trail[fd_cp++] = (uword)*A; }

#define Ent(F) Eq(RefTerm(Ref(F->ent)),ZERO) 
#define SetEnt(V) fd_bind_var(V,ZERO)
#define IndexicalEnt(F)\
{ PushFdTrail(&(F->constr->method)); F->constr->method = NULL; }

#ifdef THREADED_CODE

typedef address fd_instr;
#define EnumToFdInstr(c)	(bcode)(fd_instr_label_table[c])
#define FdInstrToEnum(c)	fd_instr_to_enum(c)
#define NextFdInstr             { fd_op = (fd_instr)NextInstr(c); SwitchTo(fd_op); }
extern address *fd_instr_label_table;
extern int fd_instr_label_table_length;
extern enum_fd_instr fd_instr_to_enum();

#else

typedef enum_fd_instr fd_instr;
#define EnumToFdInstr(c)	(bcode)(c)
#define FdInstrToEnum(c)	(c)
#define NextFdInstr             { fd_op = (fd_instr)NextInstr(c); break; }

#endif

typedef unsigned long bitmask;
typedef long bcode;
typedef enum {EMPTY, SUBSET, NONEMPTY} intersect;
typedef enum {BOTTOM,SHRINKING,GROWING,CONSTANT} dom_info;
typedef enum {P_DOM=0, P_MIN=1, P_MAX=2, P_MINMAX=3} dep_class;

extern dep_class propagate_info();
typedef enum {SELF, CHILD, ANCESTOR, SIBLING, NOMORE} locality_info;

typedef struct {
  enum {INTERVAL, DOMAIN} tag;
  int lm;
  bitmask *d;
} domstruct;

typedef Term argvec;

typedef struct moncons {
  Term d;
  struct moncons *next;
} moncons;

typedef struct {
  moncons *mon;
  moncons *amon;
} monstruct;

typedef struct fd_function {
  Term d;
  long c;
  unsigned long info;
  envid *env;
  Term ent;
  argvec args;
  monstruct *moninfo;
  constraint *constr;
} fd_function;

typedef struct fd_suspension {
  dep_class dep;
  fd_function *f;
  struct fd_suspension *next;
} fd_suspension;

typedef struct findom {
  struct gvamethod *method;
  envid 	 *env;
  suspension     *asusp;	/* agent suspensions */  
  suspension     *tsusp;	/* trail suspensions */
  fd_suspension	 *isusp;	/* indexical suspensions */
  struct findom  *next;		/* queueing link */
  envid          *trailed;	/* environment of trail */
  unsigned long  info;
  unsigned long  min;
  unsigned long  max;
  bitmask        *d;
} findom;

typedef struct fd_constraint {
  constrtable		*method;
  struct constraint 	*next;
  fd_function           *f;
} fd_constraint;

typedef struct trail_constraint {
  constrtable	        *method;
  struct constraint     *next;
  Term                  d;
  findom                *dom;
  unsigned long         info;
  unsigned long         min;
  unsigned long         max;
  envid                 *trailed;
  bitmask               *vec;
} trail_constraint;

typedef struct min_constraint {
  constrtable	    *method;
  struct constraint *next;
  Term              d;
  Term              n;
} min_constraint;

typedef struct void_list {
  void             *car;
  struct void_list *cdr;
} void_list;

#ifdef DEFINING_FD_DATA

long code_table[CodeTable];
bcode byte_code[CodeArea];
int label_indx;
int code_top;
int check_label[VStack];
int check_top;
long fd_time;
Functor fd_unif_arg_fun;
Functor fd_domv_fun;
Functor fd_prop_fun;
int dom_code_index_0;
int dom_code_index_1;
Term ZERO;
int fd_cp;
int fd_number_of_solutions;
long fd_trail[FdTrailC];
#else

extern long code_table[];
extern bcode byte_code[];
extern int label_indx;
extern int code_top;
extern int check_label[VStack];
extern int check_top;
extern long fd_time;
extern Functor fd_unif_arg_fun;
extern Functor fd_domv_fun;
extern Functor fd_prop_fun;
extern int dom_code_index_0;
extern int dom_code_index_1;
extern Term ZERO;
extern int fd_cp;
extern int fd_number_of_solutions;
extern long fd_trail[];
#endif

extern gvamethod finitemethod;
extern constrtable fd_in_method;
extern constrtable min_method;
extern constrtable trail_method;

Gvainfo new_finite();
bool 	unify_finite();
int 	print_finite();
Gvainfo	copy_finite();
Gvainfo	gc_finite();
int     uncopied_finite();
envid   *deallocate_finite();
int     kill_finite();
bool    close_finite();
bool    send_finite();
void    gc_external_finite();
void    copy_external_finite();
void    makewake_finite();
void    makerecall_finite();

bool install_in();
void reinstall_in();
void deinstall_in();
void promote_in();
int print_in();
constraint *copy_in();
constraint *gc_in();

bool install_min();
void reinstall_min();
void deinstall_min();
void promote_min();
int print_min();
constraint *copy_min();
constraint *gc_min();

bool install_trail();
void reinstall_trail();
void deinstall_trail();
void promote_trail();
int print_trail();
constraint *copy_trail();
constraint *gc_trail();
intersect intersect_trail();
void trail();
void retrail();
bool fd_in();
void range();
long min_bit();
long max_bit();
int number_of_bits();
void bind_variable();
void wake_suspensions();
void wake_agent_suspensions();
void wake_trail_suspensions();
int fd_entailed();
int copy_fd_entailed();
int fd_trailed();
int local();
locality_info locality();
void print_dom();
void copy_fd_susp();
void gc_fd_susp();
void split_fd_susp();
dom_info monotonicity();
void evaluate_range();
intersect intersect_fd();
bool resuspend_x();
dep_class prop_info();
void move_dom();
bool fd_propagate();
void fd_das();
void copy_args();
void copy_mon();
void gc_args();
void gc_mon();
bool fd_one_solution();
bool fd_one_solution_ff();
void fd_numb_of();
void fd_numb_of_ff();
int is_determined();
