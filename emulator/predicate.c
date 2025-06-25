/* $Id: predicate.c,v 1.12 1994/03/25 14:42:21 jm Exp $ */

#include "include.h"
#include "term.h"
#include "initial.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "trace.h"
#include "names.h"
#include "storage.h"
#include "code.h"

bool pred_protect;
bool pred_protection;

/* module *akl_module = NULL;		/ * basic module */
/*
void new_module(name,protected)
     Atom name;
     bool protected;
{
  module *mod = (module *) salloc(sizeof(module));
  
  mod->name = name;
  mod->members = NULL;
  mod->protected = protected;

  mod->next = akl_module;
  akl_module = mod;
}


void insert_pred_into_module(pred,mod)
     predicate *pred;
     module *mod;
{
  if (akl_module->members == NULL)
    {
      akl_module->members = pred;
      pred->mod.prev = NULL;
      pred->mod.next = NULL;
    }
  else
    {
      predicate *next_pred = akl_module->members;
      predicate *prev_pred = next_pred->mod.prev;

      akl_module->members = pred;
      next_pred->mod.prev = prev_pred->mod.next = pred;
      pred->mod.next = next_pred;
      pred->mod.prev = prev_pred;
    }
}
*/

predicate *make_e_predicate(name,arity,c)
    Atom name;
    int arity;
    code *c;
{
    predicate *def;

    def = (predicate *) qalloc(sizeof(predicate));
    def->enter_instruction = ENTER_EMULATED;
    def->predicate_type = EMULATED_PREDICATE;
    def->protected = pred_protect;
    def->arity = arity;
    def->ext = 0;
    def->name = name;
    def->code.incoreinfo = c;
#ifdef TRACE
    def->spypoint = NOSPY;
#endif
    return def;
}

predicate *make_c_predicate(name,arity,func)
    Atom name;
    int arity;
    bool (*func)();
{
    predicate *def;

    def = (predicate *) qalloc(sizeof(predicate));
    def->enter_instruction = ENTER_C;
    def->predicate_type = C_PREDICATE;
    def->protected = pred_protect;
    def->arity = arity;
    def->ext = 0;
    def->name = name;
    def->code.cinfo = func;
#ifdef TRACE
    def->spypoint = NOSPY;
#endif
    return def;
}

predicate *make_u_predicate(name,arity)
    Atom name;
    int arity;
{
    predicate *def;

    def = (predicate *) qalloc(sizeof(predicate));
    def->enter_instruction = ENTER_UNDEFINED;
    def->predicate_type = UNDEFINED_PREDICATE;
    def->protected = FALSE;
    def->arity = arity;
    def->ext = 0;
    def->name = name;
#ifdef TRACE
    def->spypoint = NOSPY;
#endif
    return def;
}

predicate *make_abstraction(arity,ext,c)
    int arity;
    int ext;
    code *c;
{
    predicate *def;

    def = (predicate *) qalloc(sizeof(predicate));
    def->enter_instruction = ENTER_EMULATED;
    def->predicate_type = EMULATED_PREDICATE;
    def->protected = pred_protect;
    def->arity = arity;
    def->ext = ext;
    def->name = store_atom("<abstraction>");
    def->code.incoreinfo = c;
#ifdef TRACE
    def->spypoint = NOSPY;
#endif
    return def;
}

void redefine_e_predicate(def,c)
     predicate *def;
     code *c;
{
    def->enter_instruction = ENTER_EMULATED;
    def->predicate_type = EMULATED_PREDICATE;
    def->protected = pred_protect;
    def->code.incoreinfo = c;
#ifdef TRACE
    def->spypoint = NOSPY;
#endif
}

void redefine_c_predicate(def,func)
     predicate *def;
     bool (*func)();
{
    def->enter_instruction = ENTER_C;
    def->predicate_type = C_PREDICATE;
    def->protected = pred_protect;
    def->code.cinfo = func;
#ifdef TRACE
    def->spypoint = NOSPY;
#endif
}

void define_e_predicate(name,arity,c)
    Atom name;
    int arity;
    code *c;
{
  predicate *def;
  def = find_predicate(name,arity);
  if(def != NULL) {
    if(def->protected == TRUE && pred_protection == TRUE) {
      printf("{Warning: %s/%d - attempt to redefine protected predicate}\n",
	     AtmPname(name), arity);
    } else {
      redefine_e_predicate(def,c);
    }
  } else {
    def = make_e_predicate(name,arity,c);
    store_predicate(name,arity,def);
  }
}


void define_c_predicate(name,arity,func)
    Atom name;
    int arity;
    bool (*func)();
{
  predicate *def;
  def = find_predicate(name,arity);
  if(def != NULL) {
    if(def->protected == TRUE && pred_protection == TRUE) {
      printf("{Warning: %s/%d - attempt to redefine protected predicate}\n",
	     AtmPname(name), arity);
    } else {
      redefine_c_predicate(def,func);
    }
  } else {
    def = make_c_predicate(name,arity,func);
    store_predicate(name,arity,def);
  }
}

void define_abstraction(number,arity,ext,c)
    int number;
    int arity;
    int ext;
    code *c;
{
  predicate *def;
  def = make_abstraction(arity,ext,c);
  store_abstraction(number,def);
}


bool akl_protect(Arg)
    Argdecl;
{
    pred_protect = TRUE;

    return TRUE;
}

bool akl_noprotect(Arg)
    Argdecl;
{
    pred_protect = FALSE;

    return TRUE;
}

bool akl_unprotectdef(Arg)
    Argdecl;
{
    Term A0, A1;
    Deref(A0, A(0));
    Deref(A1, A(1));

    if (IsATM(A0) && IsNUM(A1)) {
      predicate *def = get_predicate(Atm(A0), GetSmall(A1));
      def->protected = FALSE;
      return TRUE;
    } else {
      IfVarSuspend(A0);
      IfVarSuspend(A1);
      return FALSE;
    }
}

bool akl_protection(Arg)
    Argdecl;
{
    pred_protection = TRUE;

    return TRUE;
}

bool akl_noprotection(Arg)
    Argdecl;
{
    pred_protection = FALSE;

    return TRUE;
}

void initialize_predicate() {

  define("protect", akl_protect, 0);
  define("noprotect", akl_noprotect, 0);
  define("unprotectdef", akl_unprotectdef, 2);
  define("protection", akl_protection, 0);
  define("noprotection", akl_noprotection, 0);
}
