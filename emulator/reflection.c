/* $Id: reflection.c,v 1.41 1994/04/22 08:28:36 bd Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "copy.h"
#include "gc.h"
#include "initial.h"
#include "storage.h"
#include "config.h"
#include "trace.h"
#include "names.h"
#include "display.h"
#include "error.h"
#include "port.h"

Gvainfo	newrefl();
bool 	unifyrefl();
int 	printrefl();
Gvainfo	copyrefl();
Gvainfo	gcrefl();
int     uncopiedrefl();
envid	*deallocaterefl();
int 	killrefl();
bool 	closerefl();
bool 	sendrefl();


Term term_suspended;

Functor functor_solution_1; 	/* "solution/1" */


typedef struct reflection {
  gvamethod	*method;
  envid 	*env;
  exstate 	*exstate;
  Term		stream;
  constraint	*constr;
} reflection;


static gvamethod reflmethod =
{
  newrefl,
  unifyrefl,
  printrefl,
  copyrefl,
  gcrefl,
  uncopiedrefl,
  deallocaterefl,
  killrefl,
  closerefl,
  sendrefl,
  NULL,
  NULL
};


#define IsRefl(Prt) (IsCvaTerm(Prt) && \
		     RefCvaMethod(Ref(Prt)) == &reflmethod)

#define GvaRefl(g)  ((reflection*)(g))
#define Refl(x)     GvaRefl(RefGva(Ref(x)))



Gvainfo newrefl(old)
     reflection *old;
{
  register reflection *new;
  NEW(new,reflection);
  return (Gvainfo) new;
}

Gvainfo copyrefl(old)
     reflection *old;
{
  FatalError("Oh no! NYI");
  return NULL;
}

Gvainfo gcrefl(old,new,gcst)
     reflection *old, *new;
     gcstatep gcst;
{
  new->exstate = old->exstate;
  new->stream = old->stream;
  if(new->exstate != NULL)
     gc_exstate(new->exstate);
  gc_location(&new->stream,gcst);
  new->constr = gc_referenced_constraint(old->constr,gcst);
  return (Gvainfo) new;
}

envid *deallocaterefl(refl)
     Gvainfo refl;
{
  register reflection *r = (reflection *) refl;
  register close_entry *k;
  register generic *gen;

  /* kill all objects on the close list */
  for(k = r->exstate->close; k != NULL; k = k->next) {
    gen = (generic*)k->obj;
    gen->method->kill(gen);
  }
  
  /* return the execution state */
  return_exstate(r->exstate);
  r->exstate = NULL;
  
  close_port_stream(&r->stream);

  /* Always close, never kill, since a suspended guard may */
  /* be promoted even if both port and stream are garbage. */
  /* By closing, we make sure that the andbox will wake up */

  return r->env;	  /* Environment dereferencing is done by gc. */
}

int uncopiedrefl(refl)
     Gvainfo refl;
{
  register reflection *r = (reflection *) refl;
  register Term s;
  s = r->stream;
  while (IsREF(s)) {
    if (IsCopied(Ref(s))) {
      break;
    } else if (IsUnBoundRef(Ref(s))) {
      return 0;
    }
    s = RefTerm(Ref(s));
  }
  return 1;
}

bool unifyrefl(x,y,andb,exs)
     Term x;
     Term y;
     andbox *andb;
     exstate *exs;     
{
  if(Eq(x, y))
    return TRUE;
  return FALSE;
}


bool closerefl(Arg)
     Argdecl;
{
  Term X0;

  Deref(X0,A(0));
  if(!(IsRefl(X0))) {
    FatalError("Reflection entry on close list is fucked up!");
  } else {
    reflection *refl = Refl(X0);
    DerefGvaEnv(refl);
    if(IsLocalEnv(refl->env,exs->andb)) {
       Term point;
       refl->constr->method = NULL;	/* Kill the constraint */
       Deref(point, refl->stream);
       return unify(point, NIL, exs->andb,exs);
     }
  }
  return FALSE;
}


