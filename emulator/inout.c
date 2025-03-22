/* $Id: inout.c,v 1.57 1994/05/17 13:49:26 jm Exp $ */ 

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "names.h"
#include "exstate.h"
#include "engine.h"
#include "storage.h"
#include "unify.h"
#include "copy.h"
#include "gc.h"
#include "initial.h"
#include "config.h"
#include "display.h"
#include "foreign.h"
#include "error.h"
#include "inout.h"
#include "ptrhash.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef macintosh
#include <unix.h>
#endif

typedef struct stream {
  struct gvamethod	*method;
  envid 		*env;
  FILE 			*file;
  struct ptrdict	dict;		/* For cycle detection in terms */
} stream;

Gvainfo newstream();
bool 	unifystream();
int 	printstream();
Gvainfo	copystream();
Gvainfo	gcstream();
int     uncopiedstream();
envid	*deallocatestream();
int	killstream();
bool    sendstream();

static gvamethod streammethod = {
  newstream,
  unifystream,
  printstream,
  copystream,
  gcstream,
  uncopiedstream,
  deallocatestream,
  killstream,
  NULL,
  sendstream,
  NULL,
  NULL
};


Term term_eof;
Term term_stream;

Atom atom_fclose;
Atom atom_fflush;
#ifdef X_NON_BLOCK
Atom atom_select;
#endif X_NON_BLOCK

Functor functor_typeof;
Functor functor_fclose_1;
Functor functor_fflush_1;
Functor functor_getc_1;
#ifdef X_NON_BLOCK
Functor functor_getc_nb_1;
#endif X_NON_BLOCK
Functor functor_getc_2;
Functor functor_putc_1;
Functor functor_putc_2;
Functor functor_putd_1;
Functor functor_putd_2;
Functor functor_putd_3;
Functor functor_putf_1;
Functor functor_putf_2;
Functor functor_putf_3;
Functor functor_puts_1;
Functor functor_puts_2;
Functor functor_putsq_2;
Functor functor_putsq_3;
Functor functor_write_generic;
Functor functor_write_constraint;
Functor functor_push_dict;
Functor functor_pop_dict;
Functor functor_lookup_dict;


/*
 * METHOD DEFINITIONS
 *
 */

Gvainfo newstream(old)
     stream *old;
{
  stream *new;
  NEW(new,stream);
  new->dict.count = old->dict.count;
  return (Gvainfo) new;
}

Gvainfo copystream(old,new)
     stream *old, *new;
{
  new->file = old->file;
  new->dict.table = old->dict.table;
  if (new->dict.count != 0) {
    FatalError("copystream: dictionary not reset!");
  }
  return (Gvainfo) new;
}


void gcdict(old,new,gcst)
     stream *old, *new;
     gcstatep gcst;
{
  hash_table *table, *newtbl;
  hash_entry *e;
  long newlimit, n;

  table = old->dict.table;
  e = &table->entry[0];

  n = hash_table_length(table);
  newtbl = new_hash_table(n);
  if (newtbl == NULL) {
    FatalError("Couldn't grow hash table");
  }

  newlimit = newtbl->limit;

  /* Traverse the old table to copy all entries */

  do {
    if (e->key != VACANT_HASH_ENTRY) {
      hash_entry *enew;
      unsigned long hval;
      gc_locations(&e->key,1,gcst);
      hval = HashValue(e->key);
      HashLookup(newtbl, e->key, hval, enew, break);
      enew->key = e->key;
      enew->value = e->value;
      newlimit -= 1;
    }
    e += 1;
    n -= 1;
  } while (n != 0);

  newtbl->limit = newlimit;
  free(table);

  new->dict.table = newtbl;
  return;
}


Gvainfo gcstream(old,new,gcst)
     stream *old, *new;
     gcstatep gcst;
{
  new->file  = old->file;

  gcdict(old,new,gcst);

  return (Gvainfo) new;
}


void externstream()
{
  return;
}


/* deallocatestream() is called by the gc if the generic object
 * is unreferenced after a gc. If the stream has becomed garbage
 * it can be closed immediately. 0 is returned to indicate that
 * no further actions need to be taken by the gc.
 */

envid *deallocatestream(strm)
     stream *strm;
{
  if(strm->file != NULL)
    fclose(strm->file);

  free(strm->dict.table);

  return NULL;
}

/* uncopiedstream() is called by the copier if the stream has not been
 * copied. There is no need to add an extra object on the close list.
 */

int uncopiedstream(strm)
     stream *strm;
{
  return 0;
}


bool unifystream(strm,y,andb,exs)
     Term strm;
     Term y;
     andbox *andb;
     exstate *exs;
{
  if(Eq(strm, y))
    return TRUE;
  return FALSE;
}


int killstream(strm)
     stream *strm;
{
  if(strm->file != NULL)
    fclose(strm->file);

  free(strm->dict.table);

  return 1;
}



