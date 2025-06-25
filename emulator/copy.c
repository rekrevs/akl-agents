/* $Id: copy.c,v 1.60 1994/04/20 12:33:03 rch Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "copy.h"
#include "storage.h"
#include "config.h"
#include "initial.h"
#include "unify.h"
#include "trace.h"
#include "time.h"
#include "debug.h"
#include "statistics.h"
#include "regdefs.h"
#include "error.h"

/* The tree is copied top-down. When an andbox is copied its
 * env field is set to NULL and the status field is set to a
 * pointer to the copy. This is done in order to detect if a
 * variable is local to the sub tree that is to be copied.
 *
 * It is assumed that an andbox has only one continuation. This
 * should be the case. Several continuations are not detected,
 * no error is reported.
 *
 * When a constraint is copied the father field is set to NULL
 * and the previous pointer is used to point to the copy of
 * the constraint. This is done in order to copy the suspensions
 * of variables.  The copies will have the  the same order. This
 * is not necessary but nice for debugging purposes.
 *
 * Structures and lists are copied and the tag field is set to a
 * pointer to the copy. This prevents duplications of copies and
 * furthermore allows circular structures. A pointer to a copied
 * structure or list is placed on a stack. When the whole tree is
 * copied this stack is traversed and tag fields reset. Atoms,
 * integers and floats are not copied.
 *
 * Variables are copied if they are local to an andbox that has
 * been copied. Variables are saved and reset in the same way as
 * structures. The suspensions of constrained variables (GVA) are
 * copied when the variable is reset.
 *
 * Variables that are not local to the sub tree can still have
 * suspensions that must be duplicated. The external constrained
 * variables are saved and restored in the same way as local
 * variable. When the variables are reset the suspensions are checked
 * for copied constraints.
 *
 * Terms are copied using a scavanger algorithm.
 *
 * 
 */

/* local prototypes */
void		copy_saved_refs();
void		copy_close();
void		copy_context();
void		copy_tree();
void		copy_continuation();
void		copy_constraints();
void		copy_unifier();
void		copy_registers();
void		reset_local_suspensions();
void		reset_external_suspensions();
void		local_suspensions();
void		external_suspensions();
void 		reset_tree();
void		reset_sva();
void		reset_xtr();


/* Stacks for Reference copying (borrowed from GC) */

Term	**copyrefstart;
Term	**copyrefend = NULL;
Term	**copyreftop;
Term	**copyrefstack;
long	debug_copyrefstacksize;
long	debug_maxcopyrefstacksize = 0;

#define SaveRef(REFLOC) \
{\
  copyreftop++;\
  if(copyreftop == copyrefend) {\
    reinit_copyrefstack();\
  }\
  *copyreftop = (REFLOC);\
}


#ifdef macintosh
#define AKLCOPYCTX 100
#else
#define AKLCOPYCTX 1000
#endif

typedef struct susentry {
  suspension	**from;
  suspension	**to;
} susentry;

typedef struct chkentry {
  suspension	**from;
} chkentry;


/* XtrMark(X), etc, are used instead of the XTR and XGN object tags.
 * For variables, a mark bit is set in the susp pointer.
 * For generics, a second mark bit is set in the method pointer.
 * This is somewhat of a kludge, and some more general copying scheme
 * should be developed in the future...
 */

#define XtrMark(x)	*(tagged_address*)(&(x)->susp) += MarkBitMask
#define XtrUnMark(x)	*(tagged_address*)(&(x)->susp) -= MarkBitMask
#define IsXtrMarked(x)	((tagged_address)((x)->susp) & MarkBitMask)

#define AltMarkBitMask	((uword)0x00000002)
#define XgnMark(x)	*(tagged_address*)(&(x)->method) += AltMarkBitMask
#define XgnUnMark(x)	*(tagged_address*)(&(x)->method) -= AltMarkBitMask
#define IsXgnMarked(x)	((tagged_address)((x)->method) & AltMarkBitMask)


#define SaveTRM(OLD,NEW) \
{    (trmp++);\
     if(trmp == trmend)\
	reinit_trmstack();\
     (trmp)->loc = (ZappedWord*)(OLD);\
     (trmp)->fixup_data = *(ZappedWord*)(OLD);\
     Link(OLD,NEW);\
}

#define SaveSVA(OLD,NEW) \
{    (svap++);\
     if(svap == svaend)\
	reinit_svastack();\
     *(svap) = OLD;\
}

#define SaveXTR(OLD) \
{ if (!IsXtrMarked(OLD)) {\
     (xtrp++);\
     if(xtrp == xtrend)\
	reinit_xtrstack();\
     *(xtrp) = OLD;\
     XtrMark(OLD);\
}}

#define SaveXGN(OLD) \
{ if (!IsXgnMarked(OLD)) {\
     xgnp++;\
     if(xgnp == xgnend)\
       reinit_xgnstack();\
     *(xgnp) = OLD;\
     XgnMark(OLD);\
}}

/* Kludge due to that Xgn marking generics destroys their method */
#define Method(X)  ((gvamethod*) (((uword)((X)->method)) & ~AltMarkBitMask))

#define SaveGN(OLD) \
{    xgnp++;\
     if(xgnp == xgnend)\
       reinit_xgnstack();\
     *(xgnp) = OLD;\
}

