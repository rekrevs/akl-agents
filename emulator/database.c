/* $Id: database.c,v 1.7 1992/01/29 13:35:11 sverker Exp $ */

/* There are four hash tables: atom, predicate, builtin and
 * functors. All are of equal size and uses the same
 * hashfunctions. Instead of having to compute the hash
 * value every time we access any hash table, we store the
 * hash value in the atom structure.
 */ 

#include "include.h"


#define HashNameArity(N,A) ((N)->hashval)


predlist 		*predtable[ATOMHASHLEN];
static storeatomlist 	*atomtable[ATOMHASHLEN];
static functorlist 	*functortable[ATOMHASHLEN];

BOOL pred_protect;
BOOL pred_protection;


atom make_atom(str,hashval)
    char *str;
    int hashval;
{
    atom res;
    register int len;

    res = (atom) atom_alloc(sizeof(struct atom));

    for(len = 0; str[len]!=0; len++);

    res->pname = (char *) atom_alloc((sizeof(char) * (len + 1)) +
				(4 - (sizeof(char) * (len + 1)) % 4));
    res->type = ATM;
    res->hashval = hashval;

    (void) strcpy(res->pname,str);

    return res;
}


predlist *make_predicate_list(def,next)
    definition *def;
    predlist *next;
{
    predlist *res;

    res = (predlist *) atom_alloc(sizeof(predlist));
    res->def = def;
    res->next = next;

    return res;
}

functorlist *make_functor_list(a,arity,next)
    atom a;
    int arity;
    functorlist *next;
{
    functorlist *res;

    res = (functorlist *) atom_alloc(sizeof(functorlist));
    res->functor = (functor) atom_alloc(sizeof(struct functor));
    res->functor->name = a;
    res->functor->arity = arity;
    res->next = next;

    return res;
}

definition *make_definition_emulated(name,arity,c)
    atom name;
    int arity;
    code *c;
{
    definition *def;

    def = (definition *) atom_alloc(sizeof(definition));
    def->enter_instruction = ENTER_EMULATED;
    def->predicate_type = EMULATED_PREDICATE;
    def->arity = arity;
    def->name = name;
    def->code.incoreinfo = c;
    def->spypoint = NOSPY;

    return def;
}

definition *make_definition_c(name,arity,func)
    atom name;
    int arity;
    BOOL (*func)();
{
    definition *def;

    def = (definition *) atom_alloc(sizeof(definition));
    def->enter_instruction = ENTER_C;
    def->predicate_type = C_PREDICATE;
    def->arity = arity;
    def->name = name;
    def->code.cinfo = func;
    def->spypoint = NOSPY;

    return def;
}

definition *make_undefined(name,arity)
    atom name;
    int arity;
{
    definition *def;

    def = (definition *) atom_alloc(sizeof(definition));
    def->enter_instruction = ENTER_UNDEFINED;
    def->predicate_type = UNDEFINED_PREDICATE;
    def->arity = arity;
    def->name = name;
    def->spypoint = NOSPY;

    return def;
}

storeatomlist *make_store_atom_list(a,next)
    atom a;
    storeatomlist *next;
{
    storeatomlist *res;

    res = (storeatomlist *) atom_alloc(sizeof(storeatomlist));
    res->a = a;
    res->next = next;
    
    return res;
}

int hash_atom(str)
    char *str;
{
    int     hval,i;
    
    hval = str[0] * 113;
    i = 0;
    
    while(str[i]!=0) i++;
    hval += str[i-1] * 109;
    hval += i;
    
    return hval % ATOMHASHLEN;
}

void init_atomtable()
{
    int	i;

    for(i=0 ; i != ATOMHASHLEN ; i++)
	atomtable[i] = NULL;
}

void init_predtable()
{
    int i;

    for(i=0 ; i != ATOMHASHLEN ; i++)
	predtable[i] = NULL;
}

void init_functortable()
{
    int i;
    
    for(i=0 ; i != ATOMHASHLEN ; i++)
	functortable[i] = NULL;
}


atom store_atom(str)
    char *str;
{
    register storeatomlist *sal,*s;
    register int hv;

    hv = hash_atom(str);
    
    sal = atomtable[hv];

    if(sal != NULL) {
	while(sal != NULL) {
	    if(strcmp(str,sal->a->pname) == 0)
		return sal->a;
	    s = sal;
	    sal = sal->next;
	}
	s->next = make_store_atom_list(make_atom(str,hv),NULL);
	return s->next->a;
    }

    atomtable[hv] = make_store_atom_list(make_atom(str,hv),NULL);
    return atomtable[hv]->a;
}

/* kludges ahead */

void store_predicate(name,arity,def)
    atom name;
    int arity;
    definition *def;
{
    register predlist *pl, **bucket;

    bucket = &predtable[HashNameArity(name,arity)];

    def->protected = pred_protect;

    for(pl = *bucket; pl != NULL; pl = pl->next) {
	if((pl->def->name == name) && (pl->def->arity == arity)) {
	  if(pl->def->protected == TRUE && pred_protection == TRUE)
	    printf("{Warning: %s/%d - attempt to redefine built_in predicate}\n", name->pname, arity);
	  else
	    *(pl->def) = *def;
	  return;
	}
    }

    pl = make_predicate_list(def, *bucket);
    *bucket = pl;
}

/**/

void store_emulated_predicate(name,arity,c)
    atom name;
    int arity;
    code *c;
{
    store_predicate(name,arity,make_definition_emulated(name,arity,c));
}

void store_c_predicate(name,arity,func)
    atom name;
    int arity;
    BOOL (*func)();
{
    store_predicate(name,arity,make_definition_c(name,arity,func));
}

void store_undefined_predicate(name,arity)
    atom name;
    int arity;
{
    store_predicate(name,arity,make_undefined(name,arity));
}

definition *get_definition(name,arity)
    atom name;
    int arity;
{
    register predlist *pl, **bucket;

    bucket = &predtable[HashNameArity(name,arity)];

    for(pl = *bucket; pl != NULL; pl = pl->next) {
	if((pl->def->name == name) && (pl->def->arity == arity))
	    return pl->def;
    }

    pl = make_predicate_list(make_undefined(name,arity), *bucket);
    *bucket = pl;

    return pl->def;
}

functor store_functor(a,arity)
    atom a;
    int arity;
{
    register functorlist *fl, *f;
    register int hv;

    hv = a->hashval;

    fl = functortable[hv];

    if(fl != NULL) {
	while(fl != NULL) {
	    if((fl->functor->name == a) && (arity == fl->functor->arity)) {
		return fl->functor;
	    }
	    f = fl;
	    fl = fl->next;

	}
	f->next = make_functor_list(a,arity,NULL);
	return f->next->functor;
    }
    
    functortable[hv] = make_functor_list(a,arity,NULL);
    return functortable[hv]->functor;
}

predlist *get_predlist(a)
     atom a;
{
  return predtable[a->hashval];
}
