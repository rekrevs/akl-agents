#include "fd_akl.h"

#ifdef THREADED_CODE
address *fd_instr_label_table;
int fd_instr_label_table_length = 0;
enum_fd_instr fd_instr_to_enum(instr)
     fd_instr instr;
{
  int i = fd_instr_label_table_length;
  while (i--) {
    if (instr == fd_instr_label_table[i]) {
      return i;
    }
  }
  return 0; /* is never reached? */
}
#endif

gvamethod finitemethod =
{
  new_finite,
  unify_finite,
  print_finite,
  copy_finite,
  gc_finite,
  uncopied_finite,
  deallocate_finite,
  kill_finite,
  close_finite,
  send_finite,
  copy_external_finite,
  gc_external_finite,
  NULL,
  makewake_finite,
  makerecall_finite
};

constrtable fd_in_method = {
  install_in,
  reinstall_in,
  deinstall_in,
  promote_in,
  print_in,
  copy_in,
  gc_in
};

constrtable min_method = {
  install_min,
  reinstall_min,
  deinstall_min,
  promote_min,
  print_min,
  copy_min,
  gc_min
};

constrtable trail_method = {
  install_trail,
  reinstall_trail,
  deinstall_trail,
  promote_trail,
  print_trail,
  copy_trail,
  gc_trail
};

void makewake_finite(x,andb)
     findom *x;
     andbox *andb;
{
  suspension *susp;
  DerefEnv(&(x->env));
  NewSuspension(susp);
  susp->type = ANDB;
  susp->suspended.andb = andb;
  susp->next = x->asusp;
  x->asusp = susp;
  Mark(andb,Home(x->env));
  return;
}

void makerecall_finite(x,chb)
     findom *x;
     choicebox *chb;
{
  suspension *susp;
  DerefEnv(&(x->env));
  NewSuspension(susp);
  susp->type = CHB;
  susp->suspended.chb = chb;
  susp->next = x->asusp;
  x->asusp = susp;
  Mark((chb)->father,Home(x->env));
  return;
}

bool unify_finite(xa,ya,andb,exs)
     Term xa;
     Term ya;
     andbox *andb;
     exstate *exs;
{
  argvec args;
  Structure Args, Prop, DomV;
  monstruct *ms;
  Term V, P, dv, propv;

  if (Eq(xa,ya)) return TRUE;

  MakeVariableTerm(V,andb);
  
  MakeStruct(Args,fd_unif_arg_fun,andb);
  args = TagStr(Args);
  EnvArg(0,args) = ya;
  EnvArg(1,args) = xa;

  MakeStruct(DomV,fd_domv_fun,andb);
  dv = TagStr(DomV);
  StrArgument(DomV,0) = ya;

  MakeStruct(Prop,fd_prop_fun,andb);
  propv = TagStr(Prop);
  MakeIntegerTerm(P,P_DOM);
  StrArgument(Prop,0) = P;

  NEW(ms,monstruct);
  ms->mon = NULL;
  if (IsDetermined(ya)) {
    ms->amon = NULL;
  }
  else {
    NEW(ms->amon,moncons);
    ms->amon->d = ya;
    ms->amon->next = NULL;
  }

  if (fd_in(xa,dom_code_index_0,V,args,propv,ms,1,dv,andb,exs)) {
    NEW(ms,monstruct);
    ms->mon = NULL;
    if (IsDetermined(xa)) {
      ms->amon = NULL;
    }
    else {
      NEW(ms->amon,moncons);
      ms->amon->d = xa;
      ms->amon->next = NULL;
    }
    StrArgument(DomV,0) = xa;
    return fd_in(ya,dom_code_index_1,V,args,propv,ms,1,dv,andb,exs);
  }
  else 
    return FALSE;
}

int uncopied_finite() { return 0; }

envid *deallocate_finite() { return NULL; }

int kill_finite() { return 0; }

bool close_finite() { return FALSE; }

bool send_finite() { return FALSE; }

int print_finite(file,x)
     FILE *file;
     findom *x;
{
  fprintf(file,":0x%x{ ",x);
  print_dom(file,x);
  fprintf(file,"}");
  return 1;  
}

Gvainfo new_finite(old)
     findom *old;
{
  findom *new;

  NEW(new,findom);
  if (!IsInterval(old)) {
    NEWX(Domain(new),bitmask,(LastMask(old)+1));
  }
  else {
    Domain(new) = NULL;
  }
  return (Gvainfo)new;
}