#define SaveSusp(SUSP,TO) {\
  (susp++);\
  if(susp == susend) {\
    reinit_susstack();\
  }\
  susp->from = (SUSP);\
  susp->to = (TO);\
}

#define ResetSVA reset_sva();

#define ResetXTR reset_xtr();

#define CopiedAndb(A) ((A)->father == NULL)

#define CopiedChb(C) ((C)->father == NULL)

#define InCopyUVA(V) InCopyEnv(Env(RefTerm(V)))
#define InCopyGVA(V) InCopyEnv(GvaEnv(RefGva(V)))
#define InCopyRef(V) InCopyEnv(RefEnv(V))

#ifdef STRUCT_ENV
#define InCopyStr(S) InCopyEnv(StructEnvRef(S)->env)
#endif

typedef unsigned long ZappedWord;

typedef struct trmentry {
  ZappedWord *loc;
  ZappedWord fixup_data;
} trmentry;

trmentry *trmstack;
trmentry *trmend = NULL;
trmentry *trmp;

svainfo **svastack;
svainfo **svaend = NULL;
svainfo **svap;

svainfo **xtrstack;
svainfo **xtrend = NULL;
svainfo **xtrp;

Gvainfo *xgnstack;
Gvainfo *xgnend = NULL;
Gvainfo *xgnp;

susentry *susstack;
susentry *susend = NULL;
susentry *susp;

chkentry *chkstack;
chkentry *chkend = NULL;
chkentry *chkp;

#define SetupStack(TYPE,STACK,END,TOP,SIZE) \
{\
    if ((END) == NULL) {\
      (STACK) = (TYPE*) malloc(sizeof(TYPE)*(SIZE));\
      if ((STACK) == NULL) {\
        FatalError("malloc in copy failed");\
      }\
      (END) = (STACK) + (SIZE);\
    }\
    (TOP) = (STACK);\
}

#define ReInitStackProc(TYPE,STACK,END,TOP,NAME) \
{\
    long length = (END)-(STACK);\
    long newlength;\
    TYPE *new;\
    newlength = length+length;\
    new = (TYPE*) realloc(STACK, sizeof(TYPE)*newlength);\
    if (new == NULL) {\
      char buf[100];\
      sprintf(buf, "could not reallocate %s", (NAME));\
      FatalError(buf);\
    }\
    (STACK) = new;\
    (TOP) = (STACK)+length;\
    (END) = (STACK)+newlength;\
    /*printf("{Growing %s to length %ld}\n", (NAME), newlength);*/\
}

void reinit_copyrefstack()
{
  ReInitStackProc(Term*,copyrefstack,copyrefend,copyreftop,"copyrefstack");
  copyrefstart = &copyrefstack[-1];
}

void reinit_trmstack()
{
  ReInitStackProc(trmentry,trmstack,trmend,trmp,"trmstack");
}

void reinit_svastack()
{
  ReInitStackProc(svainfo*,svastack,svaend,svap,"svastack");
}

void reinit_xtrstack()
{
  ReInitStackProc(svainfo*,xtrstack,xtrend,xtrp,"xtrstack");
}

void reinit_xgnstack()
{
  ReInitStackProc(Gvainfo,xgnstack,xgnend,xgnp,"xgnstack");
}

void reinit_susstack()
{
  ReInitStackProc(susentry,susstack,susend,susp,"susstack");
}

void reinit_chkstack()
{
  ReInitStackProc(chkentry,chkstack,chkend,chkp,"chkstack");
}

void copy_reset_gn()
{
  Gvainfo *ip, *op, xgn;

  ip = xgnp;
  op = xgnp;
  while (ip != xgnstack) {
    xgn = *ip;
    if (!IsXgnMarked(xgn)) { 
      ((Gvainfo)Forw(xgn))->method->copy_external(xgn);
    }
    if (op != xgnp) {
      ip = xgnp;
      op = xgnp;
    }
    else {
      ip--;
    }
  }
}

void reset_xgn()
{
  Gvainfo xgn;

  while (xgnp != xgnstack) {
    xgn = *xgnp--;
    if (IsXgnMarked(xgn)) { 
      XgnUnMark(xgn);
      xgn->method->copy_external(xgn);  
    }
  }
}