int killrefl(refl)
     reflection *refl;
{
  register close_entry *k;
  register generic *gen;
  constraint *constr = refl->constr;

  if (IsCopied(constr))
    constr = ((constraint*)Forw(constr));
  constr->method = NULL;

  if (refl->exstate != NULL) {
    /* kill all objects on the close list */
    for(k = refl->exstate->close; k != NULL; k = k->next) {
      gen = (generic*)k->obj;
      gen->method->kill(gen);
    }
  
    /* return the execution state */
    return_exstate(refl->exstate);
    refl->exstate = NULL;
  }

  return 1;
}


int printrefl(file,refl,tsiz)
     FILE *file;
     reflection *refl;
     int tsiz;
{
  fprintf(file,"{reflection: %#lx}", (unsigned long)refl->exstate);
  return 1;
}


bool sendrefl(message, self, exs)
     Term message, self;
     exstate *exs;
{
  return FALSE;
}


reflection *new_reflection(exs,andb,strm)
     exstate *exs;
     andbox *andb;
     Term strm;
{
  exstate *nexs;
  choicebox *root;
  andbox *mother;
  reflection *refl;
  
  nexs = init_exstate();

  PushContext(nexs);

  MakeRootChoicebox(root,NULL,NULL);

  MakeEmptyChoiceCont(root,nexs->areg,0);
  
  root->previous = NULL;
  root->next = NULL;

  Root(nexs);
  
  Start(nexs);

  MakeAndbox(mother,root);

  root->tried = mother;

  /* halt is pointing to a GUARD_HALT instruction */
  MakeAndCont(mother,halt,NULL,1);

  nexs->father = exs;
  nexs->next = exs->child;
  exs->child = nexs;

  nexs->child = NULL;
  nexs->pc = NULL;
  nexs->arity = 0,
  nexs->def = NULL;
  nexs->insert = NULL;
  nexs->andb = mother;
  nexs->root = root;

  /* the generic oject is created */
  NEW(refl, reflection);
  refl->method  = &(reflmethod);
  refl->env = (envid*)&(andb->env);
  refl->exstate = nexs;
  refl->stream = strm;
  refl->constr = make_port_constraint(exs);
      
  add_gvainfo_to_close((Gvainfo)refl,exs);
  return refl;
}  


/* Exported predicates */

