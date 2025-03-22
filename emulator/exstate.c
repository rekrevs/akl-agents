/* $Id: exstate.c,v 1.23 1994/05/10 13:47:00 jm Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "storage.h"
#include "config.h"
#include "examine.h"
#include "error.h"

#if !defined(APOLLO) && !defined(macintosh)
#include <malloc.h>
#endif

void		update_context_trail();
void		update_context_task();
void		update_context_recall();
void		update_context_wake();


exstate *freelist = NULL;

void init_trail(exs,size)
     exstate *exs;
     int size;
{
  trailentry *seg;

  seg = (trailentry*)malloc(sizeof(trailentry)*size);
  if(seg == NULL) {
    FatalError("could not allocate trail stack");
  }
  exs->trail.size = size;
  exs->trail.current = seg;
  exs->trail.start = seg;
  exs->trail.end = seg + size;
}

void reinit_trail(exs)
     exstate *exs;
{
  trailentry *new;
  int  offset;

  exs->trail.size += exs->trail.size;
  new = (trailentry*)realloc(exs->trail.start, sizeof(trailentry)*exs->trail.size);
  if(new == NULL)
    FatalError("could not reallocate trail stack");
  if(new != exs->trail.start) {
    
    update_context_trail(exs, exs->trail.start, new);
    offset = exs->trail.current - exs->trail.start;
    exs->trail.start = new;
    exs->trail.current = new + offset;
  }
  exs->trail.end = exs->trail.start + exs->trail.size;
}    


void init_suspend(exs,size)
     exstate *exs;
     int size;
{
  Reference *seg;
  seg = (Reference*)malloc(sizeof(Reference)*size);
  if(seg == NULL) {
    FatalError("could not allocate suspend stack");
  }
  exs->suspend.size = size;
  exs->suspend.current = seg;
  exs->suspend.start = seg;
  exs->suspend.end = seg + size;
}

void reinit_suspend(exs)
     exstate *exs;
{
  Reference *new;
  int  offset;

  exs->suspend.size += exs->suspend.size;
  new = (Reference*)realloc(exs->suspend.start, sizeof(Reference)*exs->suspend.size);
  if(new == NULL)
    FatalError("could not reallocate suspend stack");
  if(new != exs->suspend.start) {
    offset = exs->suspend.current - exs->suspend.start;
    exs->suspend.start = new;
    exs->suspend.current = new + offset;
  }
  exs->suspend.end = exs->suspend.start + exs->suspend.size;
}


void init_task(exs,size)
     exstate *exs;
     int size;
{
  task *seg;
  seg = (task*)malloc(sizeof(task)*size);
  if(seg == NULL) {
    FatalError("could not allocate task stack");
  }
  exs->task.size = size;
  exs->task.current = seg;
  exs->task.start = seg;
  exs->task.end = seg + size;
}

void reinit_task(exs)
     exstate *exs;
{
  task *new;
  int  offset;

  exs->task.size += exs->task.size;
  new = (task *)realloc(exs->task.start, sizeof(task)*exs->task.size);
  if(new == NULL)
    FatalError("could not reallocate task stack");
  if(new != exs->task.start) {
    update_context_task(exs,exs->task.start,new);
    offset = exs->task.current - exs->task.start;
    exs->task.start = new;
    exs->task.current = new + offset;
  }
  exs->task.end = exs->task.start + exs->task.size;
}    


void init_recall(exs,size)
     exstate *exs;
     int size;
{
  recall *seg;
  seg = (recall*)malloc(sizeof(recall)*size);
  if(seg == NULL) {
    FatalError("could not allocate recall stack");
  }
  exs->recall.size = size;
  exs->recall.current = seg;
  exs->recall.start = seg;
  exs->recall.end = seg + size;
}

void reinit_recall(exs)
     exstate *exs;
{
  recall *new;
  int  offset;

  exs->recall.size += exs->recall.size;
  new = (recall*)realloc(exs->recall.start, sizeof(recall)*exs->recall.size);
  if(new == NULL)
    FatalError("could not reallocate recall stack");
  if(new != exs->recall.start) {
    update_context_recall(exs,exs->recall.start,new);
    offset = exs->recall.current - exs->recall.start;
    exs->recall.start = new;
    exs->recall.current = new + offset;
  }
  exs->recall.end = exs->recall.start + exs->recall.size;
}


void init_wake(exs,size)
     exstate *exs;
     int size;
{
  wake *seg;
  seg = (wake*)malloc(sizeof(wake)*size);
  if(seg == NULL) {
    FatalError("could not allocate wake stack");
  }
  exs->wake.size = size;
  exs->wake.current = seg;
  exs->wake.start = seg;
  exs->wake.end = seg + size;
}

void reinit_wake(exs)
     exstate *exs;
{
  wake *new;
  int  offset;

  exs->wake.size += exs->wake.size;
  new = (wake *)realloc(exs->wake.start, sizeof(wake)*exs->wake.size);
  if(new == NULL)
    FatalError("could not reallocate wake stack");
  if(new != exs->wake.start) {
    update_context_wake(exs,exs->wake.start,new);
    offset = exs->wake.current - exs->wake.start;
    exs->wake.start = new;
    exs->wake.current = new + offset;
  }
  exs->wake.end = exs->wake.start + exs->wake.size;
}    



recall *shift_recall(exs,i,n)
     exstate *exs;
     int i, n;
{
  recall *current, *end, *top, *insert;
  context *from, *cntxt;

  current = exs->recall.current;
  end = exs->recall.end;
  
  top = current + i;
  while(top > end) {
    reinit_recall(exs);
    current = exs->recall.current;
    end = exs->recall.end;
    top = current + i;
  }

  /* the uppermost contexts should not change */
  from = exs->context.start + n;

  if(current != from->recall) {
    recall *tsk, *newt;
    tsk = current;
    newt = tsk + i;
    for(;tsk != from->recall; ){
      tsk--;
      newt--;
      *newt = *tsk;
    }
  }

  insert = from->recall;

  exs->recall.current = current + i;

  /* update the context recall field */
  
  for(cntxt = exs->context.current; cntxt != from; cntxt--) {
    cntxt->recall = cntxt->recall + i;      
  }
  return insert;
}


