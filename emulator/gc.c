/* $Id: gc.c,v 1.70 1994/04/18 15:04:22 bjornc Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "gc.h"
#include "unify.h"
#include "initial.h"
#include "storage.h"
#include "trace.h"
#include "config.h"
#include "time.h"
#include "display.h"
#include "statistics.h"
#include "debug.h"
#include "regdefs.h"
#include "error.h"

/* from bignum.c */
extern void	bignum_gc();
extern Bignum	new_bignum();

/* local prototypes */
void		gc_exstate();
Term		gc_term();
void		gc_variable();
void		gc_saved_refs();
void 		gc_tree();
void		gc_context();
void		gc_registers();
void		gc_constraints();
void		gc_unifier();
void 		gc_continuations();
void            gc_suspensions();
void		gc_a_registers();
void		gc_trailed_values();
void 		gc_install();
void 		gc_branch();
void 		gc_branch_andboxes();
void		gc_undo();
void		gc_close();


typedef struct gcsuspentry {
  suspension	*susp;
  suspension	**to;
} gcsuspentry;

typedef struct gcstate {
  gcsuspentry	*start;
  gcsuspentry	*end;
  gcsuspentry	*top;  
  gcsuspentry	*stack;
  long		gcrefindex;
  Generic       *gnstack;
  Generic       *gnend;
  Generic       *gnp;
} gcstate;

#define InitState(GCST) \
{\
  (GCST).end = NULL;\
  SetupStack(gcsuspentry,(GCST).stack,(GCST).end,(GCST).top,AKLGCSUSP);\
  (GCST).start = &((GCST).stack[-1]);\
  (GCST).top = (GCST).start;\
  (GCST).gcrefindex = gcreftop - gcrefstack;\
  (GCST).gnend = NULL;\
  SetupStack(Generic,(GCST).gnstack,(GCST).gnend,(GCST).gnp,AKLCOPYXGN);\
}

#define SaveSusp(GCST,SUSP,TO) \
{\
  (GCST)->top++;\
  if((GCST)->top == (GCST)->end) {\
    reinit_gcsuspstack(GCST);\
  }\
  (GCST)->top->susp = (SUSP);\
  (GCST)->top->to = (TO);\
}

/* Stacks for Reference copying */

Term	**gcrefstart;
Term	**gcrefend = NULL;
Term	**gcreftop;
Term	**gcrefstack;
long	debug_gcrefstacksize;
long	debug_maxgcrefstacksize = 0;

#ifdef GC_DEBUG
int stat_unif[20];
int stat_susp[20];  
int stat_acont[20];
int stat_ccont[20];
int abox;
int cbox;
#endif

#define SaveRef(REFLOC) \
{\
  gcreftop++;\
  if(gcreftop == gcrefend) {\
    reinit_gcrefstack();\
  }\
  *gcreftop = (REFLOC);\
}

#define SaveGN(GCST,OLD) \
{\
     (GCST)->gnp++;\
     if((GCST)->gnp == (GCST)->gnend) {\
       reinit_gnstack(GCST);\
     }\
     *((GCST)->gnp) = OLD;\
}