andbox *copy(cand, exs)
     andbox *cand;		/* The candidate for promotion */
     exstate *exs;
{
  andbox 	*mother;	/* The andbox to be copied */
  andbox 	*copy;		/* The copy of mother */
  choicebox 	*fork;		/* The choicebox above the candidate */

  andbox	*promoted;	/* The copy of the candidate */
  andbox 	*tried;		/* The rest of the andboxes under fork */

 
#ifdef METERING
  int temptime;
#endif
    
#ifdef METERING
  temptime = systime();
  copytotal += 1;
#endif

  SetupStack(trmentry,trmstack,trmend,trmp,AKLCOPYTRM);
  SetupStack(svainfo*,svastack,svaend,svap,AKLCOPYSVA);
  SetupStack(svainfo*,xtrstack,xtrend,xtrp,AKLCOPYXTR);
  SetupStack(Gvainfo, xgnstack,xgnend,xgnp,AKLCOPYXGN);
  SetupStack(susentry,susstack,susend,susp,AKLCOPYSUS);
  SetupStack(chkentry,chkstack,chkend,chkp,AKLCOPYCHK);

  SetupStack(Term*, copyrefstack,copyrefend,copyreftop,AKLCOPYREFS);
  copyrefstart = &copyrefstack[-1];
  copyreftop = copyrefstart;
  debug_copyrefstacksize = 0;

  fork = cand->father;
  mother = fork->father;

  /* unlink the candidate from the rest of the
   * andboxes under fork.
   */

  if(cand->previous != NULL)
    cand->previous->next = cand->next;
  else
    fork->tried = cand->next;
  if(cand->next != NULL)
    cand->next->previous = cand->previous;
  cand->next = NULL;
  cand->previous = NULL;

  /* The sibblings to the candidate are saved.
   * The candidate is the only anbox below fork
   */
  
  tried = fork->tried;
  fork->tried = cand;

  /* The copy of mother is created */
  NewAndbox(copy);

#ifdef TRACE
  copy->id = identity++;
  copy->trace = mother->trace;
  copy->clno = mother->clno;
#endif

  copy->status = mother->status;
  copy->father = mother->father;
  copy->env = NULL;

  /* insert the copy to the left of mother */
  copy->next = mother;
  copy->previous = mother->previous;
  if(mother->previous != NULL)
    mother->previous->next = copy;
  else 
    mother->father->tried = copy;
  mother->previous = copy;

  /* The mother is marked as copied */
  mother->father = NULL;

  copy_continuation(mother, copy);

  copy_constraints(mother, copy);

  copy_unifier(mother, copy);  

  copy_tree(mother, copy);

  copy_reset_gn();

  copy_saved_refs();

  copy_close(exs);

  /* Traverse the refstack again,
   * since copy_close() may have tried to copy variables:
   */
  copy_saved_refs();

  copy_context(exs);

  /* The copy of the candidate is found by looking at
   * the previous field of the candidate.
   */

  promoted = (andbox*)cand->previous;
  
  ResetSVA;

  reset_xtr();

  reset_xgn();

  /*
   * Reset forwarding pointers
   */
  while(trmp != trmstack) {
    *(trmp->loc) = trmp->fixup_data;
    /* This also resets the mark bit for copied objects,
     * since the mark bit is in the pointer.
     */
    trmp -= 1;
  }

  reset_local_suspensions();

  reset_external_suspensions();

  reset_tree(mother);

  mother->father = copy->father;

  /* install the sibblings of the candidate */
  fork->tried = tried;

  /* kill the candidate  */
  cand->status = DEAD;

  free(trmstack); trmend = NULL;
  free(svastack); svaend = NULL;
  free(xtrstack); xtrend = NULL;
  free(xgnstack); xgnend = NULL;
  free(susstack); susend = NULL;
  free(chkstack); chkend = NULL;
  free(copyrefstack); copyrefend = NULL;

#ifdef METERING
  copytime = copytime + (systime() - temptime);
#endif
  
  if (debug_copyrefstacksize > debug_maxcopyrefstacksize) {
    debug_maxcopyrefstacksize = debug_copyrefstacksize;
#if 0
    printf("<<COPY REF stack high score: %ld>>\n", debug_maxcopyrefstacksize);
#endif
  }

  return promoted;
}

void reset_tree(andb)
     andbox *andb;
{
  andbox *tempa, *preva;
  choicebox *tempc, *prevc;
  prevc = NULL;
  
  for(tempc = andb->tried; tempc != NULL; tempc = tempc->next){
    tempc->father = andb;
    tempc->previous = prevc;
    prevc = tempc;
    preva = NULL;
    for(tempa = tempc->tried; tempa != NULL; tempa = tempa->next) {
      tempa->father = tempc;
      tempa->previous = preva; 
      preva = tempa;
      if(tempa->tried != NULL)
	reset_tree(tempa);
    }
  }
}



int is_copied(trm)
     Term trm;
{
  return IsForwMarked(AddressOf(trm));
}


void reset_local_suspensions()
{
  suspension **to;
  suspension **from;
  
  while(susp != susstack) {
    to = susp->to;
    from = susp->from;
    local_suspensions(to, from);
    susp--;
  }
}

void local_suspensions(to, from)
     suspension **to;
     suspension **from;
{
  suspension *next, *prevs, *news; 

  prevs = NULL;
  for(next = *from; next != NULL; next = next->next) {
    switch(next->type) {
    case CHB:
      {
	choicebox *chb, *newc;
	chb = next->suspended.chb;
	if(chb->father == NULL) {
	  newc = chb->previous;
	  NewSuspension(news);
	  news->type = CHB;
	  news->suspended.chb = newc;
	  if(prevs == NULL)
	    *to = news;
	  else
	    prevs->next = news;
	  prevs = news;
	} else {
	  *from = next->next;
	}
	break;
      }
    case ANDB:
      {
	andbox *andb, *newa;
	andb = next->suspended.andb;
        if(andb->father == NULL) {
	  newa = andb->previous;
	  NewSuspension(news);
	  news->type = ANDB;
	  news->suspended.andb = newa;
	  if(prevs == NULL)
	    *to = news;
	  else
	    prevs->next = news;
	  prevs = news;
	} else {
	  *from = next->next;
	}
	break;
      }
    default:
      FatalError("Copy: wrong type of type in suspension");
      break;
    }
    from = &next->next;
  }
  if(prevs == NULL)
    *to = NULL;
  else
    prevs->next = NULL;
}

