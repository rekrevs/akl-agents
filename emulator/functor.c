#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "gc.h"
#include "copy.h"
#include "storage.h"
#include "config.h"
#include "initial.h"
#include "builtin.h"
#include "error.h"
#include "names.h"
#include "functor.h"
#include "display.h"

#include <string.h>


/* -------------------------------------------------------------------- */

/*  **** MAXIMUM NUMBER OF FEATURES SET TO 4096 ***                      */

#define POWERTABLELENGTH 13

static power_table[POWERTABLELENGTH] = 
      {0,1,4,8,16,32,64,128,256,512,1024,2048,4096};

#define EMPTY_ENTRY -1


/*#define FUNCDEBUG*/
/*#define FUNCDEBUGTABLE*/

/* -------------------------------------------------------------------- 
   Stuff related to the generic type functor */

Generic	newfunctor();
bool 	unifyfunctor();
int 	printfunctor();

method functormethod =
{
  newfunctor,
  unifyfunctor,
  printfunctor,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* -------------------------------------------------------------------- */

Generic newfunctor(old)
     Functor *old;
{
  return (Generic) NULL;
}

/* -------------------------------------------------------------------- */

bool unifyfunctor(x,y,andb,exs)
     Term x, y;
     andbox *andb;
     exstate *exs;
{

  /* Start by checking fatal error.
     If y is a functor then check if x & y is the same,
     otherwise try to unify them.
     Else (y not functor) if y is variabel then bind y to x.
     Else (y not variable) FAIL 
     */

  if (!IsFUNC(x)) {
    FatalError("unifyfunctor called with non-Functor argument");
  } else {
    if (IsFUNC(y)) {
      if (Eq(x,y)) {
	return TRUE;		/* xt & yt is the same functor */
      } else {
	return FALSE;           
      }
    } else {
      if (IsVar(y)) 
	return unify_more(y,x,andb,exs);
    }    
  }
  return FALSE;			/* Catch all ather cases */
}

/* -------------------------------------------------------------------- */

int printfunctor(file,f,tsiz)
     FILE *file;
     Functor f;
     int tsiz;
{

  Term *names = FnkNames(f);
  int i,rest=0,flag=0;

  fprintf(file,"{functor: '%s' [",FnkPname(f));

  /*Print out features less than 1 */
  if (IsINT(names[0])) 
    if (GetInteger(names[0]) < 1) {
      fdisplay_term(file,names[0],tsiz-1);
      flag = 1;
    }
  for(i=1 ; i < FnkFeatures(f); i++) {
    if (IsINT(names[i])) {
      if (GetInteger(names[i]) < 1) {
	fprintf(file,",");
	fdisplay_term(file,names[i],tsiz-1);
	flag =1;
      } else 
	break;
    } else
      break;
  }
  if (flag) 
    rest = i;			

  /* Print out numbers */
  if (FnkNums(f)) {
    if (flag) {
      rest = i;
      fprintf(file,",");
    }
    for(i=0 ; i < FnkNums(f);i++) {
      fprintf(file,"%d",i+1);
      if(i != (FnkNums(f) - 1))      
	fprintf(file,",");
    }
    flag = 1;
  }

  /* Print out features > 1 */
  if (rest < FnkFeatures(f)) {
    if (flag)
      fprintf(file,",");
    for(i=rest ; i < FnkFeatures(f); i++) {
      fdisplay_term(file,names[i],tsiz-1);
      if(i != (FnkFeatures(f) - 1))
	fprintf(file,",");
    }
  }
  fprintf(file,"]}");
  return 1;
}

/* -------------------------------------------------------------------- 
   Stuff related to the generic type functor0 */

Generic	newfunctor0();
bool 	unifyfunctor0();
int 	printfunctor0();
Generic	copyfunctor0();
Generic	gcfunctor0();

method functor0method =
{
  newfunctor0,
  unifyfunctor0,
  printfunctor0,
  copyfunctor0,
  gcfunctor0,
  NULL,
  NULL,
  NULL,
  NULL
};

/* -------------------------------------------------------------------- */

Generic newfunctor0(old)
     functor0 *old;
{
  functor0 *new;
  NewHeapFunctor(new);
  return (Generic) new;
}

/* -------------------------------------------------------------------- */

Generic copyfunctor0(old,new)
     functor0 *old, *new;
{
  new->name = old->name;
  copy_location(&(new->name));
  return (Generic) new;
}

/* -------------------------------------------------------------------- */

Generic gcfunctor0(old,new,gcst)
     functor0 *old, *new;
     gcstatep gcst;
{
  new->name = old->name;
  gc_location(&(new->name),gcst);
  return (Generic) new;
}

/* -------------------------------------------------------------------- */

bool unifyfunctor0(x,y,andb,exs)
     Term x, y;
     andbox *andb;
     exstate *exs;
{

  /* Start by checking fatal error.
     If y is a functor then check if x & y both have arity = 0.
     Else (not arity = 0) FAIL
     Else (y not functor) if y is variabel then bind y to x.
     Else (y not variable) FAIL 
     */

  if (!IsFUNC(x)) {
    FatalError("unifyfunctor called with non-Functor argument");
  } else {
    if (IsFUNC(y)) {
      if ((FnkArity(Fnk(x)) == 0) && (FnkArity(Fnk(y)) == 0)) {
	Term nx,ny;
	functor0 *fx = (functor0*)Fnk(x);
	functor0 *fy = (functor0*)Fnk(y);

	GetArg(nx,&(fx->name));
	GetArg(ny,&(fy->name));
	return unify_more(nx,ny,andb,exs);
      } else
	return FALSE;
    } else {
      if (IsVar(y)) 
	return unify_more(y,x,andb,exs);
    } 
  }
  return FALSE;			/* Catch all ather cases */
}

/* -------------------------------------------------------------------- */

int printfunctor0(file,f,tsiz)
     FILE *file;
     functor0 *f;
     int tsiz;
{

  fprintf(file,"{functor: '");
  fdisplay_term(file,FnkName(f),tsiz);
  fprintf(file,"' []}");

  return 1;
}


/* ---------------------------------------------------------------------------
   Build a hashvalue of all features. 
   For atoms:   use atom-id.
   For numbers: use value. 
   Xor them all together...
   Numbers 1...n are ignored when reading_numbers = 1 */

void build_hashvalue(trm,reading_numbers,hval)
     Term trm;
     int *hval,reading_numbers;
{
  if (IsATM(trm)) 
    *hval = (*hval ^  (int)Atm(trm)) % ATOMHASHLEN;
  else 
    if (!reading_numbers)
      *hval = (*hval ^  (int)GetInteger(trm)) % ATOMHASHLEN; 
}
/*----------------------------------------------------------------------
  Choose a nice length of the functor hash table */

int choose_feature_table_size(n)
     int n;
{
  int i=0;
  
  while (n > power_table[i++] && i < POWERTABLELENGTH) 
    {}
  return power_table[i-1];
}
/*----------------------------------------------------------------------
  Hashing when looking for the value of a feature. The 3 below is because
  the structure atom is 8 bytes long. */


int hash_val(trm,length)
     Term trm;
     int  length;
{
  if (IsATM(trm)) {
    return ((long int)(Atm(trm)) >> 3) & (length -1);
  } else {
    return ((long int)(GetInteger(trm))) & (length -1);
  }
}

/*----------------------------------------------------------------------
  Check if a table of feature atoms has the same features as an existing
  feature_table (used during load time) 
  Table contains only hashed features */

bool same_features(f,tbl,features,arity)
     Functor f;
     Term *tbl;
     int  features,arity;
{
  int i;
  Term *names;

  if (FnkFeatures(f) == features && FnkArity(f) == arity) {
    names = FnkNames(f);
    for (i=0;i<features;i++) {
      if (names[i] != (tbl[i])) {
#ifdef FUNCDEBUG
	printf("not same in position %d \n",i);
#endif
	return FALSE;
	}
      }
    return TRUE;
  } else {
    return FALSE;
  }
}
/*---------------------------------------------------------------------
  Store one feature entry in a feature-table-hash-table. If atom already exist
  in the hash-table then return FALSE */

bool insert_feature_in_functor(name,index,table,length,numbers)
     Term name;
     int index,length,numbers;
     fnk_hash_entry *table;
{
    register fnk_hash_entry *hl,*h;
    register int hv;

    if (IsINT(name)) 		
      if ((GetInteger(name) <= numbers) && (GetInteger(name) > 0))
	/* Duplicate number and feature */
	return FALSE;

    hv = hash_val(name,length);
    hl = &table[hv];

    if (hl->index != EMPTY_ENTRY) {
      if (hl->name == name) {
	return FALSE;
      } else {
	h = hl;
	hl = hl->next;
	while(hl != NULL) {
	  if(hl->name == name) {
	    return FALSE;
	  }
	  h = hl;
	  hl = hl->next;
	}
	hl = h;
	/*If the table gets full this loop will never terminate. */
	while(hl->index != EMPTY_ENTRY) {
	  hl--;
	  if (hl < table)
	    hl = (table + length);
	}
	h->next = hl;
      }
    }
    hl->name=name;
    hl->index=index;
    hl->next=NULL;
    return TRUE;
  }

/*--------------------------------------------------------------------------*
  Try to lookup a feature in the functor hash table. If found
  set index to the index of the corresponding value in the structure. */

bool lookup_feature_in_functor(name,table,length,index)
     Term name;
     fnk_hash_entry *table;
     int length,*index;
{
  register fnk_hash_entry *hl;
  register int hv;
  
  hv = hash_val(name,length);
  hl = &table[hv];
  if(hl->name == name) {
    *index = hl->index;
    return TRUE;
  } else {
    hl = hl->next;
    while(hl != NULL) {
      if(hl->name == name) {
	*index = hl->index;
	return TRUE;
      }
      hl = hl->next;
    }
  }
  return FALSE;
}

/*----------------------------------------------------------------------
  Create a functor with features. This function expects the set of features
  to be valid. Feature names are stored as Terms in tbl. 
  Tbl contains only hashed features */

Functor make_feat_functor(name,features,arity,tbl)
     Term name;
     int features,arity;
     Term *tbl;
{
  fnk_hash_entry   *entries;
  Functor              fp;
  int                  i,index,atm_index=0,numbers = arity - features,length;
  Term                 *names;
  
  length = choose_feature_table_size(features);

  NewExtendedFunctor(fp,length,features);

#ifdef FUNCDEBUG
      printf("created %5s/%.2d %.2d %.2d \n",AtmPname(Atm(name)),arity,
	     arity- features,features);
#endif

  FnkName(fp) = Atm(name);
  FnkArity(fp) = arity;

  FnkFeatures(fp) = features;
  FnkLength(fp) = length;
  FnkNums(fp) = arity - features;
  
  entries = FnkHash(fp);
  names = FnkNames(fp);

  /*Initialize feature table */
  for (i = 0;i<length;i++) {
    entries[i].index = EMPTY_ENTRY;
  }
  
  /*Traverse table of feature atoms and:
    - insert a feature into the functor-hash-table
    - Save feature atom in area above functor-hash-table 
    */

  for (index = 0; index < features; index++) {
    if(!insert_feature_in_functor(tbl[index],index+numbers,entries,length,numbers)) {
      return NULL;
    }
    names[atm_index++] = tbl[index];
  }
#ifdef FUNCDEBUG
  print_table(fp);
#endif
  return fp;
}

/*--------------------------------------------------------------------------*
DEBUG FUNCTION
  Lookup a feature value */

#ifdef FUNCDEBUG
bool lookup_and_cnt(atm,table,length,count,lists)
     Term atm;
     fnk_hash_entry *table;
     int length,*count,*lists;
{
  register fnk_hash_entry *hl;
  register int hv;
  
  if (length) {			/* Necessary? */
    hv = hash_val(atm,length);
    hl = &table[hv];
    if(hl->name == atm) {
      return TRUE;
    } else {
      hl = hl->next;
      (*count)++;
      (*lists)++;
      while(hl != NULL) {
	if(hl->name == atm) {
	  return TRUE;
	}
	hl = hl->next;
	(*count)++;
      }
    }
  }
  return FALSE;
}
#endif


/*----------------------------------------------------------------------*/
/*Debug function ...*/
void print_table(f)
     Functor f;
{
#ifdef FUNCDEBUG

  int length = f->length;
  int arity = f->features;
  Term   *atoms;
  int hval;

  int i,empty=0,count=0,lists=0;
  fnk_hash_entry *hl;

  atoms = (Term*)(&(f->entries[length]));  
  for (i=0;i<arity;i++) {
    lookup_and_cnt(atoms[i],f->entries,length,&count,&lists);
  }

  for (i=0;i < length ; i++) {
    hl = &f->entries[i];
#ifdef FUNCDEBUGTABLE
    printf("table[%.3d] ",i);
#endif
    if (hl->index != EMPTY_ENTRY) {
      hval = hash_val(hl->name,length);
#ifdef FUNCDEBUGTABLE
      if (IsATM(hl->name))
	printf("atom '%8s' hv %.3d ind: %.3d ",AtmPname(Atm(hl->name)),hval,hl->index);
      else
	printf("atom '%d' hv %.3d ind: %.3d ",GetInteger(hl->name),hval,hl->index);
#endif
    } else {
#ifdef FUNCDEBUGTABLE
      printf("EMPTY ");
#endif
      empty++;
    }
    if(hl->next != NULL) {
      hl = hl->next;
      hval = hash_val(hl->name,length);
      if(hl != NULL && hval == i) {
#ifdef FUNCDEBUGTABLE
	printf("--> ");
#endif
      } else {
#ifdef FUNCDEBUGTABLE
	printf(" (((");
#endif
      }
      while(hl != NULL) {
#ifdef FUNCDEBUGTABLE
	if (IsATM(hl->name))
	  printf("atom: '%8s' hv %.3d ind: %.3d ",AtmPname(Atm(hl->name)),hval,hl->index);
      else
	printf("atom '%8d' hv %.3d ind: %.3d ",GetInteger(hl->name),hval,hl->index);
#endif
	hl = hl->next;
	if(hl != NULL) {
	  if (i == (hval = hash_val(hl->name,length))) {
#ifdef FUNCDEBUGTABLE
	    printf("--> ");
#endif
	  } else {
#ifdef FUNCDEBUGTABLE
	    printf(" (((");
#endif
	  }
	}
      }
    }
#ifdef FUNCDEBUGTABLE
    printf("\n");
#endif
  }
  printf("-------------------");
#ifdef FUNCDEBUGTABLE
  for (i=0;i<arity;i++) {
    if (IsATM(atoms[i]))
      printf(":'%s' ",AtmPname(Atm(atoms[i])));
    else
      printf(":'%d' ",GetInteger(atoms[i]));
  }
#endif
  printf("\nlength %d arity %d \n",length,arity);
  printf("t{thet %f \n",(float)arity/(float)length);
  printf("average: s|kningar/lista %f \n",(float)count/(float)lists);
  printf("average: s|kningar/uppslag  %f \n",(float)count/(float)arity);
#endif
}



  