void init_context(exs,size)
     exstate *exs;
     int size;
{
  context *seg;
  seg = (context *)malloc(sizeof(context)*size);
  if(seg == NULL) {
    FatalError("could not allocate context stack");
  }
  exs->context.size = size;
  exs->context.current = seg;
  exs->context.start = seg;
  exs->context.end = seg + size;
}

void reinit_context(exs)
     exstate *exs;
{
  context *new;
  int  offset;

  exs->context.size += exs->context.size;
  new = (context *)realloc(exs->context.start, sizeof(context)*exs->context.size);
  if(new == NULL)
    FatalError("could not reallocate context stack");
  if(new != exs->context.start) {
    offset = exs->context.current - exs->context.start;
    exs->context.start = new;
    exs->context.current = new + offset;
  }
  exs->context.end = exs->context.start  + exs->context.size;
}    


void newcontext(exs)
     exstate *exs;
{
  exs->context.current++;
  if(exs->context.current == exs->context.end){
    reinit_context(exs);
  }
  exs->context.current->recall = exs->recall.current;
  exs->context.current->wake = exs->wake.current;  
  exs->context.current->task = exs->task.current;
  exs->context.current->trail = exs->trail.current;
}

void resetcontext(exs)
     exstate *exs;
{
  exs->context.current--;
}

void update_context_task(exs,old,new)
     exstate *exs;
     task *old, *new;
{
  context *cntx;
  int off;
  
  for(cntx = exs->context.current; cntx != exs->context.start; cntx--){
    off = cntx->task - old;
    cntx->task = new + off;
  }
}


void update_context_recall(exs,old,new)
     exstate *exs;
     recall *old, *new;
{
  context *cntx;
  int off;
  
  for(cntx = exs->context.current; cntx != exs->context.start; cntx--){
    off = cntx->recall - old;
    cntx->recall = new + off;
  }
}

void update_context_wake(exs,old,new)
     exstate *exs;
     wake *old, *new;
{
  context *cntx;
  int off;
  
  for(cntx = exs->context.current; cntx != exs->context.start; cntx--){
    off = cntx->wake - old;
    cntx->wake = new + off;
  }
}


void update_context_trail(exs, old,new)
     exstate *exs;
     trailentry *old, *new;
{
  context *cntx;
  int offset;

  for(cntx = exs->context.current; cntx != exs->context.start; cntx--){
    offset = cntx->trail - old;
    cntx->trail = new + offset;
  }
}

void add_generic_to_close(obj,exs)
     generic *obj;
     exstate *exs;     
{
  close_entry *newl;
  NEW(newl, close_entry);
  newl->type = GENERIC_CLOSE;
  newl->obj = obj;
  newl->next = exs->close;
  exs->close = newl;
}

void add_gvainfo_to_close(obj,exs)
     gvainfo *obj;
     exstate *exs;     
{
  close_entry *newl;
  NEW(newl, close_entry);
  newl->type = GVAINFO_CLOSE;
  newl->obj = obj;
  newl->next = exs->close;
  exs->close = newl;
}




