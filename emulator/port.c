/* $Id: port.c,v 1.48 1994/04/18 13:39:30 rch Exp $ */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "storage.h"
#include "unify.h"
#include "copy.h"
#include "gc.h"
#include "initial.h"
#include "config.h"
#include "display.h"
#include "error.h"
#include "names.h"


bool		install_port_constr();
void		reinstall_port_constr();
void		deinstall_port_constr();
void		promote_port_constr();
int		print_port_constr();
constraint	*copy_port_constr();
constraint	*gc_port_constr();

typedef struct port_constr {
  constrtable	*method;
  constraint	*next;
} port_constr;

static constrtable port_constr_methods = {
  install_port_constr,
  reinstall_port_constr,
  deinstall_port_constr,
  promote_port_constr,
  print_port_constr,
  copy_port_constr,
  gc_port_constr,
};


bool install_port_constr(constr,andb,exs)
     port_constr *constr;
     andbox *andb;
     exstate *exs;
{
  return TRUE;
}

void reinstall_port_constr(constr,andb)
     port_constr *constr;
     andbox *andb;
{
}

void deinstall_port_constr(constr)
     port_constr *constr;
{
}

void promote_port_constr(constr,prom,andb,exs)
     port_constr *constr;
     andbox *prom, *andb;
     exstate *exs;
{
  LinkConstr(andb,(constraint*)constr);
}

int print_port_constr(constr)
     port_constr *constr;
{
  printf("{port %#lx}", (long)constr);
  return 0;
}

constraint *copy_port_constr(old)
     port_constr *old;
{
  port_constr *new;
  NEW(new, port_constr);
  return (constraint*)new;
}

constraint *gc_port_constr(old,gcst)
     port_constr *old;
     gcstatep gcst;
{
  port_constr *new;
  NEW(new, port_constr);
  return (constraint*)new;
}


/* Create a port constraint and store it in the current andbox */

constraint *make_port_constraint(exs)
     exstate *exs;
{
  port_constr *constr;
  NEW(constr, port_constr);
  constr->method = &port_constr_methods;

  LinkConstr(exs->andb, (constraint*)constr);

  return (constraint*)constr;
}


/* These are meant to be called by objects that reference constraints */

constraint *copy_referenced_constraint(old)
    constraint *old;
{
  constraint *new;
  if (IsCopied(old)) {
    new = (constraint*)Forw(old);
  } else {
    if (old->method != NULL)
      new = old->method->copy(old);
    else
      NEW(new, constraint);
    new->method = old->method;
    copy_forward(old, new);
  }
  return new;
}

constraint *gc_referenced_constraint(old,gcst)
    constraint *old;
    gcstatep gcst;
{
  constraint *new;
  if (IsCopied(old)) {
    new = (constraint*)Forw(old);
  } else {
    if (old->method != NULL)
      new = old->method->gc(old,gcst);
    else
      NEW(new, constraint);
    new->method = old->method;
    gc_forward(old, new);
  }
  return new;
}


/* This is meant to be called by "deallocate" methods, to update the stream.
 * It checks if the stream is uncopied, and in case it is instantiated
 * to non-NIL, it replaces the stream with the atom 0 in order to force
 * the port closing to fail without having to copy the stream.
 */

void close_port_stream(s)
    Term *s;
{
  register Term x = *s;
  while (IsREF(x)) {
    if (IsCopied(Ref(x))) {
      x = TagRef(Forw(Ref(x)));
      break;
    } else if (IsUnBoundRef(Ref(x))) {
      break;
    }
    x = RefTerm(Ref(x));
  }
  /* If stream is instantiated to non-NIL, force failure without copying */
  if (!(IsREF(x) && IsUnBoundRef(Ref(x)) || Eq(x,NIL)))
    *s = MakeSmallNum(0);

  return;
}


/* This is meant to be called by "uncopied" methods, in order to check
 * if the copied port needs to be entered on the close list. It also
 * does early update of the stream (as in "close_port_stream"), and
 * early killing of the port constraint.
 */

int port_needs_copy_close(s, constr)
    Term *s;
    constraint *constr;
{
  register Term x = *s;
  while (IsREF(x)) {
    if (IsCopied(Ref(x))) {
      x = TagRef(Forw(Ref(x)));
      break;
    } else if (IsUnBoundRef(Ref(x))) {
      /* The stream is an unbound variable which is not referenced.
       * We must make sure that the corresponding port constraint
       * will not cause suspension. If the port constraint has not
       * been copied yet, we zap the original. This is safe, since
       * any awakening of the original andbox would otherwise result
       * in suspension, followed by closing at the next GC.
       */
      if (IsCopied(constr))
	constr = ((constraint*)Forw(constr));
      constr->method = NULL;
      return 0;
    }
    x = RefTerm(Ref(x));
  }
  /* If stream is instantiated to non-NIL, force failure without copying */
  if (!(IsREF(x) && IsUnBoundRef(Ref(x)) || Eq(x,NIL)))
    *s = MakeSmallNum(0);
    
  return 1;
}