Gvainfo gc_finite(old,new,gcst)
     findom *old;
     findom *new;
     gcstatep gcst;
{
  int i, lm;

  new->next = NULL;
  new->min = old->min;
  new->max = old->max;
  new->info = old->info;
  new->trailed = NULL;
  if (!IsInterval(old)) {
    lm = LastMask(old);
    for (i=0; i<=lm; i++)
      DomainWord(new,i) = DomainWord(old,i);
  }
  gc_save_susp(gcst,old->asusp,&new->asusp);
  gc_save_susp(gcst,old->tsusp,&new->tsusp);
  gc_todo(old,gcst);
  return (Gvainfo)new;
}

Gvainfo copy_finite(old,new)
     findom *old;
     findom *new;
{
  int i, lm;

  new->next = NULL;
  new->min = old->min;
  new->max = old->max;
  new->info = old->info;
  new->trailed = NULL;
  if (!IsInterval(old)) {
    lm = LastMask(old);
    for (i=0; i<=lm; i++)
      DomainWord(new,i) = DomainWord(old,i);
  }
  copy_todo(old);
  return (Gvainfo)new;
}

void copy_external_finite(old)
     findom *old;
{
  findom *new;
  if (Forwarded(old)) {
    new = DerefForward(old);
    local_suspensions(&(new->asusp),&(old->asusp));
    local_suspensions(&(new->tsusp),&(old->tsusp));
    copy_fd_susp(&(old->isusp),&(new->isusp));
  }
  else {
    external_suspensions(&(old->asusp));
    external_suspensions(&(old->tsusp));
    split_fd_susp(old->isusp);
  }
  return;
}

void gc_external_finite(old,gcst)
     findom *old;
     gcstatep gcst;
{
  findom *new;

  new = DerefForward(old);
  gc_fd_susp(&(old->isusp),&(new->isusp),gcst);
  return;
}

bool akl_in(Arg)
     Argdecl;
{
  Term X0, X1, X2, X3, X4, X5, X6, X7, T, L;
  int i;
  monstruct *ms;
  moncons **lp;
  Structure SA;

  Deref(X0,A(0));		/* finite domain variable */
  Deref(X1,A(1));		/* label of range code */
  Deref(X2,A(2));		/* entailment variable */
  Deref(X3,A(3));		/* environment of range */
  Deref(X4,A(4));		/* dependencies between environment and X0 */
  Deref(X5,A(5));		/* monotonicity information */
  Deref(X6,A(6));		/* execute for ask&tell or not */
  Deref(X7,A(7));		/* domain variables in indexical */

  if (Eq(X2,ZERO))		/* indexical already entailed */
    return TRUE;
  if (!IsATM(X3)) {
    SA = Str(X3);
    for (i=0; i<StrArity(SA); i++) {
      GetStrArg(T,SA,i);
      Deref(T,T);
      if (IsFinite(T)) continue;
      IfVarSuspend(T);
    }
  }
  NEW(ms,monstruct);
  lp = &(ms->mon);
  Deref(L,StrArgument(Str(X5),0));
  while (!Eq(L,NIL)) {
    Deref(T,LstCar(Lst(L)));
    if (IsFinite(T)) {
      if (!IsDetermined(T)) {
	NEW(*lp,moncons);
	(*lp)->d = T;
	lp = &((*lp)->next);
      }
    }
    Deref(L,LstCdr(Lst(L)));
  }
  *lp = NULL;
  lp = &(ms->amon);
  Deref(L,StrArgument(Str(X5),1));
  while (!Eq(L,NIL)) {
    Deref(T,LstCar(Lst(L)));
    if (IsFinite(T)) {
      if (!IsDetermined(T)) {
	NEW(*lp,moncons);
	(*lp)->d = T;
	lp = &((*lp)->next);
      }
    }
    Deref(L,LstCdr(Lst(L)));
  }
  *lp = NULL;
  return fd_in(X0,code_table[GetInteger(X1)],X2,X3,X4,ms,
	       GetInteger(X6),X7,exs->andb,exs);
}

void deinstall_in(c)
     fd_constraint *c;
{
  return;
}