int printstream(file,strm,tsiz)
     FILE *file;
     stream *strm;
     int tsiz;
{
  if(strm->file == stdin)
    fprintf(file, "{stream: stdin}");
  else if(strm->file == stdout)
    fprintf(file, "{stream: stdout}");
  else if(strm->file == stderr)
    fprintf(file, "{stream: stderr}");
  else
    fprintf(file, "{stream: %#lx}", (unsigned long)strm);
  return 1;
}


bool sendstream(message, self, exs)
     Term message, self;
     exstate *exs;
{
  bool method_typeof();
  bool method_fclose_0();
  bool method_fclose_1();
  bool method_fflush_0();
  bool method_fflush_1();
  bool method_getc_1();
#ifdef X_NON_BLOCK
  bool method_getc_nb_1();  
#endif X_NON_BLOCK
  bool method_getc_2();
  bool method_putc_1();
  bool method_putc_2();
  bool method_putd_1();
  bool method_putd_2();
  bool method_putd_3();
  bool method_putf_1();
  bool method_putf_2();
  bool method_putf_3();
  bool method_puts_1();
  bool method_puts_2();
  bool method_putsq_2();
  bool method_putsq_3();
  bool method_write_generic();
  bool method_write_constraint();
  bool method_push_dict();
  bool method_pop_dict();
  bool method_lookup_dict();
#ifdef X_NON_BLOCK
  bool method_select_0();
#endif X_NON_BLOCK  
  if(IsATM(message)) {
    Atom op = Atm(message);

    if(op == atom_fclose)
      return method_fclose_0(self, exs);
    else if(op == atom_fflush)
      return method_fflush_0(self, exs);
#ifdef X_NON_BLOCK
    else if(op = atom_select)
      return method_select_0(self,exs);
#endif X_NON_BLOCK
    else
      return FALSE;
  }

  if(IsSTR(message)) {
    Functor op = StrFunctor(Str(message));
    int arity = op->arity;
    Term X0, X1, X2;

    if(arity > 0) {
      GetStrArg(X0, Str(message), 0);
      Deref(X0, X0);
    }
    if(arity > 1) {
      GetStrArg(X1, Str(message), 1);
      Deref(X1, X1);
    }
    if(arity > 2) {
      GetStrArg(X2, Str(message), 2);
      Deref(X2, X2);
    }
    
    if(op == functor_typeof)
      return method_typeof(X0, exs);
    else if(op == functor_fclose_1)
      return method_fclose_1(self, X0, exs);
    else if(op == functor_fflush_1)
      return method_fflush_1(self, X0, exs);
    else if(op == functor_getc_1)
      return method_getc_1(self, X0, exs);
#ifdef X_NON_BLOCK
    else if(op == functor_getc_nb_1)
      return method_getc_nb_1(self, X0, exs);
#endif X_NON_BLOCK
    else if(op == functor_getc_2)
      return method_getc_2(self, X0, X1, exs);
    else if(op == functor_putc_1)
      return method_putc_1(self, X0, exs);
    else if(op == functor_putc_2)
      return method_putc_2(self, X0, X1, exs);
    else if(op == functor_putd_1)
      return method_putd_1(self, X0, exs);
    else if(op == functor_putd_2)
      return method_putd_2(self, X0, X1, exs);
    else if(op == functor_putd_3)
      return method_putd_3(self, X0, X1, X2, exs);
    else if(op == functor_putf_1)
      return method_putf_1(self, X0, exs);
    else if(op == functor_putf_2)
      return method_putf_2(self, X0, X1, exs);
    else if(op == functor_putf_3)
      return method_putf_3(self, X0, X1, X2, exs);
    else if(op == functor_puts_1)
      return method_puts_1(self, X0, exs);
    else if(op == functor_puts_2)
      return method_puts_2(self, X0, X1, exs);
    else if(op == functor_putsq_2)
      return method_putsq_2(self, X0, X1, exs);
    else if(op == functor_putsq_3)
      return method_putsq_3(self, X0, X1, X2, exs);
    else if(op == functor_write_generic)
      return method_write_generic(self, X0, exs);
    else if(op == functor_write_constraint)
      return method_write_constraint(self, X0, exs);
    else if(op == functor_push_dict)
      return method_push_dict(self, X0, exs);
    else if(op == functor_pop_dict)
      return method_pop_dict(self, X0, exs);
    else if(op == functor_lookup_dict)
      return method_lookup_dict(self, X0, X1, exs);
    else
      return FALSE;
  }

  IfVarSuspend(message);
  return FALSE;
}


/*
 * SUPPORT
 *
 */