/* reflection/1 Refl */
bool akl_reflection(Arg) Argdecl;
{
  Term X0;
  Deref(X0,A(0));

  if(IsRefl(X0))
    return TRUE;
  IfVarSuspend(X0);
  return FALSE;
}
/* reflective_call/2 Refl, Term, Refl */
bool akl_reflective_call(Arg)
     Argdecl;
{
  Term X0,X1,X2;

  reflection *refl;
  Term refl_t;
  bool foo;

#ifdef TRACE
  filter=init_filter();
#endif
  
  Deref(X0,A(0));
  Deref(X1,A(1));
  Deref(X2,A(2));

  if(IsVar(X0)) {
    if(IsSTR(X1)||IsATM(X1) ) {
      Term cons;
      Term new, point;
      exstate *rexs;
      predicate *pred;
      andbox *solved;
      Term free;
      int a, arity;

      refl = new_reflection(exs,exs->andb,X2);
      MakeCvaTerm(refl_t, (Gvainfo)refl);

      /* the first argument is unified with the port */
      foo = unify(X0,refl_t,exs->andb,exs);     

      rexs = refl->exstate;

      if(IsATM(X1) ) {
	arity = 1;
	pred = get_predicate(Atm(X1),1);
      }
      if(IsSTR(X1)) {
	arity = StrArity(Str(X1)) + 1;
	pred = get_predicate(StrName(Str(X1)),arity);
      }

      /* a new variable is loaded into yreg[0] */
      MakeVariableTerm(free,rexs->andb);
      rexs->andb->cont->yreg[0] = free;

      /* the argument registers are initialized */
      for(a = 0; a != (arity - 1); a++) {
	GetStrArg(rexs->areg[a], Str(X1), a);
      }
      /* the last argument is the unbound variable */
      rexs->areg[a] = free;	

      switch(pred->enter_instruction) {
      case ENTER_EMULATED:
	rexs->pc = pred->code.incoreinfo;
	rexs->arity = arity,
	rexs->def = pred;
	rexs->insert = NULL;

	MakeAndCont(exs->andb,NULL,exs->andb->cont,2);
	exs->andb->cont->yreg[0] = refl_t;
	exs->andb->cont->yreg[1] = X2;    
    
	engine(rexs);

	rexs->andb = NULL;	/* No longer relevant (and may be dead) */
	rexs->arity = 0;

	gc(rexs);		/* Do a final GC for ports that are unclosed */

	if(!EmptyRecall(rexs)) {

	  Start(rexs);

	  rexs->pc = halt;
	  rexs->insert = NULL;

	  engine(rexs);
	  rexs->andb = NULL;
	  rexs->arity = 0;
	}

	refl_t = exs->andb->cont->yreg[0];
	refl = Refl(refl_t);
	X2 = exs->andb->cont->yreg[1];

	exs->andb->cont = exs->andb->cont->next;
	solved = rexs->root->tried;
	
	/* Pick up the current insertion point of the stream */
	point = refl->stream;
	Deref(point, point);

	/* Construct a new cons and a new variable */
	MakeListTerm(cons,exs->andb);		

	InitVariable(LstCdrRef(Lst(cons)), exs->andb);

	if(solved != NULL) {
	  if(solved->tried == NULL) {
	    Term solution;
	    close_entry *curr, *last, *next;
	    /* forward the solved andbox to the current exstate */
	    PromoteAndbox(solved,exs->andb);
	    /* move all generic objects on the close list, belonging
	       to the solved and-box to the current exstate */
	    last = NULL;
	    next = NULL;
	    /* [BD] isn't this kind of dangerous, actually? */
	    for(curr = rexs->close; curr != NULL; curr = next) {
	      if(curr->type == GVAINFO_CLOSE) {
		gvainfo *g = (gvainfo*)curr->obj;
		DerefGvaEnv(g);
		if(Home(g->env) == exs->andb) {
		  /* Move it to our exstate */
		  if(last != NULL) {
		    last->next = curr->next;
		    next = curr->next;
		  } else {
		    rexs->close = curr->next;
		    next = curr->next;
		  }
		  curr->next = exs->close;
		  exs->close = curr;
		  continue;
		}
	      }
	      /* else */
	      last = curr;
	      next = curr->next;
	    }
		
	    solved->status = DEAD;
	    rexs->root->tried = solved->next;
	    if(solved->next != NULL)
	      solved->next->previous = NULL;
	    MakeStructTerm(solution, functor_solution_1, exs->andb);
	    StrArgument(Str(solution),0) = solved->cont->yreg[0];
	    /* The message to be sent */
	    LstCar(Lst(cons)) = solution;
	  } else {
	    LstCar(Lst(cons)) = term_suspended;
	  }
	} else {
	  LstCar(Lst(cons)) = term_fail;
	}
	/* cdr is made the new insertion point */
	GetLstCdr(new, Lst(cons));
	refl->stream = new;	
	/* unify the current insertion point with the new cons */
	return unify(point, cons, exs->andb,exs);
      case ENTER_C:
	Error("NYI");
	return FALSE;
      case ENTER_UNDEFINED:
	{
	  ENGINE_ERROR_2(AtmPname(pred->name), pred->arity, "undefined agent")
	  return FALSE;
	}
      }
      return FALSE;    
    }
    IfVarSuspend(X1);
  }
  return FALSE;
}