#define SetupStack(TYPE,STACK,END,TOP,SIZE) \
{\
    if ((END) == NULL) {\
      (STACK) = (TYPE*) malloc(sizeof(TYPE)*(SIZE));\
      if ((STACK) == NULL) {\
        FatalError("malloc in gc failed");\
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

void reinit_gcrefstack()
{
  ReInitStackProc(Term*,gcrefstack,gcrefend,gcreftop,"gcrefstack");
  gcrefstart = &gcrefstack[-1];
}

void reinit_gcsuspstack(gcst)
    gcstate *gcst;
{
  ReInitStackProc(gcsuspentry,gcst->stack,gcst->end,gcst->top,"gcsuspstack");
  gcst->start = &gcst->stack[-1];
}

void reinit_gnstack(gcst)
     gcstatep gcst;
{
  ReInitStackProc(Generic,gcst->gnstack,gcst->gnend,gcst->gnp,"gnstack");
}

#define ResetGN(GCST) \
  while((GCST)->gnp != (GCST)->gnstack) {\
        Generic gn = *(GCST)->gnp--; \
        ((gvainfo*)Forw(gn))->method->gc_external(gn,GCST); \
}

void gc(exs)
     exstate *exs;
{
  exstate *super, *active, *tmp;

#ifdef METERING
  int tmpt;
#endif 
  
#ifdef GC_DEBUG
  int insize;
  int outsize;
#endif
  
#ifdef GC_DEBUG
  insize = heap_size();
  fprintf(stderr, "{GC, initial size %dK }\n", insize/1024);
  {
    int i;
    for(i = 0; i < 20; i++)
      stat_unif[i] = 0;
    for(i = 0; i < 20; i++)
      stat_susp[i] = 0;
    for(i = 0; i < 20; i++)
      stat_acont[i] = 0;
    for(i = 0; i < 20; i++)
      stat_ccont[i] = 0;
    abox = 0;
    cbox = 0;
  }
#endif    
  
  
#ifdef METERING
  tmpt = systime();
#endif
  
  
  active = exs;
  
  SetupStack(Term*, gcrefstack,gcrefend,gcreftop,AKLGCREFS);

  gcrefstart = &gcrefstack[-1];
  gcreftop = gcrefstart;
  debug_gcrefstacksize = 0;

  /* deinstall the exstates */
  for(tmp = active; tmp != NULL; tmp = tmp->father) {
    gc_undo(tmp);
  }
  
  gc_begin();
  
  for(super = active; super->father != NULL; super = super->father);
  
  gc_exstate(super);
  
  /* update the variable name table */
  gc_varname();
  
  /* reinstall the the current branch */

  for(tmp = active; tmp != NULL; tmp = tmp->father) {
    gc_branch(tmp->andb,tmp);
  }

  gc_end();
  
  free(gcrefstack);

  gcrefend = NULL;

#ifdef METERING
  gctime = gctime + (systime() - tmpt);
#endif
  
  if (debug_gcrefstacksize > debug_maxgcrefstacksize) {
    debug_maxgcrefstacksize = debug_gcrefstacksize;
#if 0
    printf("<<GC REF stack high score: %ld>>\n", debug_maxgcrefstacksize);
#endif
  }

#ifdef GC_DEBUG
  {
    outsize = heap_size();
    fprintf(stderr, "{GC, compressed size %dK (%d%%)}\n",
	    outsize/1024, (outsize*100)/insize);
  }
  {
    fprintf(stderr, "{Andboxes: %d, Choiceboxes: %d}\n", abox, cbox);
  }
  {
    int i;
    fprintf(stderr, "{Coice-cont : ");
    for(i = 0; i < 19; i++) {
      fprintf(stderr, "%d, ", stat_ccont[i]);
    }
    fprintf(stderr, "%d }\n", stat_ccont[19]);
  }
  {
    int i;
    fprintf(stderr, "{And-cont   : ");
    for(i = 0; i < 19; i++) {
      fprintf(stderr, "%d, ", stat_acont[i]);
    }
    fprintf(stderr, "%d }\n", stat_acont[19]);
  }
  {
    int i;
    fprintf(stderr, "{Suspensions: ");
    for(i = 0; i < 19; i++) {
      fprintf(stderr, "%d, ", stat_susp[i]);
    }
    fprintf(stderr, "%d }\n", stat_susp[19]);
  }
  {
    int i;
    fprintf(stderr, "{Unifiers:    ");
    for(i = 0; i < 19; i++) {
      fprintf(stderr, "%d, ", stat_unif[i]);
    }
    fprintf(stderr, "%d }\n", stat_unif[19]);
  }  
#endif
}



/* All installed bindings must be undone. This means that
 * the variable on the trail must me unbound (and their values
 * recorded) and that all installed constrainst (unify)
 * are undone.
 */

void gc_undo(exs)
     exstate *exs;
{
  andbox *current;
  choicebox *root;
  context *cntx;
  trailentry *trlc, *tend;
  
  current = exs->andb;
  
  if(current == NULL)
    return;		/* Nothing installed, nothing to do */

  cntx = exs->context.current;
  trlc = exs->trail.current;
  
  /*
   * Zap the trail.
   */

  tend = exs->trail.start;

  for(; trlc != tend; trlc--) {
    Reference v;
    Term value;
    v = Ref(trlc->var);
    value = RefTerm(v);
    RefTerm(v) = trlc->val;
    trlc->val = value;
  }

  while (current != NULL) 
  {
    root = current->father;  
    UndoUnifier(current);
    UndoConstraints(current);
    cntx--;
    current = root->father;
  }
}

void gc_exstate(exs)
     exstate *exs;
{
  choicebox *root, *groot;
  choicecont *cont;
  gcstate gcst;

  InitState(gcst);
  
  root = exs->root;
  
  /* The copy of root is created */
  NewChoicebox(groot);
  
#ifdef TRACE
  NewChoiceCont(cont,0);
  groot->cont = cont;
  groot->cont->arity = 0;
  groot->cont->instr.label = NULL;
  groot->trace=NOTRACE;
  groot->next_clno=1;
#else
  groot->cont = NULL;
#endif
  groot->father = root->father;
  groot->next = root->next;
  groot->previous = root->previous;
  groot->def = root->def;
  groot->type = root->type;
  
#ifdef TRACE
  groot->id = root->id;
  groot->trace = root->trace;
#endif
  
  gc_tree(root,groot,&gcst);
  
  gc_a_registers(exs,&gcst);

  gc_trailed_values(exs,&gcst);

  gc_saved_refs(exs,&gcst);

  newcontext(exs);
  /* restore the context */
  gc_context(exs);

  /* update the close list, deallocate all objects that have not been copied */
  gc_close(exs,&gcst);

  /* Traverse the refstack again,
   * since gc_close() may have tried to copy variables:
   */
  gc_saved_refs(exs,&gcst);

  resetcontext(exs);
  
  ResetGN(&gcst);

  /* copy the suspensions */
  gc_suspensions(&gcst);
  
  /* update the exstate */
  exs->root = groot;
  if(exs->andb !=NULL)
    exs->andb = exs->andb->previous;
  if(exs->insert != NULL)
    exs->insert = exs->insert->previous;
  
  free(gcst.stack);
}


/* For each context the task entries must be updated,
 * only entries that refer to copied structures will
 * remain. The task stack is compacted using the hare
 * and turtle.
 */

void gc_context(exs)
     exstate *exs;
{
  context *cntx;
  task	*hare, *turtle;
  recall *rhare, *rturtle;
  wake *whare, *wturtle;
  
  turtle = exs->task.start;
  hare = turtle;
  
  rturtle = exs->recall.start;
  rhare = rturtle;
  
  wturtle = exs->wake.start;
  whare = wturtle;
  
  for(cntx = exs->context.start; cntx != exs->context.current;) {
    cntx++;
    
    /* The trail remains untouched */
    
    /* recall entries that are pointing to an uncopied choicebox are removed */
    while(rhare != cntx->recall) {
      if(rhare->chb->father == NULL) {
	rturtle->chb = rhare->chb->previous;
	rturtle++;
      }
      rhare++;
    }
    cntx->recall = rturtle;
    
    /* wake entries that are pointing to an uncopied andbox are removed */
    while(whare != cntx->wake) {
      if(whare->andb->father == NULL) {
	wturtle->andb = whare->andb->previous;
	wturtle++;
      }
      whare++;
    }
    cntx->wake = wturtle;
    
    /* task entries that are pointing to an uncopied andbox are removed */
    while(hare != cntx->task){
      switch(hare->type) {
      case PROMOTE:
	if(hare->value.andbox->father == NULL) {
	  turtle->type = PROMOTE;
	  turtle->value.andbox = (andbox*)hare->value.andbox->previous;
	  turtle++;
	}
	break;
      case SPLIT:
	if(hare->value.andbox->father == NULL) {
	  turtle->type = SPLIT;
	  turtle->value.andbox = (andbox*)hare->value.andbox->previous;
	  turtle++;
	}	  
	break;
      case ROOT:
	turtle->type = ROOT;
	turtle->value.code = hare->value.code;
	turtle++;
	break;
      case START:
	turtle->type = START;
	turtle->value.code = hare->value.code;
	turtle++;
	break;
      default:
	FatalError("wrong type of task");
      }
      hare++;
    }
    
    cntx->task = turtle;
  }
  exs->wake.current = wturtle;
  exs->recall.current = rturtle;
  exs->task.current = turtle;  
}


/* The suspension list are copied last. Each entry contains a pointer to
 * a suspension list and a pointer to the new locatio.
 */


void gc_suspensions(gcst)
     gcstate *gcst;
{
  gcsuspentry *entryp;
  suspension **to;
  suspension *first;
  suspension *next, *prevs, *news;
  
  for(entryp = (gcst->top); entryp != gcst->start; entryp--) {
#ifdef GC_DEBUG
    int  n = 0;
#endif    
    first = entryp->susp;
    to = entryp->to;
    prevs = NULL;
    for(next = first; next != NULL; next = next->next) {
#ifdef GC_DEBUG
      n++;
#endif    
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
	  }
	  break;
	}
      case ANDB:
	{
	  andbox *andb, *newa;
	  andb = next->suspended.andb;
	  if(andb->father == NULL){
	    newa = andb->previous;
	    NewSuspension(news);
	    news->type = ANDB;
	    news->suspended.andb = newa;
	    if(prevs == NULL)
	      *to = news;
	    else
	      prevs->next = news;
	    prevs = news;
	  }
	  break;
	}
      default:
	FatalError("GC: wrong type in suspenion");
      }
    }
    if(prevs == NULL)
      *to = NULL;
    else
      prevs->next = NULL;
#ifdef GC_DEBUG
    if(n < 19) 
      stat_susp[n]++;
    else
      stat_susp[19]++;
#endif
  }
}