stream *open_stream(name,mode,exs,andb)
     Term name;
     Term mode;
     exstate *exs;
     andbox *andb;
{
  stream *strm;
  FILE *filep;
  char pathBuf[MAXPATHLEN+1];

  if (IsATM(name)) {
    if(!expand_file_name(AtmPname(Atm(name)),pathBuf))
      return NULL;

    filep = fopen(pathBuf, AtmPname(Atm(mode)));
  } else
    filep = fdopen(GetInteger(name), AtmPname(Atm(mode))); /* file number */
    
  if(filep == NULL)
    return  NULL;

  MakeGvainfo(strm,stream,&streammethod,andb);

  add_gvainfo_to_close((Gvainfo)strm,exs);

  strm->file = filep;
  strm->dict.count = 0;
  strm->dict.table = new_hash_table(DICT_TABLE_SIZE);
  
  return strm;
}


/*
 * MAKING NEW STREAMS
 *
 */

/* fopen(FileName, Mode, Stream) */

bool akl_fopen_3(Arg)
     Argdecl;
{
  Term filename, mode, streamt, tmpv;
  stream *strm;
  
  Deref(filename, A(0));
  Deref(mode,     A(1));
  Deref(streamt,  A(2));

  IfVarSuspend(filename);
  if(!IsATM(filename))
    if(!IsINT(filename))
      DOMAIN_ERROR("fopen", 3, 1, "atom or integer")
    else if(!IsMachineInteger(filename))
      REPRESENTATION_ERROR("fopen", 3, 1, "magnitude exceeds maximum")

  IfVarSuspend(mode);
  if(!IsATM(mode))
    DOMAIN_ERROR("fopen", 3, 2, "atom")

  strm = open_stream(filename, mode, exs, exs->andb);

  if(strm == NULL)
    PERMISSION_ERROR("fopen", 3, "open", "file or file descriptor", filename, "fopen or fdopen fails")

  MakeCvaTerm(tmpv, (Gvainfo)strm);

  return unify(streamt, tmpv, exs->andb, exs);
} 

/* fopen(FileName, Mode, Stream, Result) */

bool akl_fopen_4(Arg)
     Argdecl;
{
  Term filename, mode, streamt, result, tmps, tmpr;
  stream *strm;
  
  Deref(filename, A(0));
  Deref(mode,     A(1));
  Deref(streamt,  A(2));
  Deref(result,   A(3));

  IfVarSuspend(filename);
  if(!IsATM(filename))
    if(!IsINT(filename))
      DOMAIN_ERROR("fopen", 4, 1, "atom or integer")
    else if(!IsMachineInteger(filename))
      REPRESENTATION_ERROR("fopen", 4, 1, "magnitude exceeds maximum")

  IfVarSuspend(mode);
  if(!IsATM(mode))
    DOMAIN_ERROR("fopen", 4, 2, "atom")

  strm = open_stream(filename, mode, exs, exs->andb);

  if(strm == NULL) {
    EXCEPTION(tmpr, ERRNO);
    return unify(result, tmpr, exs->andb, exs);
  } else {
    MakeCvaTerm(tmps, (Gvainfo)strm);
    return unify(streamt, tmps, exs->andb, exs) &&
           unify(result, term_true, exs->andb, exs);
  }
} 

/* stdin(StdIn) */

bool akl_stdin(Arg)
     Argdecl;
{
  Term streamt, tmpv;
  stream *strm;

  Deref(streamt, A(0));

  MakeGvainfo(strm, stream, &streammethod, exs->andb);
  strm->file = stdin;
  strm->dict.count = 0;
  strm->dict.table = new_hash_table(DICT_TABLE_SIZE);

  MakeCvaTerm(tmpv, (Gvainfo)strm);
  return unify(streamt, tmpv, exs->andb, exs);
}


/* stdout(StdOut) */

bool akl_stdout(Arg)
     Argdecl;
{
  Term streamt, tmpv;
  stream *strm;
  
  Deref(streamt, A(0));

  MakeGvainfo(strm, stream, &streammethod, exs->andb);
  strm->file = stdout;
  strm->dict.count = 0;
  strm->dict.table = new_hash_table(DICT_TABLE_SIZE);

  MakeCvaTerm(tmpv, (Gvainfo)strm);
  return unify(streamt, tmpv, exs->andb, exs);
}


/* stderr(StdErr) */

bool akl_stderr(Arg)
     Argdecl;
{
  Term streamt, tmpv;
  stream *strm;
  
  Deref(streamt, A(0));

  MakeGvainfo(strm, stream, &streammethod, exs->andb);
  strm->file = stderr;
  strm->dict.count = 0;
  strm->dict.table = new_hash_table(DICT_TABLE_SIZE);

  MakeCvaTerm(tmpv, (Gvainfo)strm);
  return unify(streamt, tmpv, exs->andb, exs);
}


/*
 * METHODS
 *
 */


/* typeof(Type)@Stream */

bool method_typeof(type, exs)
     Term type;
     exstate *exs;     
{
  return unify(type, term_stream, exs->andb, exs);
}


/* fclose@Stream */

bool method_fclose_0(self, exs)
     Term self;
     exstate *exs;     
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[1]; /* dummy */

  if(fclose(streamself->file) == EOF)
    METHOD_PERMISSION_ERROR("fclose", 0, "close", "stream", self, "fclose fails", self)
  else {
    streamself->file = NULL;
    return TRUE;
  }
}  