void external_suspensions(from)
     suspension **from;
{
  suspension *next, *prevs, *news;
      
  prevs = NULL;
  for(next = *from; next != NULL; next = next->next) {
    switch(next->type) {
    case CHB:
      {
	choicebox *chb, *newc;
	chb = next->suspended.chb;
	if(CopiedChb(chb)) {
	  newc = chb->previous;
	  NewSuspension(news);
	  news->type = CHB;
	  news->suspended.chb = newc;

	  if(prevs == NULL)
	    *from= news;
	  else
	    prevs->next = news;
	  news->next = next;
	}
	prevs = next;
	break;
      }
    case ANDB:
      {
	andbox *andb, *newa;
	andb = next->suspended.andb;
	if(CopiedAndb(andb)) {
	  newa = andb->previous;
	  NewSuspension(news);
	  news->type = ANDB;
	  news->suspended.andb = newa;

	  if(prevs == NULL)
	    *from= news;
	  else
	    prevs->next = news;
	  news->next = next;
	}
	prevs = next;
	break;
      }
    default:
      FatalError("Copy: wrong type of type in suspension");
      break;
    }
  }
}


void reset_external_suspensions()
{
  while(chkp != chkstack) {
    external_suspensions(chkp->from);
    chkp--;
  }
}

void reset_sva() {
  svainfo *old, *new;
  
  while(svap != svastack) {
    old = *svap;
    new = Forw(old);
    local_suspensions(&new->susp, &old->susp);
    svap--;
  }
}

void reset_xtr() {

  svainfo *xtr;
  
  while(xtrp != xtrstack) {
    xtr = *xtrp;
    XtrUnMark(xtr);
    external_suspensions(&xtr->susp);
    xtrp--;
  }
}

void copy_close(exs)
     exstate *exs;
{

  close_entry *hare;
  generic *gen;
  
  for(hare = exs->close; hare != NULL; hare = hare->next ) {
    gen = (generic*)hare->obj;	/* Let's pretend it's a generic */
    if(IsCopied(gen)) {
      /* if the generic object is copied we must add
       * an entry on the close list.
       */
      if(hare->type == GENERIC_CLOSE)
	add_generic_to_close((generic*)(Forw(gen)),exs);
      else
	add_gvainfo_to_close((gvainfo*)(Forw(gen)),exs);
    } else {
      /* let the generic object decide if it needs
       * to add itself on the close list.
       */
      if(Method(gen)->uncopied(gen)) {
	if(hare->type == GENERIC_CLOSE) {
	  generic *ngen = gen->method->new(gen);
	  ngen->method = gen->method;
	  SaveTRM(gen,ngen);
	  ngen->method->copy(gen,ngen);
	  add_generic_to_close(ngen,exs);
	} else {
	  /* It's a gvainfo thing */
	  gvainfo *gva = (gvainfo*)gen;
	  DerefGvaEnv(gva);
	  if(InCopyEnv(GvaEnv(gva))) {
	    /* Create a new gvainfo object and copy the old with its method */
	    gvainfo *ngva = gva->method->new(gva);
	    ngva->method = gva->method;
	    ngva->env = NewEnv(gva->env);
	    SaveTRM(gva,ngva);
	    ngva->method->copy(gva,ngva);
	    add_gvainfo_to_close(ngva,exs);
	  }
	}
      }
    }
  }    
}

void copy_context(exs)
     exstate *exs;
{
  choicebox *save[AKLCOPYCTX];
  context *start, *current, *cntxt;
  int j;
  
  newcontext(exs);
  
  j = 1;
  start = exs->context.start +j;
  current = exs->context.current;
  
  for(cntxt = start; cntxt != current; cntxt++, j++) {
    recall *first, *last, *rec, *wakp;
    int i;

    i = 0;
    first = cntxt->recall;
    last = (cntxt +1)->recall;

    for(rec = first; rec != last; rec++) {
      if(CopiedChb(rec->chb)) {
	save[++i] = rec->chb->previous;
      }
    }

    if(i == 0)
      continue;

    wakp =  shift_recall(exs,i,j);

    for(; i != 0; i--) {
      wakp->chb = save[i];
      wakp++;
    }
  }

  resetcontext(exs);
}

    