/* In order to handle auto closing of ports,
 * all ports must be traversed after a gc.
 * Ports that have been copied remain in the
 * linked list of living ports. Uncopied ports
 * are not longer referenced. If the stream of
 * an uncopied port is copied, that stream must
 * be closed. A close choice-box is added
 * to the andbox that is home of the stream and
 * a recall entry is added to the task stack.
 */


/* This is a help function that copies a generic or gvainfo */

generic *copy_close_obj(gcst, gen, type)
     gcstatep gcst;
     generic *gen;
     enum close_obj_type type;
{
  if(type == GENERIC_CLOSE) {
    generic *ngen;
    ngen = gen->method->new(gen);
    ngen->method = gen->method;
    Link(gen,ngen);
    ngen->method->gc(gen,ngen,gcst);
    return ngen;
  } else {
    /* It's a gvainfo thing. Do the same as for generic objects. */
    gvainfo *gva;
    gvainfo *ngva;
    gva = (gvainfo*)gen;
    DerefGvaEnv(gva);
    ngva = gva->method->new(gva);
    ngva->method = gva->method;
    ngva->env = NewEnv(gva->env);
    Link(gva,ngva);
    ngva->method->gc(gva,ngva,gcst);
    return (generic*)ngva;
  }
}


struct close_request {
  envid			*env;
  enum close_obj_type	type;
  void			*obj;
};