/* fclose(Result)@Stream */

bool method_fclose_1(self, result, exs)
     Term self, result;
     exstate *exs;     
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp;

  if(fclose(streamself->file) == EOF)
    EXCEPTION(tmp, ERRNO)
  else {
    streamself->file = NULL;
    tmp = term_true;
  }

  return unify(result, tmp, exs->andb, exs);
}  


/* fflush@Stream */

bool method_fflush_0(self, exs)
     Term self;
     exstate *exs;     
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[1]; /* dummy */

  if(fflush(streamself->file) == EOF)
    METHOD_PERMISSION_ERROR("fflush", 0, "flush", "stream", self, "fflush fails", self)
  else {
    reset_dict(&streamself->dict);
    return TRUE;
  }
}


/* fflush(Result)@Stream */

bool method_fflush_1(self, result, exs)
     Term self, result;
     exstate *exs;     
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp;

  if(fflush(streamself->file) == EOF)
    EXCEPTION(tmp, ERRNO)
  else {
    reset_dict(&streamself->dict);
    tmp = term_true;
  }

  return unify(result, tmp, exs->andb, exs);
}


/* getc(Char)@Stream */

bool method_getc_1(self, ch, exs)
     Term self, ch;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  int i;
  Term tmp, A[1];
  
  A[0] = ch;

  if (feof(streamself->file))
    METHOD_PERMISSION_ERROR("getc", 1, "get", "character", NullTerm, "stream at EOF", self)
  else {
    i =  getc(streamself->file);
    if(i == EOF)
      if (feof(streamself->file)) {
	if (isatty(fileno(streamself->file)))
	  clearerr(streamself->file);
	MakeIntegerTerm(tmp, -1);
	return unify(ch, tmp, exs->andb, exs);
      } else
	METHOD_PERMISSION_ERROR("getc", 1, "get", "character", NullTerm, "getc fails", self)
    else {
      MakeIntegerTerm(tmp, i);
      return unify(ch, tmp, exs->andb, exs);
    }
  }
}


/* getc(Char,Result)@Stream */

bool method_getc_2(self, ch, result, exs)
     Term self, ch, result;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  int i;
  Term tmp;
  
  if (feof(streamself->file)) {
    EXCEPTION(tmp, term_eof)
    return unify(result, tmp, exs->andb, exs);
  } else {
    i =  getc(streamself->file);
    if(i == EOF)
      if (feof(streamself->file)) {
	if (isatty(fileno(streamself->file)))
	  clearerr(streamself->file);
	MakeIntegerTerm(tmp, -1);
	return unify(ch, tmp, exs->andb, exs) &&
	       unify(result, term_true, exs->andb, exs);
      } else {
	EXCEPTION(tmp, ERRNO)
	return unify(result, tmp, exs->andb, exs);
      }
    else {
      MakeIntegerTerm(tmp, i);
      return unify(ch, tmp, exs->andb, exs) &&
	     unify(result, term_true, exs->andb, exs);
    }
  }
}



/* putc(Char)@Stream */

bool method_putc_1(self, ch, exs)
     Term self, ch;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp, A[1];
  
  A[0] = ch;

  IfVarSuspend(ch);

  if(!IsINT(ch))
    METHOD_DOMAIN_ERROR("putc", 1, 1, "integer", self)
  else if(!IsMachineInteger(ch))
    METHOD_REPRESENTATION_ERROR("putc", 1, 1, "magnitude exceeds maximum", self)

  if (putc((char)GetInteger(ch), streamself->file) == EOF)
    METHOD_PERMISSION_ERROR("putc", 1, "put", "character", NullTerm, "putc fails", self)
  else
    return TRUE;
}


/* putc(Char,Result)@Stream */

bool method_putc_2(self, ch, result, exs)
     Term self, ch, result;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp, A[2];
  
  A[0] = ch;
  A[1] = result;

  IfVarSuspend(ch);

  if(!IsINT(ch))
    METHOD_DOMAIN_ERROR("putc", 2, 1, "integer", self)
  else if(!IsMachineInteger(ch))
    METHOD_REPRESENTATION_ERROR("putc", 2, 1, "magnitude exceeds maximum", self)

  if(putc((char)GetInteger(ch), streamself->file) == EOF)
    EXCEPTION(tmp, ERRNO)
  else
    tmp = term_true;
  
  return unify(result, tmp, exs->andb, exs);
}


static int print_integer(f, i)
     FILE *f;
     Term i;
{
  if(IsNUM(i))
    return fprintf(f, "%ld", GetSmall(i));
  else if(IsBIG(i))
    return bignum_print(f, Big(i));
}


/* putd(Integer)@Stream */