void copy_tree(olda,newa)
     andbox *olda, *newa;
{
  choicebox *prevc, *templc;

  prevc = NULL;
  for(templc = olda->tried; templc != NULL; templc = templc->next) {
    choicebox *chb;
    andbox *preva, *templa;

    NewChoicebox(chb);
    if(templc->cont != NULL) {
      choicecont *cont;
      NewChoiceCont(cont,templc->cont->arity);
      chb->cont = cont;
      copy_registers(templc->cont, cont);
      chb->cont->instr.label = templc->cont->instr.label;
    } else {
      chb->cont = NULL;
    }
    chb->previous = prevc;
    if(prevc != NULL)
      prevc->next = chb;
    else
      newa->tried = chb;

    chb->def = templc->def;
    chb->father = newa;
    chb->type = templc->type;
#ifdef TRACE
    chb->id = identity++;
    chb->trace = templc->trace;
    chb->next_clno= templc->next_clno;
#endif

    /* to indicate that the choicebox has been copied */
    templc->father = NULL;

    /* this is so that the copy can be found */    
    templc->previous = chb;
    
    preva = NULL;

    for(templa = templc->tried; templa != NULL; templa = templa->next) {
      andbox *andb;

      NewAndbox(andb);

      andb->status = templa->status;
      andb->father = chb;
      andb->env = NULL;
#ifdef TRACE
      andb->id = identity++;
      andb->trace = templa->trace;
      andb->clno = templa->clno;
#endif

      andb->previous = preva;

      if(preva != NULL)
	preva->next = andb;
      else
	chb->tried = andb;

      /* this is to indicate that the andbox has been copied */
      templa->father = NULL;

      /* this is so that the copy can be found */
      templa->previous = andb;

      copy_continuation(templa, andb);

      copy_constraints(templa, andb);

      copy_unifier(templa, andb);      

      /* a recursive call if there are choiceboxes below */
      if(templa->tried != NULL)
	copy_tree(templa, andb);
      else
	andb->tried = NULL;

      preva = andb;
    }
    if(preva != NULL)
      preva->next = NULL;
    else
      chb->tried = NULL;

    prevc = chb;
  }
  if(prevc != NULL)
    prevc->next = NULL;
  else
    newa->tried = NULL;
}


void copy_continuation(templa, andb)
     andbox *templa, *andb;
{
    andcont	*templt,  *cont;
    Term 	old, *tyreg, *cyreg;
    int		ysize, i;
    templt = templa->cont;
    NewAndCont(cont, templt->ysize);
    cont->next = NULL;
    cont->label = templt->label;
    cont->ysize = templt->ysize;
#ifdef TRACE
    cont->trace = templt->trace;
    cont->def = templt->def;
    cont->clno = templt->clno;
    {
      choicecont *ch_cont;
      NewChoiceCont(ch_cont,templt->choice_cont->arity);
      cont->choice_cont=ch_cont;
      copy_registers(templt->choice_cont,ch_cont);
    }
#endif

    tyreg = templt->yreg;
    ysize = templt->ysize;
    cyreg = cont->yreg;
    for(i = 0; i != ysize ; i++ ) {
	if(Eq(tyreg[i], NullTerm)) {
	    cyreg[i] = NullTerm;
	} else {
	    old = tyreg[i];
	    cyreg[i] = old;
	    copy_locations(&cyreg[i],1);
	}
    }
    andb->cont = cont;
}


void copy_constraints(templa,andb)
     andbox *templa, *andb;
{
  constraint *constr, *prevc, *newc;

  prevc = NULL;
    
  for(constr = templa->constr; constr != NULL; constr = constr->next) {
    constrtable *meth = constr->method;
    if (meth != NULL) {
      if (IsForwMarked(meth)) {
	newc = ForwOf((tagged_address)meth);
      } else {
	newc = (meth->copy)(constr);
	if (!newc) continue;
	newc->method = meth;
	SaveTRM(TagRef(constr),TagRef(newc));
      }
      if(prevc == NULL)
	andb->constr = newc;
      else
	prevc->next = newc;

      prevc = newc;
    }
  }
  if(prevc != NULL)
    prevc->next = NULL;
  else
    andb->constr = NULL;
}


void copy_unifier(templa,andb)
     andbox *templa, *andb;
{
  unifier *unif, *prevu, *newu;
  Term var, trm;
  prevu = NULL;
    
  for(unif = templa->unify; unif != NULL; unif = unif->next) {

    NewUnifier(newu);

    var = unif->arg1;
    trm = unif->arg2;

    newu->arg1 = var;
    newu->arg2 = trm;

    copy_locations(&newu->arg1,2);	/* Copies both arg1 and arg2 */

    if(prevu == NULL)
      andb->unify = newu;
    else
      prevu->next = newu;

    prevu = newu;
  }
  if(prevu != NULL)
    prevu->next = NULL;
  else
    andb->unify = NULL;
}


void copy_registers(templc, chb)
     choicecont* templc, *chb;
{
  Term 	*targ, *carg;
  int 	arity, i;
  arity = templc->arity;
  targ = templc->arg;
  carg = chb->arg;
  
  for(i = 0 ; i != arity ; i++ ) {
    if(Eq(targ[i], NullTerm)) {
      carg[i] = NullTerm;
    } else {
      carg[i] = targ[i];
      copy_locations(&carg[i],1);
    }
  }
  chb->arity = arity;
}