exstate *new_exstate()
{
  exstate *exs;

  if(freelist != NULL) {
    exs = freelist;
    freelist = freelist->next;
    exs->trail.current = exs->trail.start;
    exs->task.current = exs->task.start;
    exs->context.current = exs->context.start;    
#ifdef BAM
    exs->cons_stack.current = exs->cons_stack.start;
    exs->try_stack.current = exs->try_stack.start;
#endif
  } else {
    exs = (exstate *)malloc(sizeof(exstate));
    if(exs == NULL)
      FatalError("could not allocate exstate");
    init_trail(exs,AKLTRAILSIZE);
    init_suspend(exs,AKLSUSPENDSIZE);
    init_task(exs,AKLTASKSIZE);
    init_recall(exs,AKLTASKSIZE);
    init_wake(exs,AKLTASKSIZE);    
    init_context(exs,AKLCONTEXTSIZE);
    init_unifystate(exs);
#ifdef BAM
    initialize_bam(exs);
#endif
  }
  return exs;
}


void return_exstate(exs)
     exstate* exs;
{
  exstate **zaploc;
  exstate *father = exs->father;

  /* If this is someones child, unlink it from the father */
  if (father != NULL) {
    zaploc = &father->child;
    while (*zaploc != exs) {
      zaploc = &((*zaploc)->next);
    }
    *zaploc = exs->next;
  }

  /* Put it in the freelist */
  exs->next = freelist;
  freelist = exs;
}

exstate *init_exstate()
{

  exstate *exs;
  int i;

  exs = new_exstate();
  exs->father = NULL;
  exs->next = NULL;
  exs->child = NULL;
  exs->close = NULL;
  exs->pc = NULL;
  exs->arity = 0;
  exs->def = NULL;
  exs->insert = NULL;
  exs->root = NULL;
  exs->andb  = NULL;

  for(i = 0; i < MAX_AREGS; i++) {
    exs->areg[i] = NullTerm;
  }
  return exs;
}



void suspend_trail(exs,andb)
     exstate *exs;
     andbox *andb;
{
    register trailentry *tr, *stop;
    stop = (exs)->context.current->trail; 
    tr = (exs)->trail.current;
    for(; tr != stop; tr--) {
        unifier *unif;
        Term v0,v1,nobind;
        v0 = tr->var;
	nobind = tr->val;
	Deref(v1,RefTerm(Ref(v0)));
	UnBindRef(Ref(v0),nobind);
	if (IsUVA(nobind)) {
	  MakeBigVar(Ref(v0));
	}
	if(IsREF(v1) && VarIsUVA(Ref(v1))) {
	  MakeBigVar(Ref(v1));
	}
        MakeUnifier(unif,v0,v1,andb);
	MakeWake(Ref(v0),andb);
        if(IsVar(v1)) {
	  MakeWake(Ref(v1),andb);
	}
    }
    (exs)->trail.current = stop;
}


void promote_constraints(exs, prom, andb)
     exstate *exs;
     andbox *prom, *andb;
{
  unifier *unif, *next;
  Reference var0;
  Term tmp;
      
  for(unif = prom->unify; unif != NULL; unif = next) {
    /* [BD] Uninstall it first */
    UnBindUnifier(unif);

    next = unif->next;
    var0 = Ref(unif->arg1);
    /* it is already installed */
    DerefGvaEnv(RefGva(var0));	/* This is always a big var */
    if(IsLocalGVA(var0,andb)) {
      WakeAll(exs,var0);
      /* [BD] Reinstall it */
      BindUnifier(unif);	/* Or just Bind(Ref(unif->arg1),unif->arg2)? */
    } else {
      Deref(tmp, unif->arg2);
      if (IsREF(tmp)) {
	register Reference tmpv = Ref(tmp);
	if (VarIsUVA(tmpv)) {
	  DerefUvaEnv(tmpv);
	  if(IsLocalUVA(tmpv,andb)) {
	    Bind(tmpv,unif->arg1);
	  } else {
	    Examine(exs,var0,andb);
	    MakeWake(var0,andb);
	    /* [BD] Reinstall it */
	    BindUnifier(unif);
	    LinkUnifier(andb,unif);
	  }
	} else {		/* GVA */
	  DerefGvaEnv(RefGva(tmpv));
	  if(IsLocalGVA(tmpv,andb)) {
	    WakeAll(exs,tmpv);
	    Bind(tmpv,unif->arg1);
	  } else {
	    Examine(exs,var0,andb);
	    MakeWake(var0,andb);
	    MakeWake(tmpv,andb);
	    /* [BD] Reinstall it */
	    BindUnifier(unif);
	    LinkUnifier(andb,unif);
	  }
	}
      } else {
	Examine(exs,var0,andb);
	MakeWake(var0,andb);
	/* [BD] Reinstall it */
	BindUnifier(unif);
	LinkUnifier(andb,unif);
      }
    }
  }
  /* The constraints are moved to the father and box. */
  {
    register constraint *constr, *next;

    for(constr = prom->constr; constr != NULL; constr = next) {
      constrtable *meth = constr->method;
      next = constr->next;
      if (meth != NULL)
	meth->promote(constr,prom,andb,exs);
    }
  }
}