bool method_putd_1(self, i, exs)
     Term self, i;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[1];

  A[0] = i;

  IfVarSuspend(i);

  if(!IsINT(i))
    METHOD_DOMAIN_ERROR("putd", 1, 1, "integer", self)

  if(print_integer(streamself->file, i) == EOF)
    METHOD_PERMISSION_ERROR("putd", 1, "put", "integer", NullTerm, "putc fails", self)
  else
    return TRUE;
}


/* putd(Integer,Result)@Stream */

bool method_putd_2(self, i, result, exs)
     Term self, i, result;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp, A[2];

  A[0] = i;
  A[1] = result;

  IfVarSuspend(i);

  if(!IsINT(i))
    METHOD_DOMAIN_ERROR("putd", 2, 1, "integer", self)

  if(print_integer(streamself->file, i) == EOF)
    EXCEPTION(tmp, ERRNO)
  else
    tmp = term_true;

  return unify(result, tmp, exs->andb, exs);
}


/* putd(D, Format, Arg)@Stream (called from format/[1-4]) */

bool method_putd_3(self, d, format, arg, exs)
     Term self, d, format, arg;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[3];
  char formatChar, *buf, temp[MAXNUMLEN];
  long size;
  int do_free = 0;
  int argValue, signed_radix;
  int radix = 10, precision = 0;

  extern void itoa();

  A[0] = d;
  A[1] = format;
  A[2] = arg;

  IfVarSuspend(d);
  IfVarSuspend(format);
  IfVarSuspend(arg);

  if(!IsINT(d))
    METHOD_DOMAIN_ERROR("putd", 3, 1, "integer", self)
  if(!IsINT(format))
    METHOD_DOMAIN_ERROR("putd", 3, 2, "integer", self)
  else if(!IsMachineInteger(format))
    METHOD_REPRESENTATION_ERROR("putd", 3, 2, "magnitude exceeds maximum", self)
  if(!IsINT(arg))
    METHOD_DOMAIN_ERROR("putd", 3, 3, "integer", self)
  else if(!IsMachineInteger(arg))
    METHOD_REPRESENTATION_ERROR("putd", 3, 3, "magnitude exceeds maximum", self)

  argValue = GetInteger(arg);
  formatChar = GetInteger(format);
  if(formatChar == 'r') {
    radix = argValue;
    signed_radix = radix;
  } else if(formatChar == 'R') {
    radix = argValue;
    signed_radix = -radix;
  } else {
    precision = argValue;
    signed_radix = radix;
  }

  size = integer_sizeinbase(d, radix) + 100;

  if (size > MAXNUMLEN) {
    buf = salloc(size);
    do_free = 1;
  } else {
    buf = temp;
    size = MAXNUMLEN;
  }

  term_itoa(d, signed_radix, buf);

  if(precision > 0) {
    int usilen = strlen(buf) - (buf[0]=='-');
    int n = precision-usilen+1;
    
    if(n>0) {
      int i;
      int dig1 = (buf[0] == '-');
      int slen = strlen(buf);
      
      if(slen+n+1 > size) {
	if (do_free)
	  free(buf);
	METHOD_REPRESENTATION_ERROR("putd", 3, 3, "magnitude exceeds maximum", self)
      }
      
      for(i=slen; i>=dig1; i--)
	buf[i+n] = buf[i];
      for(i=dig1+n-1; i>=dig1; i--)
	buf[i] = '0';
    }
    
    {
      int i;
      int slen = strlen(buf);
      int ppos = slen-precision;
      
      if(slen+2 > size) {
	if (do_free)
	  free(buf);
	METHOD_REPRESENTATION_ERROR("putd", 3, 3, "magnitude exceeds maximum", self)
      }
      
      for(i=slen; i>=ppos; i--)
	buf[i+1] = buf[i];
      buf[ppos] = '.';
    }
  }
  if(formatChar=='D') {
    int i, count;
    int slen = strlen(buf);
    int dig1 = (buf[0]=='-');
    int ppos = slen;
    
    for(i=dig1, count=0; i<ppos; i++) {
      if(buf[i]=='.') ppos=i;
      else count++;
    }
    count = (count-1)/3;
    
    if(count>0) {
      if(slen+count+1 > size) {
	if (do_free)
	  free(buf);
	METHOD_REPRESENTATION_ERROR("putd", 3, 3, "magnitude exceeds maximum", self)
      }
      
      for(i=slen; i>=ppos; i--)
	buf[i+count] = buf[i];
      for(i=ppos-1; count>0; count--)
	buf[i+count] = buf[i], i--,
	buf[i+count] = buf[i], i--,
	buf[i+count] = buf[i], i--,
	buf[i+count] = ',';
    }
  }

  if (fprintf(streamself->file, "%s", buf) == EOF) {
    if (do_free)
      free(buf);
    METHOD_PERMISSION_ERROR("putd", 3, "put", "integer", NullTerm, "putc fails", self)
  } else {
    if (do_free)
      free(buf);
    return TRUE;
  }
}


/* putf(Float)@Stream */