#define CopyUnbound(OLD,NEW) \
{\
  register Term ZZZt = (OLD);\
  if (IsUVA(ZZZt)) {\
    (NEW) = TagUva(NewEnv(Env(ZZZt)));\
  } else {\
    register uword FW;\
    register gvainfo *ZZZg = Gva(ZZZt);\
    FW = ForeWord(ZZZg);\
    if (IsForwMarked(FW)) {\
      (NEW) = TagGva(ForwOf(FW));\
    } else if (GvaIsSva(ZZZg)) {\
      svainfo *ZZZs = GvaSva(ZZZg);\
      svainfo *ZZZn;\
      NewSvainfo(ZZZn);\
      ZZZn->method = NULL;\
      ZZZn->susp = NULL;\
      ZZZn->env = NewEnv(ZZZs->env);\
      (NEW) = TagGva(ZZZn);\
      SaveSVA(ZZZs,ZZZn);\
      SaveTRM(ZZZs,ZZZn);\
    } else {\
      gvainfo *ZZZn;\
      SaveHeapRegisters();\
      ZZZn = ZZZg->method->new(ZZZg);\
      ZZZn->method = ZZZg->method;\
      ZZZn->env = NewEnv(ZZZg->env);\
      SaveTRM(ZZZg,ZZZn);\
      ZZZn->method->copy(ZZZg,ZZZn);\
      RestoreHeapRegisters();\
      (NEW) = TagGva(ZZZn);\
    }\
  }\
}

void copy_saved_refs()
{
  long refstacksize = copyreftop - copyrefstart;
  if (refstacksize > debug_copyrefstacksize)
    debug_copyrefstacksize = refstacksize;

  while (copyreftop != copyrefstart) {
    Term *loc, old_t, result;
    uword FW;

    loc = *copyreftop;
    copyreftop--;
    old_t = *loc;

    if (!IsREF(old_t)) {
      FatalError("COPY: Non-REF on ref stack");
    } else {
      register Reference old = Ref(old_t);
      FW = ForeWord(old);
      if (IsForwMarked(FW)) {
	result = TagRef(ForwOf(FW));
      } else {
	Reference new;
	NewReference(new);
	if(IsUnBoundRef(old)) {
	  register Term varterm;
	  CopyUnbound(RefTerm(old),varterm);
	  RefTerm(new) = varterm;
	  SaveTRM(old,new);
	} else {
	  RefTerm(new) = RefTerm(old);
	  SaveTRM(old,new);
	  copy_locations(new,1);
	}
	result = TagRef(new);
      }
    }

    *loc = result;
  }
}



/*
 *  New copy scheme:
 *
 *  Copy one object with its substructures intact (pointing to from-space).
 *  Put a forwarding pointer in the original object, together with a mark
 *  bit to indicate that it has been copied. Scavenge the internal parts
 *  of the object, back to front. A stack of locations not yet scavenged
 *  is maintained by flipping pointers inside objects (threaded stack).
 *  A mark bit is used to mark the first pointer in an object, in order
 *  to detect when scavenging has been completed in that object (going
 *  back to front). The same bit is used as the one for forwarding
 *  pointers.
 *
 */

#define ROOT_MARKER AddressOf(NullTerm)

#define ScavPush() {\
  *locptr = tad_to_term(m + TadPtr(prev_locptr, t)); \
  prev_locptr = locptr; \
}

/* [BD] Hmm... Think more about this... are XTRs CVA or SVA or both? */
#define MaybeXtrMark(y) {\
  if (IsGVA(y) && GvaIsSva(Gva(y))) { \
    SaveXTR(GvaSva(Gva(y))); \
  } \
}

void copy_location(loc)
     register Term *loc;
{
  copy_locations(loc, 1);
}