bool install_in(cns,andb,exs)
     fd_constraint *cns;
     andbox *andb;
     exstate *exs;
{
  Term X;
  long c;
  argvec args;
  monstruct *ms;
  domstruct t;
  dep_class prop;
  dom_info di;
  findom *x;

  if (Entailed(cns->f)) 
    return TRUE;
  Deref(X,cns->f->d);
  c = cns->f->c;
  args = cns->f->args;
  ms = cns->f->moninfo;
  di = monotonicity(ms);

  if (Bottom(di)) return SUSPEND;
  evaluate_range(c,args,&t);
  switch (intersect_fd(X,&t)) {
  case EMPTY:
    if (Shrinking(di) || Constant(di)) {
      return FALSE;
      }
    return SUSPEND;
  case SUBSET:
    if (Shrinking(di))
      return SUSPEND;
    SetEntailed(cns->f->ent,andb,exs);
    return TRUE;
  case NONEMPTY:
    if Growing(di) {
      return SUSPEND;
    }
    x = Finite(X);
    if (MustTrail(x,andb))
      trail(X,andb);
    prop = propagate_info(X,&t,andb,exs);
    if (!IsDetermined(X)) 
      move_dom(x,&t);
    else
      wake_agent_suspensions(x,andb,exs);
    wake_trail_suspensions(x,andb,exs);
    if (fd_propagate(x,prop,andb,exs)) { /* may wake underlying andboxes */
      if (Constant(di)) {
	SetEntailed(cns->f->ent,andb,exs);
	return TRUE;
      }
      else
	return SUSPEND;
    }
    else
      return FALSE;
  }
}

void reinstall_in(cns,andb)
     fd_constraint *cns;
     andbox *andb;
{
  return;
}

void promote_in(cns,prom,andb,exs)
     fd_constraint *cns;
     andbox *prom, *andb;
     exstate *exs;
{
  LinkConstr(andb,(constraint *)cns);
  return;			
}

int print_in(cns)
     fd_constraint *cns;
{
  long d = cns->f->c;
  long b_code[200];
  long *cp;
  enum_fd_instr instr;

  cp = b_code+200;
  *(--cp) = FD_HALT;
  while (cp-b_code > 0) {
    instr = FdInstrToEnum(byte_code[d]);
    switch(instr) {
    case FD_HALT:
      goto rest;
    case FD_CONST:
    case FD_VAL:
    case FD_DOMMIN:
    case FD_DOMMAX:
    case FD_DOM:
    case FD_CHECK:
      *(--cp) = byte_code[d+1];
      d++;
    default:
      *(--cp) = instr;
      d++;
    }
  }
  FatalError("Overflow when printing in-constraint!");
 rest:
  display_term(cns->f->d);
  printf(" in ");
  fd_das(cp,cns->f->args);
  return 1;
}

constraint *copy_in(c)
     fd_constraint *c;
{
  fd_constraint *newc;
  fd_function *nfdc, *fdc;

  NEW(newc,fd_constraint);
  fdc = c->f;
  if (Forwarded(fdc)) {
    nfdc = (fd_function *)DerefForward(fdc);
  }
  else {
    NEW(nfdc,fd_function);	
    nfdc->info = fdc->info;
    DerefEnv(&(fdc->env));
    nfdc->env = NewEnv(fdc->env);
    nfdc->d = fdc->d;
    copy_location(&(nfdc->d));
    copy_forward(fdc,nfdc);	/* destroys fdc->d */
    nfdc->c = fdc->c;
    nfdc->ent = fdc->ent;
    copy_location(&(nfdc->ent));
    copy_args(fdc->args,&(nfdc->args));
    copy_mon(fdc->moninfo,&(nfdc->moninfo));
    nfdc->constr = (constraint *)newc;
  }
  newc->next = NULL;
  newc->method = c->method;
  newc->f = nfdc;
  return (constraint *)newc;
}

constraint *gc_in(c,gcst)
     fd_constraint *c;
     gcstatep gcst;
{
  fd_constraint *newc;
  fd_function *nfdc, *fdc;

  NEW(newc,fd_constraint);
  fdc = c->f;
  if (Forwarded(fdc)) {
    nfdc = (fd_function *)DerefForward(fdc);
  }
  else {
    NEW(nfdc,fd_function);	
    nfdc->info = fdc->info;
    DerefEnv(&(fdc->env));
    nfdc->env = NewEnv(fdc->env);
    nfdc->d = fdc->d;
    gc_location(&(nfdc->d),gcst);
    gc_forward(fdc,nfdc);
    nfdc->c = fdc->c;
    nfdc->ent = fdc->ent;
    gc_location(&(nfdc->ent),gcst);
    gc_args(fdc->args,&(nfdc->args),gcst);
    gc_mon(fdc->moninfo,&(nfdc->moninfo),gcst);
    nfdc->constr = (constraint *)newc;
  }
  newc->next = NULL;
  newc->method = c->method;
  newc->f = nfdc;
  return (constraint *)newc;
}