bool method_putf_1(self, flt, exs)
     Term self, flt;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[1];
  char temp[MAXNUMLEN];

  extern void ftoa();

  A[0] = flt;

  IfVarSuspend(flt);

  if(!IsFLT(flt))
    METHOD_DOMAIN_ERROR("putf", 1, 1, "float", self)

  ftoa(FltVal(Flt(flt)), temp);
  if(fprintf(streamself->file, "%s", temp) == EOF)
    METHOD_PERMISSION_ERROR("putf", 1, "put", "float", NullTerm, "putc fails", self)
  else
    return TRUE;
}


/* putf(Float,Result)@Stream */

bool method_putf_2(self, flt, result, exs)
     Term self, flt, result;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp, A[2];
  char temp[MAXNUMLEN];

  extern void ftoa();

  A[0] = flt;
  A[1] = result;

  IfVarSuspend(flt);

  if(!IsFLT(flt))
    METHOD_DOMAIN_ERROR("putf", 2, 1, "float", self)

  ftoa(FltVal(Flt(flt)), temp);
  if(fprintf(streamself->file, "%s", temp) == EOF)
    EXCEPTION(tmp, ERRNO)
  else
    tmp = term_true;
  
  return unify(result, tmp, exs->andb, exs);
}


/* putf(F, Format, Arg)@Stream (called from format/[1-4]) */

bool method_putf_3(self, f, format, arg, exs)
     Term self, f, format, arg;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[3];
  char formatChar, formatString[10], temp[MAXNUMLEN];
  int precision;

  A[0] = f;
  A[1] = format;
  A[2] = arg;

  IfVarSuspend(f);
  IfVarSuspend(format);
  IfVarSuspend(arg);

  if(!IsFLT(f))
    METHOD_DOMAIN_ERROR("putf", 3, 1, "float", self)
  if(!IsINT(format))
    METHOD_DOMAIN_ERROR("putf", 3, 2, "integer", self)
  else if(!IsMachineInteger(format))
    METHOD_REPRESENTATION_ERROR("putf", 3, 2, "magnitude exceeds maximum", self)
  if(!IsINT(arg))
    METHOD_DOMAIN_ERROR("putf", 3, 3, "integer", self)
  else if(!IsMachineInteger(arg))
    METHOD_REPRESENTATION_ERROR("putf", 3, 3, "magnitude exceeds maximum", self)

  formatChar = GetInteger(format);
  precision = GetInteger(arg);
  if (precision >= 0)
    sprintf(formatString, "%%.%d%c", precision, formatChar);
  else
    sprintf(formatString, "%%%c", formatChar);

  sprintf(temp, formatString, FltVal(Flt(f)));

  if (fprintf(streamself->file, "%s", temp) == EOF)
    METHOD_PERMISSION_ERROR("putf", 3, "put", "float", NullTerm, "putc fails", self)
  else
    return TRUE;
}


/* puts(Atom)@Stream */

bool method_puts_1(self, string, exs)
     Term self, string;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[1];

  A[0] = string;

  IfVarSuspend(string);

  if(!IsATM(string))
    METHOD_DOMAIN_ERROR("puts", 1, 1, "atom", self)

  if(fprintf(streamself->file, "%s", AtmPname(Atm(string))) == EOF)
    METHOD_PERMISSION_ERROR("puts", 1, "put", "string", NullTerm, "putc fails", self)
  else
    return TRUE;
}


/* puts(Atom,Result)@Stream */

bool method_puts_2(self, string, result, exs)
     Term self, string, result;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term tmp, A[2];

  A[0] = string;
  A[1] = result;

  IfVarSuspend(string);

  if(!IsATM(string))
    METHOD_DOMAIN_ERROR("puts", 2, 1, "atom", self)

  if(fprintf(streamself->file, "%s", AtmPname(Atm(string))) == EOF)
    EXCEPTION(tmp, ERRNO)
  else
    tmp = term_true;
  
  return unify(result, tmp, exs->andb, exs);
}


/* putsq(Atom,QuoteChar)@Stream */

bool method_putsq_2(self, string, ch, exs)
     Term self, string, ch;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[2], tmp;
  register char *p;
  register char c, cq;
  register FILE *f = streamself->file;

  A[0] = string;
  A[1] = ch;

  IfVarSuspend(string);
  IfVarSuspend(ch);

  if(!IsATM(string))
    METHOD_DOMAIN_ERROR("putsq", 2, 1, "atom", self)

  if(!IsINT(ch))
    METHOD_DOMAIN_ERROR("putsq", 2, 2, "integer", self)
  else if(!IsMachineInteger(ch))
    METHOD_REPRESENTATION_ERROR("putsq", 2, 2, "magnitude exceeds maximum", self)

  p = AtmPname(Atm(string));
  cq = GetInteger(ch);

  tmp = term_true; /* tmp handling here is clumsy but correct. */
  if(putc(cq, f) == EOF)
    EXCEPTION(tmp, ERRNO)
  while((c = *(p++))) {
    if(c == cq)
      if(putc(c, f) == EOF)
	EXCEPTION(tmp, ERRNO)
    if(putc(c, f) == EOF)
      EXCEPTION(tmp, ERRNO)
  }
  if(putc(cq, f) == EOF)
    EXCEPTION(tmp, ERRNO)

  if (tmp != term_true)
    METHOD_PERMISSION_ERROR("putsq", 2, "put", "string", NullTerm, "putc fails", self)
  else
    return TRUE;
}