void gc_close(exs, gcst)
     exstate *exs;
     gcstate *gcst;
{
  close_entry *hare, *close;
  recall *wakp;
  int i;
  struct close_request save[AKLGCGEN];

  i = 0;
  
  close = exs->close;
  exs->close = NULL;
  
  
  for(hare = close; hare != NULL; hare = hare->next) {
    generic *gen;
    struct close_request c;
    c.obj = hare->obj;
    if(!IsCopied(c.obj)) {
      /* The generic object is not touched.
       * Call its deallocate method. The return value determines
       * if it should be scheduled for closing.
       */
      gen = (generic*)c.obj;	/* Note: gvainfo is treated as a generic too */
      c.env = gen->method->deallocate(gen);
      if(c.env != NULL) {
	/* The object must be closed */
	DerefGenEnv((envid*)&c.env);
	if(Home(c.env)->father == NULL) {
	  /* the home and-box is copied (live) */
	  /* so we can schedule it for closing. */
	  /* [BD] handle icky situation when the home and-box is not */
	  /* in the leftmost branch of the root choice-box. In that case */
	  /* defer closing until the next solution is requested. */

	  andbox *andb_next, *andb = Home(NewEnv(c.env));
	  while ((andb_next = andb->father->father) != NULL)
	    andb = andb_next;

	  if (andb->previous != NULL) {
	    /* keep the object, defer closing */
	    gen = copy_close_obj(gcst, gen, hare->type);
	    if (hare->type == GENERIC_CLOSE)
	      add_generic_to_close(gen,exs);
	    else
	      add_gvainfo_to_close((gvainfo*)gen, exs);
	    continue;
	  }
	  c.type = hare->type;
	  save[i] = c;
	  i += 1;
	  if(i == AKLGCGEN)
	    FatalError("GC: Generic stack full");
	} else {
	  /* the home and-box is dead */
	  gen->method->kill(gen);
	}
      }
    } else {
      /* the generic object has been copied, this
       * implies that it is part of the living tree.
       * add the copy to the close list.
       */
      if(hare->type == GENERIC_CLOSE)
	add_generic_to_close((generic*)(Forw(c.obj)),exs);
      else
	add_gvainfo_to_close((gvainfo*)(Forw(c.obj)),exs);
    }
  }
  
  if(i == 0)
    return;
  
  /* shift the recall stack above first context */
  wakp =  shift_recall(exs,i,1);
  
  /* enter new recall entries */
  
  while (i != 0) {
    generic *gen;
    choicebox *chb;
    choicecont *cont;
    andbox *andb;
    bool (*cinfo)();
    envid *newenv;
    
    i -= 1;

    NewChoiceCont(cont,1);

    newenv = NewEnv(save[i].env);

    gen = copy_close_obj(gcst, (generic*)save[i].obj, save[i].type);

    if(save[i].type == GENERIC_CLOSE) {
      cont->arg[0] = TagGen(gen);
      cinfo = gen->method->close;
    } else {
      /* It's a gvainfo thing */
      Reference tmp;
      gvainfo *gva = (gvainfo*)gen;
      /* Make a new variable cell to hold it */
      NewReference(tmp);
      RefTerm(tmp) = TagGva(gva);
      cont->arg[0] = TagRef(tmp);
      cinfo = gva->method->close;
    }

    cont->instr.cinfo = cinfo;
    cont->arity = 1;

    andb = Home(newenv);
    
    NewChoicebox(chb);

    chb->def = NULL;
    chb->father = andb;
    chb->cont = cont;
    chb->tried = NULL;
    chb->next = andb->tried;
    chb->previous = NULL;
    chb->type = 0;

#ifdef TRACE
    chb->trace=NOTRACE;		/* ATTENTION */
    chb->next_clno=0;
    chb->id = identity++;
#endif    

    if(andb->tried != NULL)
      andb->tried->previous = chb;
    
    andb->tried = chb;
    
    
    wakp->chb = chb;
    wakp++;
  }
}

void gc_a_registers(exs,gcst)
     exstate *exs;
     gcstate *gcst;
{
  gc_locations(exs->areg, exs->arity, gcst);
}
  

void gc_trailed_values(exs,gcst)
     exstate *exs;
     gcstate *gcst;
{
  trailentry *trlc, *tend;

  trlc = exs->trail.start;
  tend = exs->trail.current;

  gc_locations(trlc+1, 2*(tend-trlc), gcst);
}

