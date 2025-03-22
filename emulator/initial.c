/* $Id: initial.c,v 1.61 1994/05/17 13:16:28 jm Exp $ */


#include "include.h"
#include "instructions.h"
#include "term.h"
#include "config.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "code.h"
#include "names.h"
#include "initial.h"
#include "time.h"
#include <string.h>
#include "intrpt.h"
#include "unify.h"

/* modules */
#include "aggregate.h"
#include "is.h"
#include "compare.h"
#include "port.h"
#include "builtin.h"
#include "bignum.h"
#include "rational.h"

#include "array.h"
#include "storage.h"
#include "foreign.h"
#include "load.h"
#include "inout.h"
#include "statistics.h"
#include "candidate.h"
#include "reflection.h"
#include "display.h"
#include "debug.h"
#include "trace.h"
#include "error.h"
#include "ghash.h"
#include "abstraction.h"

#include "encodeinstr.h"
#include "instrdefs.h"
#include "fd.h"

extern void install_foreign_predicates PROTO((void));


code *halt;
code *proceed;
code *fail_dec;

Term term_true;
Term term_fail;
Term term_nil;

static void init_halt()
{
  halt = codecurrent;
  Instr_None(GUARD_HALT);
}

static void init_proceed()
{
  proceed = codecurrent;
  Instr_None(DEALLOC_PROCEED);
}

static void init_fail_dec()
{
#ifdef BAM
  fail_dec = codecurrent;
  Instr_None(FAIL_DEC);
#endif
}

void define(pname, procedure, arity)
    char *pname;
    bool (*procedure)();
    int arity;
{
  define_c_predicate(store_atom(pname), arity, procedure);
}

bool agents_features(Arg)
    Argdecl;
{
  char buf[200];
  Term X0;
  Deref(X0, A(0));

  buf[0] = 0;

#ifdef THREADED_CODE
  strcat(buf, "threaded code");
#else
  strcat(buf, "non-threaded code");
#endif

#ifdef BAM
  strcat(buf, ", BAM");
#endif

  return unify(X0, TagAtm(store_atom(buf)), exs->andb, exs);
}

static void init_intrinsics() {

  /* misc intrinsics in modules */

  initialize_time();

  initialize_is();
  initialize_compare();
  initialize_port();
  initialize_builtin();
  initialize_bignum();
#ifdef RATNUM
  initialize_rational();
#endif
  initialize_array();
  initialize_hash();
  initialize_storage();
  initialize_foreign();
  initialize_load();
  initialize_inout();
  initialize_statistics();
  initialize_candidate();
  initialize_reflection();
  initialize_display();
  initialize_debug();
  initialize_trace();
  initialize_predicate();
  initialize_aggregate();
  initialize_finite();
  initialize_error();
  initialize_abstraction();

  define("agents_features",agents_features,1);

  install_foreign_predicates();
}



void init_constants()
{
  term_nil =	TagAtm(store_atom("[]"));
  term_true =	TagAtm(store_atom("true"));
  term_fail =	TagAtm(store_atom("fail"));
}


void init_each_time()
{
  initialize_intrpt();
  compute_cwd();
}

void reinitialise()
{
  fflush(stdout);
  fflush(stderr);
}

static uword dummy[2];

void check_architecture()
{
  Term x, y;
  uword *xp, *yp;
  void (*pp)();
  Atom xa;
  xp = (uword*) OptionalWordAlign(&dummy[0]);
  yp = (uword *) malloc(sizeof(uword));
  pp = check_architecture;
  x = TagGva(xp);
  y = TagStr(yp);
  if (!IsGVA(x) ||
      !IsSTR(y) ||
      Gva(x) != (gvainfo*)xp ||
      Str(y) != (Structure)yp)
    {
      FatalError("The tag scheme needs fixing on this machine");
    }

  free(yp);

  if (((Tad(x) & MarkBitMask) != 0) ||
      ((Tad(y) & MarkBitMask) != 0) ||
      (((uword) pp) & /*0x3*/ 0 != 0))	/* [BD] Detta kunde vara snyggare */
    {
      FatalError("The GC/copy marking scheme needs fixing on this machine");
    }

  x = MakeSmallNum(4711);
  y = MakeSmallNum(-4711);
  if (!IsNUM(x) || !IsNUM(y) ||
      GetSmall(x) != 4711 || GetSmall(y) != -4711)
    {
      FatalError("The small integer scheme needs fixing on this machine");
    }

  xa = (Atom) AddressOth(NullTerm);	/* Fake an atom */
  x = TagAtm(xa);
  if (!IsATM(x) || Atm(x) != xa)
    {
      FatalError("The atom scheme needs fixing on this machine");
    }
}


void initialise()
{
  check_architecture();
  init_alloc();
  init_code();
  init_atomtable();
  init_predtable();
  init_functortable();
/*  mark_po();			BC added  temporary */
  init_constants();

  init_halt();
  init_proceed();
  init_fail_dec();

  init_intrinsics();

  init_each_time();
}
