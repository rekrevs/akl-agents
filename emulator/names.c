/* $Id: names.c,v 1.11 1994/05/05 09:36:47 secher Exp $ */

/* There are three hash tables: atom, predicate and
 * functor. All are of equal size and uses the same
 * hashfunctions. Instead of having to compute the hash
 * value every time we access any hash table, we store the
 * hash value in the atom structure. 
 */ 

#include "include.h"
#include "term.h"
#include "predicate.h"
#include "config.h"
#include "names.h"
#include "storage.h"
#include "tree.h"
#include "exstate.h"
#include "error.h"
#include "functor.h"
#include "engine.h"


typedef struct predlist {
    struct predicate *def;
    struct predlist *next;
} predlist;

typedef struct atomlist {
    Atom a;
    struct atomlist *next;
} atomlist;

typedef struct functorlist {
    Functor functor;
    struct functorlist *next;
} functorlist;

static predlist		*predtable[ATOMHASHLEN];
static atomlist 	*atomtable[ATOMHASHLEN];
static functorlist 	*functortable[ATOMHASHLEN];

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


atomlist *make_atom_list(a,next)
     Atom a;
     atomlist *next;
{
  atomlist *res;

  res = (atomlist *) qalloc(sizeof(atomlist));
  res->a = a;
  res->next = next;
  return res;
}


functorlist *make_functor_list(fnk,next)
    Functor fnk;
    functorlist *next;
{
  functorlist *res;

  res = (functorlist *) qalloc(sizeof(functorlist));
  res->functor = fnk;
  res->next = next;
  return res;
}

predlist *make_predicate_list(def,next)
    predicate *def;
    predlist *next;
{
  predlist *res;

  res = (predlist *) qalloc(sizeof(predlist));
  res->def = def;
  res->next = next;

  return res;
}


int hash_atom(str)
    unsigned char *str;
{
  int     hval,i;
    
  hval = str[0] * 113;
  i = 0;
    
  while(str[i]!=0) i++;
  hval += str[i-1] * 109;
  hval += i;
    
  return hval % ATOMHASHLEN;
}


Atom store_atom(str)
    char *str;
{
  register atomlist *sal,*s;
  register int hv;
  
  hv = hash_atom(str);
    
  sal = atomtable[hv];

  if(sal != NULL) {
    while(sal != NULL) {
      if(!(strcmp(str,AtmPname(sal->a))))
	return sal->a;
      s = sal;
      sal = sal->next;
    }
    s->next = make_atom_list(make_atom(str,hv),NULL);
    return s->next->a;
  }

  atomtable[hv] = make_atom_list(make_atom(str,hv),NULL);
  return atomtable[hv]->a;
}


Term store_integer_term(str)
     char *str;
{
  Term res;
  res = integer_copy_to_constspace(atoi_term(str));
  return res;
}


Fp store_float(val)
    double val;
{
  Fp res;

  res = make_constant_float(val);
  return res;
}

Functor store_functor(a,arity)
    Atom a;
    int arity;
{
    register functorlist *fl, *f;
    register int hv;

    hv = HashNameArity(a,arity); /* This used to be AtmHash(a) */

    fl = functortable[hv];

    if(fl != NULL) {
	while(fl != NULL) {
	    if((FnkName(fl->functor) == a) && 
	       (arity == FnkArity(fl->functor)
		&& (fl->functor->features) == 0 )){ /* no features! */
	      /*printf("found it: ");
	      printf("name = %s \n",fl->functor->name->pname);*/
	      return fl->functor;
	    }
	    f = fl;
	    fl = fl->next;
	}
	f->next = make_functor_list(make_functor(a,arity),NULL);
	return f->next->functor;
    }
    
    functortable[hv] = make_functor_list(make_functor(a,arity),NULL);
    return functortable[hv]->functor;
}

void store_predicate(name,arity,def)
    Atom name;
    int arity;
    predicate *def;
{
  register predlist *pl, **bucket;

  bucket = &predtable[HashNameArity(name,arity)];

  pl = make_predicate_list(def, *bucket);
  *bucket = pl;
}


predicate *find_predicate(name,arity)
    Atom name;
    int arity;
{
  register predlist *pl, **bucket;

  bucket = &predtable[HashNameArity(name,arity)];

  for(pl = *bucket; pl != NULL; pl = pl->next) {
    if((pl->def->name == name) && (pl->def->arity == arity))
      return pl->def;
  }
  return NULL;
}



predicate *get_predicate(name,arity)
    Atom name;
    int arity;
{
  register predlist *pl, **bucket;
  predicate *def;

  bucket = &predtable[HashNameArity(name,arity)];

  for(pl = *bucket; pl != NULL; pl = pl->next) {
    if((pl->def->name == name) && (pl->def->arity == arity)) {
      return pl->def;
    }
  }
  /* make a undefined predicate, insert it in the bucket */
  def = make_u_predicate(name,arity);
  pl = make_predicate_list(def, *bucket);
  *bucket = pl;
  return def;
}


/*---------------------------------------------------------------------*
 * For extended_functors..                                             *
 *---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Create a functor with features. tbl contain feature names as terms. They
  must be sorted. If duplicates are found in tbl then return value is NULL. */

Functor create_functor(hv,name,features,arity,tbl)
     Term name;
     Term *tbl;
     int hv,features,arity;
{
  register functorlist *fl, *f;
  Functor fp;

  if (hv < 0)
    hv = -1*hv;

  fl = functortable[hv];

  if(fl != NULL) {
    while(fl != NULL) {
      if (Atm(name) == FnkName(fl->functor))
	if(same_features(fl->functor,tbl,features,arity)) {
#ifdef FUNCDEBUG
	  printf("found %5s/%.2d %.2d %.2d \n",AtmPname(fl->functor->name),fl->functor->arity,
	     fl->functor->nums,fl->functor->features);
#endif
	  return fl->functor;
	}
      f = fl;
      fl = fl->next;
    }
    if((fp = make_feat_functor(name,features,arity,tbl)) != NULL) 
      f->next = make_functor_list(fp,NULL);
    return fp;
  } else {
    if((fp = make_feat_functor(name,features,arity,tbl)) != NULL)
      functortable[hv] = make_functor_list(fp,NULL);
    return fp;
  }
}