void redo_builtin(exs, chb)
     exstate *exs;
     choicebox *chb;
{
  recall *rcp, *rls;

  Recall(exs, (choicebox*)1);
  rls = (exs)->context.current->recall;
  for(rcp = (exs)->recall.current; rcp != rls; rcp--){
    *rcp = *(rcp -1);
  }
  rcp->chb = (chb);
}
     

void promote_context(exs, andb)
     exstate *exs;
     andbox *andb;

{

  /* The trail is adjusted for the father andbox.
   * Variables that were local to the father andbox
   * should no longer remain on the trail, and are
   * therefore removed by compaction.
   * If the local variables are constrained, wake
   * entries are created.
   */

  trailentry	*last, *first;

  first = exs->context.current->trail;
  last = exs->trail.current;

  RestoreContext(exs);
    
  PopContext(exs);
    
  {
    Reference var0;
    Term tvar, nobind, tmp;
    trailentry *turtle = first;
    trailentry *hare = turtle;

    while(hare != last) {
      hare++;
      tvar = hare->var;
      nobind = hare->val;
      if (IsUVA(nobind)) {
	DerefUvaEnv(&hare->val); /* Dereference saved env */
	if (IsLocalEnv(Env(hare->val), andb))
	  continue;
	nobind = hare->val;
	var0 = NULL;		/* [BD] Use this as UVA/GVA flag */
      } else {			/* GVA, i.e. SVA */
	var0 = Ref(tvar);
	DerefGvaEnv(Gva(nobind));
	if(IsLocalEnv(GvaEnv(Gva(nobind)), andb)) {
	  WakeAll(exs,&nobind);	/* [BD] Is this a kludge? */
	  continue;
	}
      }
      Deref(tmp,tvar);
      if (IsREF(tmp)) {
	register Reference tmpv = Ref(tmp);
	if (VarIsUVA(tmpv)) {
	  DerefUvaEnv(tmpv);
	  if(IsLocalUVA(tmpv,andb)) {
	    Reference ref = Ref(tvar);
	    UnBindRef(ref,nobind);
	    Bind(tmpv,tvar);
	  }
	} else {		/* GVA */
	  DerefGvaEnv(RefGva(tmpv));
	  if(IsLocalGVA(tmpv,andb)) {
	    /* [BD] repeat of some code from above */
	    Reference ref = Ref(tvar);
	    UnBindRef(ref,nobind);
	    WakeAll(exs,tmpv);
	    Bind(tmpv,tvar);
	  }
	}
      } else {
	if (var0 != NULL) {
	  Examine(exs,&nobind,andb); /* [BD] Is this a kludge too? */
	}
	turtle++;
	if(hare != turtle) {
	  turtle->var = hare->var;
	  turtle->val = hare->val;
	}
      }
    }
    /* the trail of the and-box will include the remaining entries */
    exs->trail.current = turtle;
  }
}

bool entailed(constrp)
     constraint **constrp;
{
  constraint *constr = *constrp;
  while (constr != NULL) {
    constrtable *meth = constr->method;
    if (meth != NULL) {
      *constrp = constr;
      return FALSE;		/* Maybe call a method here? */
    }
    constr = constr->next;
  }
  *constrp = NULL;
  return TRUE;
}

bool bind_external_uva(exs, andb, var, val)
     exstate *exs;
     andbox *andb;
     Reference var;
     Term val;
{
  register envid *I = Env(RefTerm(var));

  if (I->env != NULL) {
    /* Handle promoted environments */
    do {
      I = I->env;
    } while (I->env != NULL);
    if (IsLocalEnv(I, andb)) {
      /* It dereferenced to the local environment */
      BindCheck(var, val, return FALSE);
      return TRUE;
    }
  }

  /* WakeLocalHVA(exs, var, andb); */
  TrailValue(exs, TagRef(var), TagUva(I));
  BindCheck(var, val, return FALSE);
  return TRUE;
}

bool bind_external_sva(exs, andb, var, val)
     exstate *exs;
     andbox *andb;
     Reference var;
     Term val;
{
  register envid *I = (envid*)(RefGva(var)->env);

  if (I->env != NULL) {
    /* Handle promoted environments */
    do {
      I = I->env;
    } while (I->env != NULL);

    RefGva(var)->env = I;

    if (IsLocalEnv(I, andb)) {
      /* It dereferenced to the local environment */
      WakeAll(exs, var);
      BindCheck(var, val, return FALSE);
      return TRUE;
    }
  }

  TrailRef(exs, var);
  /* WakeLocalSVA(exs, var, andb); */
  Examine(exs, var, andb);
  BindCheck(var, val, return FALSE);
  return TRUE;
}