void copy_locations(loc,count)
     register Term *loc;
     register long count;
{
  register Term *locptr;
  register Term *prev_locptr;
  register Term result;
  register tval t;
  register uword m;
/*  register heap *local_heapcurrent REGISTER1;*/
/*  register heap *local_heapend REGISTER2;*/


#if 0
#undef heapcurrent
#undef heapend
#undef SaveHeapRegisters
#undef RestoreHeapRegisters

#define heapcurrent	local_heapcurrent
#define heapend		local_heapend
#define SaveHeapRegisters() {\
  glob_heapcurrent = heapcurrent; \
  glob_heapend = heapend; \
}
#define RestoreHeapRegisters() {\
  heapcurrent = glob_heapcurrent; \
  heapend = glob_heapend; \
}
#endif

  if (count == 0)
    return;

  RestoreHeapRegisters();

  prev_locptr = (Term*)ROOT_MARKER;

  /* Note: this assumes that global variables are aligned on
   * word boundaries. (Not valid on some 68k machines...)
   */
  locptr = loc;
  t = REVERSED_LST;

  *locptr = ScavMark(*locptr);
  locptr += count-1;

  for (;;) {
    register tagged_address FW;
    register Term old_t;

    /* Prepare to scavenge the current location. Fetch the old term
     * pointer there, and the mark bit corresponding to the location.
     */
    old_t = *locptr;
    m = Tad(old_t) & MarkBitMask;
    old_t = tad_to_term(Tad(old_t) & ~MarkBitMask);

    /* Handle variables, while dereferencing variable chains: */

    result = NullTerm;
    for(;;) {
      register Reference r;
      if (IsREF(old_t)) {
	r = Ref(old_t);
	FW = ForeWord(r);
	if (IsForwMarked(FW)) {
	  result = TagRef(ForwOf(FW));
	  break;
	}
	if (IsUVA(RefTerm(r))) {
	  DerefUvaEnv(r);
	  if (InCopyUVA(r)) {
	    *locptr = old_t;
	    SaveRef(locptr);
	    result = old_t;
	    break;
	  } else {
	    result = old_t;
	  }
	  break;
	} else if (IsGVA(RefTerm(r))) {
	  DerefGvaEnv(RefGva(r));
	  if (InCopyGVA(r)) {
	    register Term dummy;
	    *locptr = old_t;
	    CopyUnbound(RefTerm(r),dummy);
	    SaveRef(locptr);
	    result = old_t;
	    break;
	  } else {
	    if (VarIsGVA(r) && GvaIsSva(RefGva(r))) {
	      SaveXTR(GvaSva(RefGva(r)));
	    }
	    result = old_t;
	  }
	  break;
	}
      } else {
	break;
      }

      old_t = RefTerm(r);
    }

    if (Eq(result, NullTerm)) {
      SwitchTag(old_t,uvalbl,gvalbl,immlbl,biglbl,fltlbl,lstlbl,strlbl,genlbl);
      /* REF */
	FatalError("impossible VAR tag in copy_location");
        goto endswitch;
 uvalbl:
	result = old_t;	/* This was updated when the structure was copied */
	goto endswitch;
 gvalbl:
	result = old_t;	/* This was updated when the structure was copied */
	goto endswitch;
 immlbl:
 biglbl:
 fltlbl:
	result = old_t;
	goto endswitch;
 lstlbl:
	{
	  List old = Lst(old_t);
	  List new;
	  Term *old_strp, *new_strp, varterm;
	  bool dont_copy = FALSE;
	  FW = ForeWord(old);
	  if (IsForwMarked(FW)) {
	    result = TagLst(ForwOf(FW));
	    goto endswitch;
	  }
#ifdef STRUCT_ENV
	  DerefStrEnv(old);
	  if (!InCopyStr(old)) {
	    result = old_t;
	    goto endswitch;
	  }
#endif
	  NewList(new);
#ifdef STRUCT_ENV
	  StructEnvRef(new)->env = NewEnv(StructEnvRef(old)->env);
#endif
	  old_strp = LstCarRef(old);
	  new_strp = LstCarRef(new);

	  if (IsUnBoundRef(old_strp)) {
	    DerefVarEnv(old_strp);
	    if (InCopyRef(old_strp)) {
	      CopyUnbound(*old_strp,varterm);
	      *new_strp = ScavMark(varterm);
	      SaveTRM(old_strp,new_strp);
	    } else {
	      /* An external variable lives here!
	       * That means the list cell itself is external,
	       * so we set the undo flag so we can throw away the copy later.
	       * The variable may need to be XtrMarked, but there is no
	       * need to put a reference in the copy.
	       */
	      Term y = *old_strp;
	      MaybeXtrMark(y);
	      SaveTRM(old_strp,new_strp); /* A dummy entry on the trmstack */
	      dont_copy = TRUE;
	    }
	  } else {
	    *new_strp = ScavMark(*old_strp);
	    SaveTRM(old_strp,new_strp);	/* This is for the list cell itself */
	  }
	  old_strp++;
	  new_strp++;

#ifdef DEBUG
	  FW = ForeWord(old_strp);
	  if (IsForwMarked(FW)) {
	    WARNING("COPY (scavenger): Forward marked word found in list!");
	    *new_strp = TagRef(old_strp);
	  } else
#endif
	  if (IsUnBoundRef(old_strp)) {
	    DerefVarEnv(old_strp);
	    if (InCopyRef(old_strp)) {
	      CopyUnbound(*old_strp,varterm);
	      *new_strp = varterm;
	      SaveTRM(old_strp,new_strp);
	    } else {
	      Term y = *old_strp;
	      MaybeXtrMark(y);
	      dont_copy = TRUE;
	    }
	  } else {
	    *new_strp = *old_strp;
	  }

	  if (dont_copy) {
	    /*
	     * This was an external list, so we know that no GVAs have
	     * been copied, and that the top entry on the trmstack must
	     * be for the list cell itself.
	     * So just restore everything.
	     */
	    heapcurrent = (heap*)new;
	    *(trmp->loc) = trmp->fixup_data;
	    trmp -= 1;
	    result = old_t;
	    goto endswitch;
	  }

	  ScavPush();
	  locptr = &LstCdr(new);
	  t = REVERSED_LST;
	  continue;
	}
 strlbl:
	{
	  register int i, arity;
	  Structure old = Str(old_t);
	  Structure new;
	  Term *old_strp, *new_strp, varterm;
	  bool dont_copy = FALSE;
	  FW = ForeWord(old);
	  if (IsForwMarked(FW)) {
	    result = TagStr(ForwOf(FW));
	    goto endswitch;
	  }
#ifdef STRUCT_ENV
	  DerefStrEnv(old);
	  if (!InCopyStr(old)) {
	    result = old_t;
	    goto endswitch;
	  }
#endif
	  arity = StrArity(old);
	  NewStructure(new, arity);
	  StrFunctor(new) = StrFunctor(old);
#ifdef STRUCT_ENV
	  StructEnvRef(new)->env = NewEnv(StructEnvRef(old)->env);
#endif
	  old_strp = StrArgRef(old, 0);
	  new_strp = StrArgRef(new, 0);

#ifdef DEBUG
	  FW = ForeWord(old_strp);
	  if (IsForwMarked(FW)) {
	    WARNING("COPY (scavenger): Forward marked word found in struct!");
	    *new_strp = ScavMark(TagRef(old_strp));
	  } else
#endif
	  if (IsUnBoundRef(old_strp)) {
	    DerefVarEnv(old_strp);
	    if (InCopyRef(old_strp)) {
	      CopyUnbound(*old_strp,varterm);
	      *new_strp = ScavMark(varterm);
	      SaveTRM(old_strp,new_strp);
	    } else {
	      /* An external variable lives here!
	       * That means the structure itself is external,
	       * so we set the undo flag so we can throw away the copy later.
	       * The variable may need to be XtrMarked, but there is no
	       * need to put a reference in the copy.
	       */
	      Term y = *old_strp;
	      MaybeXtrMark(y);
	      dont_copy = TRUE;
	    }
	  } else {
	    *new_strp = ScavMark(*old_strp);
	  }

	  for (i = 1; i < arity; i += 1) {
	    old_strp++;
	    new_strp++;
#ifdef DEBUG
	    FW = ForeWord(old_strp);
	    if (IsForwMarked(FW)) {
	      WARNING("COPY (scavenger): Forward marked word found in struct!");
	      *new_strp = TagRef(old_strp);
	    } else
#endif
	    if (IsUnBoundRef(old_strp)) {
	      DerefVarEnv(old_strp);
	      if (InCopyRef(old_strp)) {
		CopyUnbound(*old_strp,varterm);
		*new_strp = varterm;
		SaveTRM(old_strp,new_strp);
	      } else {
		Term y = *old_strp;
		MaybeXtrMark(y);
		dont_copy = TRUE;
	      }
	    } else {
	      *new_strp = *old_strp;
	    }
	  }

	  if (dont_copy) {
	    /*
	     * This was an external structure, so we know that no GVAs have
	     * been copied, and we haven't forwarded the structure itself yet.
	     * So just restore everything.
	     */
	    heapcurrent = (heap*)new; /* [BD] WARNING! This may waste heap! */
	    result = old_t;
	    goto endswitch;
	  }

	  SaveTRM(old,new);
	  ScavPush();
	  locptr = &StrArgument(new, arity-1);
	  t = REVERSED_STR;
	  continue;
	}
 genlbl:
	{
	  Generic new;
	  Generic old = Gen(old_t);
	  FW = ForeWord(old);
	  if (IsForwMarked(FW)) {
	    result = TagGen(ForwOf(FW & ~AltMarkBitMask));
	    goto endswitch;
	  }
	  /* If a generic object has an environment field, it is the
	   * responsibility of the "new" method to check it and return
	   * NULL if the object was external.
	   */

	  SaveHeapRegisters();
	  new = old->method->new(old);
	  if (new == NULL) {
	    result = old_t;
	  } else {
	    new->method = old->method;
	    SaveTRM(old,new);		/* [BD] Here we smash method slot */
	    new->method->copy(old,new);	/* [BD] so we must use new here! */
	    result = TagGen(new);
	  }
	  RestoreHeapRegisters();
	  goto endswitch;
	}
      }
    endswitch:

    *locptr = result;

    while (m) {
      if (prev_locptr == (Term*)ROOT_MARKER) {
	SaveHeapRegisters();
	return;
      } else {
	register Term temp;
	if (t == REVERSED_LST) {
	  result = TagLst(locptr);
	} else if (t == REVERSED_STR) {
	  result = TagStr(locptr-OffsetToArgs);
	} else {
	  FatalError("copy_location pointer flip detects unsupported tag");
	}

	/* Pop the threaded stack and move up one level */
	temp = *prev_locptr;
	*prev_locptr = result;
	locptr = prev_locptr;
	t = PTagOf(temp);
	m = Tad(temp) & MarkBitMask;
	prev_locptr = (Term *) (AddressOf(temp) & ~MarkBitMask);
      }
    }
    locptr -= 1;
  }
}