void deinstall_trail(c)
     trail_constraint *c;
{
  unsigned long info, minmax;
  bitmask *bv;
  envid *trailed;
  findom *x;

  if (IsDeadTrail(c))
    return;
  x = c->dom;
  /* restore old x and save new x in c */
  x->next = NULL;
  info = x->info; x->info = c->info; c->info = info; 
  minmax = x->min; x->min = c->min; c->min = minmax;
  minmax = x->max; x->max = c->max; c->max = minmax;
  trailed = x->trailed; x->trailed = c->trailed; c->trailed = trailed;
  bv = Domain(x); Domain(x) = c->vec; c->vec = bv;
  return;
}

bool install_trail(c,andb,exs) 
     trail_constraint *c;
     andbox *andb;
     exstate *exs;
{
  Term X;

  Deref(X,c->d);
  switch(intersect_trail(X,c)) {
  case EMPTY:
    SetDeadTrail(c);
    return FALSE;
  case SUBSET:
    return TRUE;
  case NONEMPTY:
    c->dom = Finite(X);
    retrail(c,andb,exs);
    if (ExternalObj(c->dom,andb)) {
      SuspendAndb(c->dom,andb);
      return SUSPEND;
    }
    else
      return TRUE;
  }
}

void reinstall_trail(c)
     trail_constraint *c;
{
  Term X;

  Deref(X,c->d);
  assert(!IsDetermined(X));
  c->dom = Finite(X);
  deinstall_trail(c);
  return;
}

void promote_trail(c,prom,andb,exs)
     trail_constraint *c;
     andbox *prom, *andb;
     exstate *exs;
{
  findom *x;

  x = c->dom;
  x->trailed = c->trailed;
  if (Min(x) == Max(x))
    Determine(c->d,Min(x),andb,exs);
  if (IsDetermined(c->d))
    wake_agent_suspensions(x,andb,exs);
  wake_trail_suspensions(x,andb,exs);
  if (LocalObj(x,andb)) {
    return;
  }
  if (!Trailed(x,andb)) {
    LinkConstr(andb,(constraint *)c);
  }
  return;
}

int assert_trail_c(c)
     constraint *c;
{
  Term V;
  for (; c; c=c->next) {
    if (c->method != &trail_method) 
      continue;
    Deref(V,((trail_constraint *)c)->d);
    if (IsDetermined(V))
      return 0;
  }
  return 1;
}

int print_trail(c)
     trail_constraint *c;
{
  print_finite(stdout,c->dom);
  printf(" is trailed");
  return 1;
}

int p_tr(c)
     trail_constraint *c;
{
  print_trail(c);
  printf("\n");
}

constraint *copy_trail(c)
     trail_constraint *c;
{
  int i;
  trail_constraint *newc;

  NEW(newc,trail_constraint);
  newc->next = NULL;
  newc->method = c->method;
  newc->d = c->d;
  copy_location(&(newc->d));
  newc->info = c->info;
  newc->min = c->min;
  newc->max = c->max;
  DerefTrail(c);
  newc->trailed = NewTrail(c);
  if (!IsInterval(c)) {
    NEWX(newc->vec,bitmask,(LastMask(c)+1));
    for (i=0; i<=LastMask(c); i++) 
      newc->vec[i] = c->vec[i];
  }
  else
    newc->vec = NULL;
  return (constraint *)newc;
}

constraint *gc_trail(c,gcst)
     trail_constraint *c;  
     gcstatep gcst;
{
  int i;
  trail_constraint *newc;

  NEW(newc,trail_constraint);
  newc->next = NULL;
  newc->method = c->method;
  newc->d = c->d;
  gc_location(&(newc->d),gcst);
  newc->info = c->info;
  newc->min = c->min;
  newc->max = c->max;
  DerefTrail(c);
  newc->trailed = NewTrail(c);
  if (!IsInterval(c)) {
    NEWX(newc->vec,bitmask,(LastMask(c)+1));
    for (i=0; i<=LastMask(c); i++) 
      newc->vec[i] = c->vec[i];
  }
  else
    newc->vec = NULL;
  return (constraint *)newc;
}