/* putsq(Atom,QuoteChar,Result)@Stream */

bool method_putsq_3(self, string, ch, result, exs)
     Term self, string, ch, result;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[3], tmp;
  register char *p;
  register char c, cq;
  register FILE *f = streamself->file;

  A[0] = string;
  A[1] = ch;
  A[2] = result;

  IfVarSuspend(string);
  IfVarSuspend(ch);

  if(!IsATM(string))
    METHOD_DOMAIN_ERROR("putsq", 3, 1, "atom", self)

  if(!IsINT(ch))
    METHOD_DOMAIN_ERROR("putsq", 3, 2, "integer", self)
  else if(!IsMachineInteger(ch))
    METHOD_REPRESENTATION_ERROR("putsq", 3, 2, "magnitude exceeds maximum", self)

  p = AtmPname(Atm(string));
  cq = GetInteger(ch);

  tmp = term_true; /* tmp handling here is clumsy but correct. */
  if(putc(cq, f) == EOF)
    EXCEPTION(tmp, ERRNO)
  while((c = *(p++))) {
    if(c == cq)
      if(putc(c, f) == EOF)
	EXCEPTION(tmp, ERRNO)
    if(putc(c, f) == EOF)
      EXCEPTION(tmp, ERRNO)
  }
  if(putc(cq, f) == EOF)
    EXCEPTION(tmp, ERRNO)

  return unify(result, tmp, exs->andb, exs);
}


/*
 * WRITING TERMS
 *
 */


/* write_generic(Gen)@Stream */

bool method_write_generic(self, x, exs)
     Term self, x;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  IfVarSuspend(x);
  if(!IsGEN(x))
    return FALSE;
  push_dict(&streamself->dict,x);
  Gen(x)->method->print(streamself->file, Gen(x), -1, &streamself->dict);
  pop_dict(&streamself->dict,x);
  return TRUE;
}


/* write_constraint(Var)@Stream */

bool method_write_constraint(self, x, exs)
     Term self, x;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  if(IsCvaTerm(x)) {
    push_dict(&streamself->dict,x);
    RefCvaMethod(Ref(x))->print(streamself->file, RefGva(Ref(x)), -1, &streamself->dict);
    pop_dict(&streamself->dict,x);
  }

  if(!IsVar(x)) {
    WARNING("Constrained variable became determined during printing!");
  }

  return TRUE;
}


/*
 * Methods for use by cycle detection
 *
 */


/* '$push_dict'(X)@Stream */

bool method_push_dict(self, x, exs)
     Term self, x;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));

  return push_dict(&streamself->dict, x);
}


/* '$pop_dict'(X)@Stream */

bool method_pop_dict(self, x, exs)
     Term self, x;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));

  return pop_dict(&streamself->dict, x);
}


/* '$lookup_dict'(X,Dist)@Stream */

bool method_lookup_dict(self, x, y, exs)
     Term self, x, y;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  long dist;
  Term result;

  dist = lookup_dict(&streamself->dict, x);
  MakeIntegerTerm(result, dist);
  return unify(y, result, exs->andb, exs);
}

#ifdef X_NON_BLOCK

bool method_select_0(self, exs)
     Term self;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  Term A[1]; /* dummy */
  
  if (feof(streamself->file))
    METHOD_PERMISSION_ERROR("select", 0, "select", "stream", self, "stream at EOF", self)
  else {
    int nfds;
    fd_set readfds;
    struct timeval tv;
    

    FD_ZERO(&readfds);
    FD_SET(fileno(streamself->file) , &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    nfds = select(1, &readfds, NULL, NULL, &tv);

    if(nfds == 0) {
      return REDO;
    }
    return TRUE;
  }
}

/* getc_nb(Char)@Stream */

bool method_getc_nb_1(self, ch, exs)
     Term self, ch;
     exstate *exs;
{
  stream *streamself = (stream *)(RefGva(Ref(self)));
  int i;
  Term tmp, A[1];
  
  A[0] = ch;

  if (feof(streamself->file))
    METHOD_PERMISSION_ERROR("getc_nb", 1, "get", "character", NullTerm, "stream at EOF", self)
  else {
    int j;
    ioctl(fileno(streamself->file) , FIONREAD, &j);

    if(j == 0) {
      return REDO;
    }
    i =  getc(streamself->file);
    if(i == EOF)
      if (feof(streamself->file)) {
	if (isatty(fileno(streamself->file)))
	  clearerr(streamself->file);
	MakeIntegerTerm(tmp, -1);
	return unify(ch, tmp, exs->andb, exs);
      } else
	METHOD_PERMISSION_ERROR("getc_nb", 1, "get", "character", NullTerm, "getc fails", self)
    else {
      MakeIntegerTerm(tmp, i);
      return unify(ch, tmp, exs->andb, exs);
    }
  }
}