void copy_tuple(old_strp,new_strp,arity)
     Term *old_strp;
     Term *new_strp;
     register long arity;
{
  register tagged_address FW;
  register long i;
  Term varterm;

  for (i = 0; i < arity; i += 1) {
#ifdef DEBUG
    FW = ForeWord(old_strp);
    if (IsForwMarked(FW)) {
      WARNING("COPY (scavenger): Forward marked word found in struct!");
      *new_strp = TagRef(old_strp);
    } else
#endif
    if (IsUnBoundRef(old_strp)) {
      DerefVarEnv(old_strp);
      if (InCopyRef(old_strp)) {
	CopyUnbound(*old_strp,varterm);
	*new_strp = varterm;
	SaveTRM(old_strp,new_strp);
      } else {
	FatalError("COPY (scavenger): External variable found in tuple!");
      }
    } else {
      *new_strp = *old_strp;
    }
    old_strp++;
    new_strp++;
  }
  copy_locations(new_strp-arity, arity);
}

void copy_todo(x)
     Gvainfo x;
{
  SaveGN(x);
  return;
}

void copy_xtodo(x)
     Gvainfo x;
{
  SaveXGN(x);
  return;
}

void copy_forward(old,new)
     void *old;
     void *new;
{
  SaveTRM(old,new);
  return;
}