Gvainfo newport();
bool 	unifyport();
int 	printport();
Gvainfo	copyport();
Gvainfo	gcport();
int     uncopiedport();
envid	*deallocateport();
int	killport();
bool	closeport();


typedef struct port {
  gvamethod	*method;
  envid 	*env;
  Term 		stream;
  constraint	*constr;
} port;


static gvamethod portmethod = {
  newport,
  unifyport,
  printport,
  copyport,
  gcport,
  uncopiedport,  
  deallocateport,
  killport,
  closeport,
  NULL,
  NULL,
  NULL
};


#define IsPort(Prt) (IsCvaTerm(Prt) && \
		     RefCvaMethod(Ref(Prt)) == &portmethod)

#define GvaPort(g)	((port*)(g))
#define Port(x)		GvaPort(RefGva(Ref(x)))


predicate *send3_def;


Gvainfo newport(old)
     port *old;
{
  port *new;
  NEW(new,port);
  return (Gvainfo) new;
}

Gvainfo copyport(old,new)
     port *old, *new;
{
  new->stream = old->stream;
  copy_location(&new->stream);
  new->constr = copy_referenced_constraint(old->constr);
  return (Gvainfo) new;
}

Gvainfo gcport(old,new,gcst)
     port *old, *new;
     gcstatep gcst;
{
  new->stream = old->stream;
  gc_location(&new->stream,gcst);
  new->constr = gc_referenced_constraint(old->constr,gcst);
  return (Gvainfo) new;
}


envid *deallocateport(prt)
     Gvainfo prt;
{
  close_port_stream(&GvaPort(prt)->stream);

  /* Always close, never kill, since a suspended guard may */
  /* be promoted even if both port and stream are garbage. */
  /* By closing, we make sure that the andbox will wake up */

  return prt->env;	  /* Environment dereferencing is done by gc. */
}


bool closeport(Arg)
     Argdecl;
{
  Term X0;
  Term point;
  
  X0 = A(0);

  Port(X0)->constr->method = NULL;	/* Kill the constraint */

  Deref(point, Port(X0)->stream);
  return unify(point, NIL, exs->andb,exs);
}


int uncopiedport(prt)
     Gvainfo prt;
{
  DerefGvaEnv(prt);
  if(InCopyEnv(prt->env)) {
    return port_needs_copy_close(&GvaPort(prt)->stream, GvaPort(prt)->constr);
  } else {
    return 0;
  }
}


bool unifyport(prt,y,andb,exs)
     Term prt;
     Term y;
     andbox *andb;
     exstate *exs;
{
  if(Eq(prt, y))
    return TRUE;
  return FALSE;
}


int killport(prt)
     port *prt;
{
  constraint *constr = prt->constr;
  if (IsCopied(constr))
    constr = ((constraint*)Forw(constr));
  constr->method = NULL;

  return 1;
}


int printport(file,prt,tsiz)
     FILE *file;
     port *prt;
     int tsiz;
{
  Term s;
  Deref(s, prt->stream);
  if(IsVar(s))
    fprintf(file,"{port: _%d}",variable_descriptor(s));
  else
    fprintf(file,"{port: ...}");
  return 1;
}


/* These predicate are exported to the AKL level.
 *
 *
 */


/* port/1 Port */
bool akl_port(Arg)
     Argdecl;
{
  Term X0;
  Deref(X0,A(0));
  
  if(IsPort(X0))
    return TRUE;
  else if(IsCvaTerm(X0) && RefCvaMethod(Ref(X0))->send != NULL)
    return TRUE;
  IfVarSuspend(X0);
  return FALSE;
}
  

/* open_port/2 Port, List */
bool akl_open_port(Arg)
     Argdecl;
{
  Term X0, X1, res;
  port *prt;
  
  Deref(X0,A(0));		/* The term unified with the port */
  Deref(X1,A(1));		/* The stream associated ith the port */

  MakeGvainfo(prt,port,&portmethod,exs->andb);
  prt->stream = X1;
  prt->constr = make_port_constraint(exs);

  add_gvainfo_to_close((Gvainfo)prt,exs);
  
  MakeCvaTerm(res, (Gvainfo)prt);

  return unify(X0,res,exs->andb,exs);
}