/* Suspends until either the argument is determined, thus failing, 
   or until promoted. When promoted the second argument is instantiated to the
   minimum element of the first argument.
*/
bool fd_min(Arg)
     Argdecl;
{
  Term X0, X1;

  Deref(X0,A(0));
  Deref(X1,A(1));

  if (IsDetermined(X0))
    return FALSE;
  if (IsFinite(X0)) {
    SuspendMinC(X0,X1,exs->andb);
    return TRUE;
  }
  IfVarSuspend(X0);
  return FALSE;
}

bool install_min(c,andb,exs)
     min_constraint *c;
     andbox *andb;
     exstate *exs;
{
  return SUSPEND;
}

void reinstall_min(c)
     min_constraint *c;
{
  return;
}

void deinstall_min(c)
     min_constraint *c;
{
  return;
}

void promote_min(c,prom,andb)
     min_constraint *c;
     andbox *prom, *andb;
{
  int min;
  findom *x;			/* non empty */
  Term n, xa, X;

  Deref(X,c->d);
  if (IsDetermined(X)) {
    min = FdVal(X);
  }
  else {
    x = Finite(X);
    min = Min(x);
  }
  Deref(xa,c->n);
  MakeIntegerTerm(n,min);
  BindVar(Ref(xa),n);
  return;
}

int print_min(c)
     min_constraint *c;
{
  printf("fd_min(");
  display_term(c->d);
  printf(")\n");
  return 1;
}

constraint *copy_min(c)
     min_constraint *c;
{
  min_constraint *newc;

  NEW(newc,min_constraint);
  newc->next = NULL;
  newc->method = c->method;
  newc->d = c->d;
  copy_location(&(newc->d));
  newc->n = c->n;
  copy_location(&(newc->n));
  return (constraint *)newc;
}

constraint *gc_min(c,gcst)
     min_constraint *c;
     gcstatep gcst;
{
  min_constraint *newc;

  NEW(newc,min_constraint);
  newc->next = NULL;
  newc->method = c->method;
  newc->d = c->d;
  gc_location(&(newc->d),gcst);
  newc->n = c->n;
  gc_location(&(newc->n),gcst);
  return (constraint *)newc;
}

int fd_new_label()
{
  if (label_indx >= CodeTable)
    FatalError("FD code table full!");
  code_table[label_indx] = code_top;
  return label_indx++;
}

void fd_push_check_label()
{
  check_label[check_top++] = code_top++;
  return;
}

void fd_pop_check_label()
{
  byte_code[check_label[--check_top]] = code_top;
  return;
}

void fd_add_instr(i)
     int i;
{
  if (code_top>=CodeArea)
    FatalError("FD byte code area filled up!");
  NextInstr(code_top) = EnumToFdInstr(i);
  return;
}

void fd_add_const(i)
     int i;
{
  if (code_top>=CodeArea)
    FatalError("FD byte code area filled up!");
  NextInstr(code_top) = i;
  return;
}

bool fd_size(Arg)
     Argdecl;
{
  Term X0, X1;
  Term n;
  int i;
  findom *x;

  Deref(X0, A(0));
  Deref(X1, A(1));

  if (IsINT(X0)) {
    MakeIntegerTerm(n,1);
    return unify(X1,n,exs->andb,exs);
  }

  if (IsFinite(X0)) {
    x = Finite(X0);
    if (IsInterval(x)) {
      i = (Max(x) - Min(x)) + 1;
    }
    else {
      int k, lm=LastMask(x);
      for (i=0, k=0; k<=lm; k++)
	i += number_of_bits(DomainWord(x,k));
    }
    MakeIntegerTerm(n,i);
    return unify(X1,n,exs->andb,exs);
  }

  IfVarSuspend(X0);
  
  return FALSE;
}