void gc_restore_trail(exs)
     exstate *exs;
{
  trailentry *trlc, *tend;

  trlc = exs->trail.start;
  tend = exs->trail.current;

  while (trlc != tend) {
    Reference v;
    Term value;
    trlc++;
    v = Ref(trlc->var);
    value = RefTerm(v);
    RefTerm(v) = trlc->val;
    trlc->val = value;
  }
}

void gc_branch(to,exs)
     andbox* to;
     exstate *exs;
{
  if(to == NULL)
    return;			/* Nothing to install */

  gc_restore_trail(exs);
  gc_branch_andboxes(to,exs);
}

void gc_branch_andboxes(to,exs)
     andbox* to;
     exstate *exs;
{
  if(to->father->father != NULL)
    gc_branch_andboxes(to->father->father,exs);
  
  gc_install(to,exs);
}

void gc_install(andb,exs)
     andbox *andb;
     exstate *exs;
{
  constraint *constr;
  unifier *unif;

  for(unif = andb->unify; unif != NULL; unif = unif->next) {
    BindUnifier(unif);
  }
  
  for(constr = andb->constr; constr != NULL; constr = constr->next) {
    constrtable *meth = constr->method;
    if (meth != NULL)			/* ports may have zapped constraints */
      meth->reinstall(constr,andb);
  }
}

void gc_tree(old,new,gcst)
     choicebox *old, *new;
     gcstate *gcst;
{
  andbox *preva, *templa;
  choicebox *prevc, *templc;
  
  preva = NULL;
  
  for(templa = old->tried; templa != NULL; templa = templa->next) {
    andbox *andb;

#ifdef GC_DEBUG
    abox++;
#endif
    
    NewAndbox(andb);
    
    andb->status = templa->status;
    andb->father = new;
    andb->env = NULL;
    andb->cont = NULL;
    andb->previous = preva;
    
#ifdef TRACE
      andb->trace = templa->trace;
      andb->clno = templa->clno;
      andb->id = templa->id;
#endif
    
    /* mark the copied */
    templa->father = NULL;
    templa->previous = andb;
    
    if(preva != NULL)
      preva->next = andb;
    else
      new->tried = andb;
    
      gc_continuations(templa,andb, gcst);

      gc_unifier(templa, andb, gcst);
      
      gc_constraints(templa, andb, gcst);

      prevc = NULL;

      for(templc = templa->tried; templc != NULL; templc = templc->next) {
	choicebox *chb;

#ifdef GC_DEBUG
        cbox++;
#endif
	
	NewChoicebox(chb);
	if(templc->cont != NULL) {
	  choicecont *cont;
	  NewChoiceCont(cont,templc->cont->arity);

          chb->cont = cont;

#ifdef TRACE
          chb->trace = templc->trace;
          chb->next_clno = templc->next_clno;
#endif

#ifdef GC_DEBUG
	  {
	    int i;
	    i = templc->cont->arity;
	    if(i < 19)
	      stat_ccont[i]++;
	    else
	      stat_ccont[19]++;
	  }
#endif
	  gc_registers(templc->cont, cont, gcst);

	  chb->cont->instr.label = templc->cont->instr.label;
	} else {
	  chb->cont = NULL;
	}
#ifdef TRACE
        chb->id = templc->id;
        chb->trace = templc->trace;
#endif
	chb->type = templc->type;
	chb->previous = prevc;

	if(prevc != NULL)
	  prevc->next = chb;
	else
	  andb->tried = chb;

	chb->def = templc->def;
	chb->father = andb;

        if(templc->tried != NULL)
	  gc_tree(templc, chb, gcst);
        else
	  chb->tried = NULL;

	/* mark the copy */
	templc->father = NULL;
	templc->previous = chb;
	
	prevc = chb;
	
      }
      if(prevc != NULL)
	prevc->next = NULL;
      else
	andb->tried = NULL;

      preva = andb;
    }
  if(preva != NULL)
    preva->next = NULL;
  else
    new->tried = NULL;
}

void gc_registers(templc, chb, gcst)
     choicecont *templc, *chb;
     gcstate *gcst;
     
{
    Term 	*targ, *carg;
    int 	arity, i;
    
    arity = (templc)->arity;
    targ = (templc)->arg;
    carg = chb->arg;
    
    for(i = 0 ; i != arity ; i++ ) {
	if(Eq(targ[i], NullTerm)) {
	    carg[i] = NullTerm;
	} else {
	    carg[i] = targ[i];
   	    gc_locations(&carg[i],1,gcst);
	}
    }
    chb->arity = arity;
}