/* close_port/1 Port */
bool akl_close_port(Arg)
     Argdecl;
{
  Term X0;
  
  Deref(X0,A(0));

  if(IsPort(X0)) {
    Gvainfo g = RefGva(Ref(X0));
    DerefGvaEnv(g);
    if(IsLocalEnv(g->env,exs->andb)) {
      Term point;
      GvaPort(g)->constr->method = NULL;	/* Kill the constraint */
      Deref(point, GvaPort(g)->stream);
      return unify(point, NIL, exs->andb,exs);
    }
    /* A constraint is constructed without a suspension.
     * The constraint will be retried when promoted.
     */
    return SUSPEND;
  }
  IfVarSuspend(X0);
  return FALSE;
}


/* send(Term, Port) */

bool akl_send_2(Arg)
     Argdecl;
{
  Term theport,message;
  
  Deref(message,A(0));		/* The message to send */
  Deref(theport,A(1));		/* The port to send to */

  if(IsCvaTerm(theport)) {
    DerefGvaEnv(RefGva(Ref(theport)));
    if(IsLocalGVA(Ref(theport),exs->andb)) {

      if(IsPort(theport)) {
	Term cons;
	Term new, point;
	
	/* Pick up the current insertion point of the stream */
	point = Port(theport)->stream;
	
	Deref(point, point);
	
	/* Construct a new cons and a new variable */
	MakeListTerm(cons,exs->andb);		
	InitVariable(LstCdrRef(Lst(cons)), exs->andb);
	
	/* The message to be sent */
	LstCar(Lst(cons)) = message;
	
	/* cdr is made the new insertion point */
	GetLstCdr(new, Lst(cons));
	Port(theport)->stream = new;
	
	/* unify the current insertion point with the new cons */
	return unify(point, cons, exs->andb, exs);

      } else {
	bool (*sfunc)() = RefCvaMethod(Ref(theport))->send;
	
	if(sfunc != NULL)
	  return sfunc(message, theport, exs);
	else
	  return FALSE;
      }

    } else {
      /* The port is not local to the current andbox */
      /* A constraint is constructed without a suspension.
       * The constraint will be retried when promoted.
       */
/*      WARNING("send/2: non-local port - suspending");*/
      return SUSPEND;
    }
  }
  IfVarSuspend(theport);
  return FALSE;
}

/* send(Term, InPort, OutPort) */

bool akl_send_3(Arg)
     Argdecl;
{
  Term inport,message,outport;
  
  Deref(message,A(0));		/* The message to send */
  Deref(inport, A(1));		/* The port to send to */
  Deref(outport,A(2));		/* The port after the message */

  if(IsCvaTerm(inport)) {
    DerefGvaEnv(RefGva(Ref(inport)));
    if(IsLocalGVA(Ref(inport),exs->andb)) {

      if(IsPort(inport)) {
	Term cons;
	Term new, point;
	
	/* Pick up the current insertion point of the stream */
	point = Port(inport)->stream;
	
	Deref(point, point);
	
	/* Construct a new cons and a new variable */
	MakeListTerm(cons,exs->andb);
	InitVariable(LstCdrRef(Lst(cons)), exs->andb);
	
	/* The message to be sent */
	LstCar(Lst(cons)) = message;
	
	/* cdr is made the new insertion point */
	GetLstCdr(new, Lst(cons));
	Port(inport)->stream = new;
	
	/* unify the current insertion point with the new cons */
	/* the third argument is unified with the port */      
	
	return (unify(point, cons, exs->andb, exs)
		&& unify(inport,outport,exs->andb,exs));

      } else {
	bool (*sfunc)() = RefCvaMethod(Ref(inport))->send;
	bool res;
	
	if(sfunc != NULL)
	  res = sfunc(message, inport, exs);
	else
	  return FALSE;
	
	if(res == TRUE)
	  return unify(inport,outport,exs->andb,exs);
	else
	  return res;
	
      }
    
    } else {
      /* The port is not local to the current andbox */
      /* A constraint is constructed without a suspension.
       * The constraint will be retried when promoted.
       */
/*      WARNING("send/3: non-local port - suspending");*/
      return SUSPEND;
    }
  }
  IfVarSuspend(inport);
  return FALSE;
}


void initialize_port() {

  define("port",akl_port,1);
  define("open_port",akl_open_port,2);
  define("close_port",akl_close_port,1);
  define("send",akl_send_2,2);      
  define("send",akl_send_3,3);

  send3_def = get_predicate(store_atom("send"), 3);
}