#endif X_NON_BLOCK


/*
 * Procedures for cycle detection
 *
 */

void reset_dict(dict)
     struct ptrdict *dict;
{
  /* Reset the dictionary */
  dict->count = 0;
  if (hash_table_length(dict->table) > DICT_TABLE_SIZE) {
    free(dict->table);
    dict->table = new_hash_table(DICT_TABLE_SIZE);
  } else {
    clear_hash_table(dict->table);
  }
}

bool push_dict(dict, x)
     struct ptrdict *dict;
     Term x;
{
  unsigned long hval;
  hash_entry *eptr;

  dict->count += 1;

  hval = TermHashValue(x);
  TermHashLookup(dict->table, x, hval, eptr, goto not_found);

  /* If already in table, just update with the new counter value */
  eptr->value = dict->count;
  return TRUE;

 not_found:

  TermHashEnter(dict->table, eptr, x, tad_to_term(dict->count));
  return TRUE;
}

bool pop_dict(dict, x)
     struct ptrdict *dict;
     Term x;
{
  unsigned long hval;
  hash_entry *eptr;

  hval = TermHashValue(x);
  TermHashLookup(dict->table, x, hval, eptr, goto not_found);

  dict->count -= 1;

  if (dict->count == 0)
    reset_dict(dict);	/* Trim the dict when we get to the bottom */
  else
    eptr->value = -1;	/* We really should remove it from table instead */

  return TRUE;

 not_found:
  Error("problem with writing: $pop_dict argument not in table!");

  return FALSE;
}

long lookup_dict(dict, x)
     struct ptrdict *dict;
     Term x;
{
  unsigned long hval;
  long si;
  hash_entry *eptr;
  long dist;

  hval = TermHashValue(x);
  TermHashLookup(dict->table, x, hval, eptr, goto not_found);

  si = eptr->value;
  if (si < 0)
    dist = 0;		/* This is no longer visible */
  else {
    dist = dict->count - eptr->value + 1;
  }
  return dist;

 not_found:
  return 0;
}


/*
 * LOW-LEVEL
 *
 */

/* $display(Term) */

bool akl_display(Arg)
     Argdecl;
{
  display_term(A(0),-1);
  return TRUE;
}



void initialize_inout() {
  
  term_eof = TagAtm(store_atom("eof"));
  term_stream = TagAtm(store_atom("stream"));

  atom_fclose = store_atom("fclose");
  atom_fflush = store_atom("fflush");
#ifdef X_NON_BLOCK
  atom_select = store_atom("select");
#endif X_NON_BLOCK  
  functor_typeof = store_functor(store_atom("typeof"),1);
  functor_fclose_1 = store_functor(store_atom("fclose"),1);
  functor_fflush_1 = store_functor(store_atom("fflush"),1);
  functor_getc_1 = store_functor(store_atom("getc"),1);
#ifdef X_NON_BLOCK
  functor_getc_nb_1 = store_functor(store_atom("getc_nb"),1);  
#endif X_NON_BLOCK
  functor_getc_2 = store_functor(store_atom("getc"),2);
  functor_putc_1 = store_functor(store_atom("putc"),1);
  functor_putc_2 = store_functor(store_atom("putc"),2);
  functor_putd_1 = store_functor(store_atom("putd"),1);
  functor_putd_2 = store_functor(store_atom("putd"),2);
  functor_putd_3 = store_functor(store_atom("putd"),3);
  functor_putf_1 = store_functor(store_atom("putf"),1);
  functor_putf_2 = store_functor(store_atom("putf"),2);
  functor_putf_3 = store_functor(store_atom("putf"),3);
  functor_puts_1 = store_functor(store_atom("puts"),1);
  functor_puts_2 = store_functor(store_atom("puts"),2);
  functor_putsq_2 = store_functor(store_atom("putsq"),2);
  functor_putsq_3 = store_functor(store_atom("putsq"),3);
  functor_write_generic = store_functor(store_atom("write_generic"),1);
  functor_write_constraint = store_functor(store_atom("write_constraint"),1);
  functor_push_dict = store_functor(store_atom("$push_dict"),1);
  functor_pop_dict = store_functor(store_atom("$pop_dict"),1);
  functor_lookup_dict = store_functor(store_atom("$lookup_dict"),2);

  define("$display",akl_display,1);

  define("fopen", akl_fopen_3, 3);
  define("fopen", akl_fopen_4, 4);

  define("stdout", akl_stdout, 1);
  define("stdin", akl_stdin, 1);
  define("stderr", akl_stderr, 1);
}