void gc_continuations(templa,andb,gcst)
     andbox *templa, *andb;
     gcstate *gcst;
{
    andcont	*templt,  *cont, *last;
    Term 	*tyreg, *cyreg;
    int		ysize, i;

    
    last = (andb)->cont;
    
    for(templt = (templa)->cont; templt != NULL; templt = templt->next) {
      NewAndCont(cont, templt->ysize);

      cont->next = NULL;
      cont->label = templt->label;
      cont->ysize = templt->ysize;
#ifdef TRACE
      cont->trace = templt->trace;
      cont->def = templt->def;
      cont->clno=templt->clno;
      {
	choicecont *ch_cont;

	NewChoiceCont(ch_cont,templt->choice_cont->arity);
	cont->choice_cont=ch_cont;
	gc_registers(templt->choice_cont,ch_cont,gcst);
      }
#endif

      tyreg = templt->yreg;
      ysize = templt->ysize;
      cyreg = cont->yreg;
#ifdef GC_DEBUG
      if(ysize < 19)
	  stat_acont[ysize]++;
	else
	  stat_acont[19]++;
#endif
      for(i = 0; i != ysize ; i++ ) {
	if(Eq(tyreg[i], NullTerm)) {
	  cyreg[i] = NullTerm;
	} else {
	  cyreg[i] = tyreg[i];
	  gc_locations(&cyreg[i],1,gcst);
	}
      }
      if(last != NULL) {
	last->next = cont;
      } else {
        (andb)->cont = cont;
      }
      last = cont;
    }
}