/* reflective_next/2 Refl, Refl */
bool akl_reflective_next(Arg)
     Argdecl;
{
  Term X0, X1;
  Term cons;
  Term new, point;
  exstate *rexs;
  andbox *solved;

  Deref(X0, A(0));
  Deref(X1, A(1));

#ifdef TRACE
  filter=init_filter();
#endif

  if(IsRefl(X0)) {
    DerefGvaEnv(RefGva(Ref(X0)));
    if(IsLocalGVA(Ref(X0),exs->andb)) {	
      rexs = Refl(X0)->exstate;
	
      MakeAndCont(exs->andb,NULL,exs->andb->cont,2);
      exs->andb->cont->yreg[0] = X0;
      exs->andb->cont->yreg[1] = X1;    
    
      engine(rexs);

      rexs->andb = NULL;	/* No longer relevant (and may be dead) */
      rexs->arity = 0;

      gc(rexs);			/* Do a final GC for ports that are unclosed */

      if(!EmptyRecall(rexs)) {

	Start(rexs);

	rexs->pc = halt;
	rexs->insert = NULL;

	engine(rexs);
	rexs->andb = NULL;
	rexs->arity = 0;
      }

      X0 = exs->andb->cont->yreg[0];
      X1 = exs->andb->cont->yreg[1];

      exs->andb->cont = exs->andb->cont->next;

      solved = rexs->root->tried;

      /* Pick up the current insertion point of the stream */
      point = Refl(X0)->stream;
      Deref(point, point);

      /* Construct a new cons and a new variable */
      MakeListTerm(cons,exs->andb);		
      InitVariable(LstCdrRef(Lst(cons)), exs->andb);
	
      if(solved != NULL) {
	if(solved->tried == NULL) {
	  Term solution;
	  close_entry *curr, *last, *next;
	  /* forward the solved andbox to the current exstate */
	  PromoteAndbox(solved,exs->andb);
	  /* move all generic objects on the close list, belonging
	     to the solved and-box to the current exstate */
	  last = NULL;
	  next = NULL;
	  /* [BD] isn't this kind of dangerous, actually? */
	  for(curr = rexs->close; curr != NULL; curr = next) {
	    if(curr->type == GVAINFO_CLOSE) {
	      gvainfo *g = (gvainfo*)curr->obj;
	      DerefGvaEnv(g);
	      if(Home(g->env) == exs->andb) {
		/* Move it to our exstate */
		if(last != NULL) {
		  last->next = curr->next;
		  next = curr->next;
		} else {
		  rexs->close = curr->next;
		  next = curr->next;
		}
		curr->next = exs->close;
		exs->close = curr;
		continue;
	      }
	    }
	    /* else */
	    last = curr;
	    next = curr->next;
	  }

	  solved->status = DEAD;
	  rexs->root->tried = solved->next;
	  if(solved->next != NULL)
	    solved->next->previous = NULL;
	  MakeStructTerm(solution, functor_solution_1, exs->andb);
	  StrArgument(Str(solution),0) = solved->cont->yreg[0];
	  /* The message to be sent */
	  LstCar(Lst(cons)) = solution;
	} else {
	  LstCar(Lst(cons)) = term_suspended;
	}
      } else {
	LstCar(Lst(cons)) = term_fail;
      }

      /* unify the current insertion point with the new cons */
      if(unify(point, cons, exs->andb,exs)) {
	/* cdr is made the new insertion point */
	GetLstCdr(new, Lst(cons));
	Refl(X0)->stream = new;
	/* the third argument is unified with the port */
	return unify(X0,X1,exs->andb,exs);     
      }
      /* if the insertion point was not unifiable with the new message */
      return FALSE;
    }
    return SUSPEND;
  }
  IfVarSuspend(X0);
  return FALSE;  
}

/* reflective_print/2 Refl, Refl */
bool akl_reflective_print(Arg)
     Argdecl;
{
  Term X0, X1;

  Deref(X0,A(0));
  Deref(X1,A(1));
  
  if(IsRefl(X0)) {
    DerefGvaEnv(RefGva(Ref(X0)));
    if(IsLocalGVA(Ref(X0),exs->andb)) {
      /* NULL insert.p. */
      display_choicebox(Refl(X0)->exstate->root,NULL,NULL);
      return unify(X0,X1,exs->andb,exs);     
    }
    return SUSPEND;
  }
  IfVarSuspend(X0);
  return FALSE;
}

/* close_reflection/1 Refl */
bool akl_close_refl(Arg)
     Argdecl;
{
  Term X0;

  Deref(X0,A(0));
  if(IsRefl(X0)) {
    DerefGvaEnv(RefGva(Ref(X0)));
    if(IsLocalGVA(Ref(X0),exs->andb)) {
       Term point;
       Refl(X0)->constr->method = NULL;
       Deref(point, Refl(X0)->stream);
       return unify(point, NIL, exs->andb,exs);
     }
    return FALSE;
  }
  return FALSE;
}


void initialize_reflection() {
  
  term_suspended = TagAtm(store_atom("suspended"));
  functor_solution_1 = store_functor(store_atom("solution"),1);  

  define("reflection", akl_reflection, 1);
  define("reflective_call", akl_reflective_call, 3);
  define("reflective_next", akl_reflective_next, 2);  
  define("reflective_print", akl_reflective_print, 2);  
}