bool fd_one(Arg)
     Argdecl;
{
  Term X0, Car, Cdr;
  void_list *fl, *ml, **mlp;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  if (!IsLST(X0))
    return FALSE;
  Cdr = X0;

  while (!Eq(Cdr,NIL)) {
    Deref(Car,LstCar(Lst(Cdr)));
    if (!IsDetermined(Car)) {
      NEW(fl,void_list);
      fl->car = (void *)Finite(Car);
      mlp = &fl->cdr;
      Deref(Cdr,LstCdr(Lst(Cdr)));
      break;
    }
    Deref(Cdr,LstCdr(Lst(Cdr)));
  }
  while (!Eq(Cdr,NIL)) {
    Deref(Car,LstCar(Lst(Cdr)));
    if (!IsDetermined(Car)) {
      NEW(ml,void_list);
      *mlp = ml;
      ml->car = (void *)Finite(Car);
      mlp = &ml->cdr;
    }
    Deref(Cdr,LstCdr(Lst(Cdr)));
  }
  *mlp = NULL;

  fd_cp=1;
  if (!fd_one_solution(X0))
    return FALSE;

  for (; fl; fl=fl->cdr)
    wake_agent_suspensions((findom *)fl->car,exs->andb,exs);
  return TRUE;
}

bool fd_one_ff(Arg)
     Argdecl;
{
  Term X0, Car, Cdr;
  void_list *fl, *ml, **mlp;

  Deref(X0, A(0));
  IfVarSuspend(X0);
  if (!IsLST(X0))
    return FALSE;
  Cdr = X0;

  while (!Eq(Cdr,NIL)) {
    Deref(Car,LstCar(Lst(Cdr)));
    if (!IsDetermined(Car)) {
      NEW(fl,void_list);
      fl->car = (void *)Finite(Car);
      mlp = &fl->cdr;
      Deref(Cdr,LstCdr(Lst(Cdr)));
      break;
    }
    Deref(Cdr,LstCdr(Lst(Cdr)));
  }
  while (!Eq(Cdr,NIL)) {
    Deref(Car,LstCar(Lst(Cdr)));
    if (!IsDetermined(Car)) {
      NEW(ml,void_list);
      *mlp = ml;
      ml->car = (void *)Finite(Car);
      mlp = &ml->cdr;
    }
    Deref(Cdr,LstCdr(Lst(Cdr)));
  }
  *mlp = NULL;

  fd_cp=1;
  if (!fd_one_solution_ff(X0))
    return FALSE;

  for (; fl; fl=fl->cdr)
    wake_agent_suspensions((findom *)fl->car,exs->andb,exs);
  return TRUE;
}

bool fd_numberof(Arg)
     Argdecl;
{
  Term X0, X1, n;

  Deref(X0, A(0));
  Deref(X1, A(1));
  IfVarSuspend(X0);
  fd_cp=1;
  fd_number_of_solutions=0;
  fd_numb_of(X0);
  MakeIntegerTerm(n,fd_number_of_solutions);
  return unify(X1,n,exs->andb,exs);
}

bool fd_numberof_ff(Arg)
     Argdecl;
{
  Term X0, X1, n;

  Deref(X0, A(0));
  Deref(X1, A(1));
  IfVarSuspend(X0);
  fd_cp=1;
  fd_number_of_solutions=0;
  fd_numb_of_ff(X0);
  MakeIntegerTerm(n,fd_number_of_solutions);
  return unify(X1,n,exs->andb,exs);
}

void initialize_finite()
{
  label_indx = 0;
  code_top = 0;
  check_top = 0;
  fd_time = 0;
  fd_unif_arg_fun = store_functor(store_atom("args"),2);
  fd_domv_fun = store_functor(store_atom("domains"),1);
  fd_prop_fun = store_functor(store_atom("prop"),1);

#ifdef THREADED_CODE
  evaluate_range(NULL,NULL,NULL); /* initialize label table */
#endif
  dom_code_index_0 = code_top;
  NextInstr(code_top) = EnumToFdInstr(FD_DOM_0);
  NextInstr(code_top) = EnumToFdInstr(FD_HALT);
  dom_code_index_1 = code_top;
  NextInstr(code_top) = EnumToFdInstr(FD_DOM_1);
  NextInstr(code_top) = EnumToFdInstr(FD_HALT);
  MakeIntegerTerm(ZERO,0);

  define("akl_in",akl_in,8);
  define("fd_size",fd_size,2);
  define("fd_min",fd_min,2);
  define("fd_one",fd_one,1);
  define("fd_one_ff",fd_one_ff,1);
  define("fd_numberof",fd_numberof,2);
  define("fd_numberof_ff",fd_numberof_ff,2);
}