void gc_constraints(templa,andb,gcst)
     andbox *templa, *andb;
     gcstate *gcst;
{
  constraint *constr, *prevc, *newc;
  
  prevc = NULL;
    
  for(constr = templa->constr; constr != NULL; constr = constr->next) {
    constrtable *meth = constr->method;
    if (meth != NULL) {
      if (IsForwMarked(meth))
	newc = ForwOf((tagged_address)meth);
      else {
	newc = (meth->gc)(constr,gcst);
	newc->method = meth;
	Link(constr,newc);
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


void gc_unifier(templa,andb,gcst)
     andbox *templa, *andb;
     gcstate *gcst;
{
  unifier *unif, *prevu, *newu;
  Term var, trm;
#ifdef GC_DEBUG
  int n = 0;
#endif
  
  prevu = NULL;
    
  for(unif = templa->unify; unif != NULL; unif = unif->next) {
#ifdef GC_DEBUG
    n++;
#endif    
    NewUnifier(newu);

    var = unif->arg1;
    trm = unif->arg2;

    newu->arg1 = var;
    newu->arg2 = trm;

    gc_variable(&newu->arg1, gcst);

    if(IsVar(trm)) {
      gc_variable(&newu->arg2, gcst);
    } else {
      gc_locations(&newu->arg2, 1, gcst);
    }

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
#ifdef GC_DEBUG
  if(n < 20) 
    stat_unif[n]++;
  else
    stat_unif[19]++;
#endif
}

#ifdef GC_DEBUG
#define CountUVA stat_susp[0]++;
#else
#define CountUVA
#endif



#define CopyUnbound(GCST,OLD,NEW) \
{\
  register Term ZZZt = (OLD);\
  if (IsUVA(ZZZt)) {\
    CountUVA;\
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
      suspension *ZZZsus;\
      NewSvainfo(ZZZn);\
      ZZZn->method = NULL;\
      ZZZn->susp = NULL;\
      ZZZn->env = NewEnv(ZZZs->env);\
      (NEW) = TagGva(ZZZn);\
      ZZZsus = ZZZs->susp;\
      if (ZZZsus != NULL) {\
	SaveSusp(GCST,ZZZsus,&ZZZn->susp);\
      }\
      Link(ZZZs,ZZZn);\
    } else {\
      gvainfo *ZZZn;\
      SaveHeapRegisters();\
      ZZZn = ZZZg->method->new(ZZZg);\
      ZZZn->method = ZZZg->method;\
      ZZZn->env = NewEnv(ZZZg->env);\
      Link(ZZZg,ZZZn);\
      ZZZn->method->gc(ZZZg,ZZZn,GCST);\
      RestoreHeapRegisters();\
      (NEW) = TagGva(ZZZn);\
    }\
  }\
}

void gc_saved_refs(exs,gcst)
     exstate *exs;
     gcstate *gcst;
{
  Term **cached_gcrefstack = gcrefstack;
  Term **local_gcrefstart = &gcrefstack[gcst->gcrefindex];
  long refstacksize = gcreftop - gcrefstart;
  if (refstacksize > debug_gcrefstacksize)
    debug_gcrefstacksize = refstacksize;

  while (gcreftop != local_gcrefstart) {
    Term *loc, old_t, result;
    uword FW;

    loc = *gcreftop;
    gcreftop--;
    old_t = *loc;

    if (!IsREF(old_t)) {
      FatalError("GC: Non-REF on ref stack");
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
	  DerefVarEnv(old);
	  CopyUnbound(gcst,RefTerm(old),varterm);
	  Link(old,new);
	  RefTerm(new) = varterm;
	  /* Check for relocation (find a cheaper method?) */
	  if (cached_gcrefstack != gcrefstack) {
	    cached_gcrefstack = gcrefstack;
	    local_gcrefstart = &gcrefstack[gcst->gcrefindex];
	  }
	} else {
	  FatalError("GC: bound variable on refstack");
	}
	result = TagRef(new);
      }
    }

    *loc = result;
  }
}


void gc_variable(loc,gcst)
     register Term *loc;
     gcstate *gcst;

{
  register Term arg = *loc;

  if (IsREF(arg)) {
    register uword FW;
    register Reference var;
    register Term dummy;
    /* Dereference while checking forwarding pointers: */
    var = Ref(arg);
    FW = ForeWord(var);
    if (IsForwMarked(FW)) {
      *loc = TagRef(ForwOf(FW));
      return;
    } else {
      if (IsUnBoundRef(var)) {
	DerefVarEnv(var);
	*loc = TagRef(var);
	CopyUnbound(gcst,RefTerm(var),dummy);
	SaveRef(loc);
	return;
      } else {
	for(;;) {
	  register Reference new;
	  arg = RefTerm(var);
	  if (!IsREF(arg)) {
	    NewReference(new);
	    RefTerm(new) = arg;
	    *loc = TagRef(new);
	    gc_locations(new,1,gcst);
	    return;
	  } else {
	    var = Ref(arg);
	    FW = ForeWord(var);
	    if (IsForwMarked(FW)) {
	      NewReference(new);
	      RefTerm(new) = TagRef(ForwOf(FW));
	      *loc = TagRef(new);
	      return;
	    } else {
	      if (IsUnBoundRef(var)) {
		NewReference(new);
		RefTerm(new) = arg;
		*loc = TagRef(new);
		DerefVarEnv(var);
		CopyUnbound(gcst,RefTerm(var),dummy);
		SaveRef(new);
		return;
	      }
	    }
	  }
	}
      }
    }
  } else {
    FatalError("GC: non-variable in gc_variable");
  }
}



#define ROOT_MARKER AddressOf(NullTerm)

#define ScavPush() {\
  *locptr = tad_to_term(m + TadPtr(prev_locptr, t)); \
  prev_locptr = locptr; \
}

void gc_location(loc,gcst)
     register Term *loc;
     gcstate *gcst;
{
  gc_locations(loc,1,gcst);
}

void gc_locations(loc,count,gcst)
     register Term *loc;
     register long count;
     gcstate *gcst;
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
	  CountUVA;
	  SaveRef(locptr);
	  result = old_t;
	  break;
	} else if (IsGVA(RefTerm(r))) {
	  register Term dummy;
	  DerefGvaEnv(RefGva(r));
	  CopyUnbound(gcst,RefTerm(r),dummy);	/* Copy gvainfos immediately */
	  SaveRef(locptr);
	  result = old_t;
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
	FatalError("GC: impossible REF tag in gc_location");
        goto endswitch;
 uvalbl:
	result = old_t;	/* This was updated when the structure was copied */
	goto endswitch;
 gvalbl:
	result = old_t;	/* This was updated when the structure was copied */
	goto endswitch;
 immlbl:
	result = old_t;
	goto endswitch;
 biglbl:
	{
	  Bignum old = Big(old_t);
	  if(IsCnst(old)) {
	    result = old_t;
	  } else {
	    Bignum new;
	    FW = ForeWord(old);
	    if (IsForwMarked(FW)) {
	      result = TagBig(ForwOf(FW));
	      goto endswitch;
	    }
	    SaveHeapRegisters();
	    new = new_bignum(old);
	    Link(old,new);
	    bignum_gc(old,new,gcst);
	    RestoreHeapRegisters();
	    result = TagBig(new);
	  } 
	  goto endswitch;
	}
 fltlbl:
	{
	  Fp old = Flt(old_t);
	  if(IsCnst(old)) {
	    result = old_t;
	  } else {
	    Fp new;
	    FW = ForeWord(old);
	    if (IsForwMarked(FW)) {
	      result = TagFlt(ForwOf(FW));
	      goto endswitch;
	    }
	    NewFloat(new);
	    FltVal(new) = FltVal(old);
	    new->tag2 = old->tag2;
	    Link(old,new);
	    result = TagFlt(new);
	  } 
	  goto endswitch;
	}
 lstlbl:
	{
	  List old = Lst(old_t);
	  List new;
	  Term *old_strp, *new_strp, varterm;
	  FW = ForeWord(old);
	  if (IsForwMarked(FW)) {
	    result = TagLst(ForwOf(FW));
	    goto endswitch;
	  }
	  NewList(new);
#ifdef STRUCT_ENV
	  DerefStrEnv(old);
	  StructEnvRef(new)->env = NewEnv(StructEnvRef(old)->env);
#endif
	  old_strp = LstCarRef(old);
	  new_strp = LstCarRef(new);

	  if (IsUnBoundRef(old_strp)) {
	    DerefVarEnv(old_strp);
	    CopyUnbound(gcst,*old_strp,varterm);
	    *new_strp = ScavMark(varterm);
	    Link(old_strp,new_strp);
	  } else {
	    *new_strp = ScavMark(*old_strp);
	    Link(old_strp,new_strp);	/* This is for the list cell itself */
	  }
	  old_strp++;
	  new_strp++;

#ifdef GC_DEBUG
	  FW = ForeWord(old_strp);
	  if (IsForwMarked(FW)) {
	    WARNING("GC (scavenger): Forward marked word found in list!");
	    *new_strp = TagRef(old_strp);
	  } else
#endif
	  if (IsUnBoundRef(old_strp)) {
	    DerefVarEnv(old_strp);
	    CopyUnbound(gcst,*old_strp,varterm);
	    *new_strp = varterm;
	    Link(old_strp,new_strp);
	  } else {
	    *new_strp = *old_strp;
	  }
	  old_strp++;
	  new_strp++;

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
	  FW = ForeWord(old);
	  if (IsForwMarked(FW)) {
	    result = TagStr(ForwOf(FW));
	    goto endswitch;
	  }
	  arity = StrArity(old);
	  NewStructure(new, arity);
	  StrFunctor(new) = StrFunctor(old);
#ifdef STRUCT_ENV
	  DerefStrEnv(old);
	  StructEnvRef(new)->env = NewEnv(StructEnvRef(old)->env);
#endif
	  Link(old,new);

	  old_strp = StrArgRef(old, 0);
	  new_strp = StrArgRef(new, 0);

#ifdef GC_DEBUG
	  FW = ForeWord(old_strp);
	  if (IsForwMarked(FW)) {
	    WARNING("GC (scavenger): Forward marked word found in struct!");
	    *new_strp = ScavMark(TagRef(old_strp));
	  } else
#endif
	  if (IsUnBoundRef(old_strp)) {
	    DerefVarEnv(old_strp);
	    CopyUnbound(gcst,*old_strp,varterm);
	    *new_strp = ScavMark(varterm);
	    Link(old_strp,new_strp);
	  } else {
	    *new_strp = ScavMark(*old_strp);
	  }
	  old_strp++;
	  new_strp++;

	  for (i = 1; i < arity; i += 1) {
#ifdef GC_DEBUG
	    FW = ForeWord(old_strp);
	    if (IsForwMarked(FW)) {
	      WARNING("GC (scavenger): Forward marked word found in struct!");
	      *new_strp = TagRef(old_strp);
	    } else
#endif
	    if (IsUnBoundRef(old_strp)) {
	      DerefVarEnv(old_strp);
	      CopyUnbound(gcst,*old_strp,varterm);
	      *new_strp = varterm;
	      Link(old_strp,new_strp);
	    } else {
	      *new_strp = *old_strp;
	    }
	    old_strp++;
	    new_strp++;
	  }
	  ScavPush();
	  locptr = &StrArgument(new, arity-1);
	  t = REVERSED_STR;
	  continue;
	}
 genlbl:
	{
	  Generic old = Gen(old_t), new;
	  FW = ForeWord(old);
	  if (IsForwMarked(FW)) {
	    result = TagGen(ForwOf(FW));
	    goto endswitch;
	  }
	  SaveHeapRegisters();
	  new = old->method->new(old);
	  if (new == NULL) {
	    result = old_t;
	  } else {
	    new->method = old->method;
	    Link(old,new);		  /* [BD] Here we smash method slot */
	    new->method->gc(old,new,gcst);/* [BD] so we must use new here! */
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
	  FatalError("gc_location pointer flip detects unsupported tag");
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

void gc_tuple(old_strp,new_strp,arity,gcst)
     Term *old_strp;
     Term *new_strp;
     register long arity;
     gcstatep gcst;
{
#ifdef GC_DEBUG
  register tagged_address FW;
#endif
  register long i;
  Term varterm;

  for (i = 0; i < arity; i += 1) {
#ifdef GC_DEBUG
    FW = ForeWord(old_strp);
    if (IsForwMarked(FW)) {
      WARNING("GC (scavenger): Forward marked word found in tuple!");
      *new_strp = TagRef(old_strp);
    } else
#endif
    if (IsUnBoundRef(old_strp)) {
      DerefVarEnv(old_strp);
      CopyUnbound(gcst,*old_strp,varterm);
      *new_strp = varterm;
      Link(old_strp,new_strp);
    } else {
      *new_strp = *old_strp;
    }
    old_strp++;
    new_strp++;
  }
  gc_locations(new_strp-arity,arity,gcst);
}

void gc_todo(x,gcst)
     Generic x;
     gcstatep gcst;
{
  SaveGN(gcst,x);
  return;
}

void gc_forward(old,new)
     void *old;
     void *new;
{
  Link(old,new);
  return;
}

void gc_save_susp(gcst,s,sp)
     gcstatep gcst;
     suspension *s;
     suspension **sp;
{
  SaveSusp(gcst,s,sp);
}
