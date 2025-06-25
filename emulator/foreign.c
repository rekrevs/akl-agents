/* $Id: foreign.c,v 1.29 1994/05/17 13:54:35 bd Exp $ */
/* Copyright (C) 1988, Swedish Institute of Computer Science. */
/* Support for various operating system oriented predicates */
/* Adapted from ISP foreign.c, no foreign function interface yet... */

#include "include.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "unify.h"
#include "initial.h"
#include "foreign.h"
#include "storage.h"
#include "config.h"
#include "names.h"
#include "error.h"

#ifdef unix
#include <pwd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/stat.h>
#endif

#include <string.h>

#ifdef unix
#include <unistd.h>
#else
#ifdef macintosh
#include <unix.h>
#endif
#endif

#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#define MAXATOM 4711 /* note: should not be defined here, really */

#define F_OK 0

/* These macros are modified for the AKL unifier */

#define Unify_constant(X,Y) {if(!unify((Term)(X),(Term)(Y), exs->andb,exs)) \
				 return FALSE;}


#define cunify(A, X, Y) unify(X,Y,exs->andb,exs)	/* dangerous */


#define MakeString(X) TagAtm(store_atom(X))

#define MakeLST(to, car, cdr)\
{\
  register Term tmp; \
  MakeListTerm(tmp,exs->andb); \
  LstCar(Lst(tmp)) = (car); \
  LstCdr(Lst(tmp)) = (cdr); \
  (to) = tmp; \
}

extern char *mktemp PROTO((char *name));
extern int chdir PROTO((char *dir));
extern int system PROTO((char *call));
extern int access PROTO((char *path, int mode));
extern char *getwd PROTO((char *pathname));

static char cwd[MAXPATHLEN+1];


/* Support for unix(+Command)
 * atom or structure Command
 *
 * Description: interface to the operating system.
 * unix(cd(+Path))	change working directory to Path
 * unix(cd)		change working directory to $HOME
 * unix(shell)		invoke new interactive $SHELL
 * unix(shell(+Command)) pass Command to a new $SHELL, returning 0
 * unix(shell(+Command,-Status)) pass Command to a new $SHELL, returning Status
 * unix(system(+Command)) pass the string Command to the UNIX procedure
 *			system, i.e. to sh(1), returning 0.
 * unix(system(+Command,-Status)) pass the string Command to the UNIX procedure
 *			system, i.e. to sh(1), returning Status.
 * unix(argv(-Args))	return the arguments with which the current
 *			akl was invoked.
 * unix(exit(+Status))	terminate the Prolog process with Status
 * unix(mktemp(+Template,-FileName)) interface to mktemp(3):
 *                                   FileName is unified with name of file
 * unix(access(+Path,-Mode)) Mode is the accessability of Path
 * unix(chmod(Path,Old,New)) Old is the current access mode of Path;
 *                           New is the new access mode
 * unix(umask(Old,New)) Old is the current file mode creation mask;
 *                      New is the new file mode creation mask
 * break_to_gdb		trap to GDB or other C debugger
 *
 * Prolog has checked that the input arguments are valid.
 */

#ifdef unix

bool akl_unix_cd(Arg)
     Argdecl;
{
  char pathBuf[MAXPATHLEN+1];
  Term X1;

  Unify_constant(MakeString(cwd),A(0));
  
  Deref(X1,A(1));
  if (!expand_file_name(AtmPname(Atm(X1)),pathBuf))
    return FALSE;
  if (chdir(pathBuf))
    return FALSE;
  compute_cwd();
  return TRUE;
}

bool akl_unix_shell0(Arg)
     Argdecl;
{
  char cbuf[MAXPATHLEN+10];

  extern char *getenv PROTO((char *));

  strcpy(cbuf,"exec ");
  strcat(cbuf,getenv("SHELL"));
  return !system(cbuf);
}



bool akl_unix_shell2(Arg)
     Argdecl;
{
  register char *p1, *p2;
  char cbuf[2*MAXATOM+MAXPATHLEN+10];
  Term X0;
  Term cbi;

  extern char *getenv();

  Deref(X0,A(0));
  strcpy(cbuf,"exec ");
  strcat(cbuf,getenv("SHELL"));
  strcat(cbuf," -c ");
  p1 = cbuf+strlen(cbuf);
  for(p2=AtmPname(Atm(X0)); *p2;)
    *p1++ = '\\',
    *p1++ = *p2++;
  *p1++ = 0;
  MakeIntegerTerm(cbi, (system(cbuf) >> 8 ));
  return cunify(Arg, cbi, A(1));
}


bool akl_unix_system2(Arg)
     Argdecl;
{
  Term X0;
  Term i;

  Deref(X0,A(0));

  MakeIntegerTerm(i, (system(AtmPname(Atm(X0))) >> 8 ));
  return cunify(Arg, i, A(1));
}


bool akl_unix_argv(Arg)
     Argdecl;
{
  extern int akl_argc;
  extern char **akl_argv;
  

  register Term lst = NIL;
  register char **p1 = akl_argv;
  register int i;
  
  for (i=akl_argc; i>0;)
    {
      MakeLST(lst, MakeString(p1[--i]), lst);
    }
  return cunify(Arg,lst,A(0));
}

bool akl_unix_exit(Arg)
     Argdecl;
{
  Term X0;

  Deref(X0,A(0));

  exit(GetInteger(X0));
  return TRUE;
}

bool akl_unix_mktemp(Arg)
     Argdecl;
{
  char template[MAXATOM+1];
  Term X0;

  extern char *mktemp();
  
  Deref(X0,A(0));
  strcpy(template,AtmPname(Atm(X0)));
  mktemp(template);
  return cunify(Arg,MakeString(template),A(1));
}

bool akl_unix_access(Arg)
     Argdecl;
{
  char pathBuf[MAXPATHLEN+1];
  Term X0, X1;

  Deref(X0,A(0));
  if (!expand_file_name(AtmPname(Atm(X0)),pathBuf))
    return FALSE;
  Deref(X1,A(1));
  if (access(pathBuf,GetSmall(X1)))
    return FALSE;
  return TRUE;
}

bool akl_unix_chmod(Arg)
     Argdecl;
{
  struct stat statbuf;
  char pathBuf[MAXPATHLEN+1];
  Term X0, X2;
  Term mi;

  Deref(X0, A(0));
  if (!expand_file_name(AtmPname(Atm(X0)),pathBuf))
    return FALSE;
  if (stat(pathBuf, &statbuf))
    return FALSE;
  MakeIntegerTerm(mi, statbuf.st_mode&0xfff);
  Unify_constant(mi, A(1));

  Deref(X2, A(2));
  if (chmod(pathBuf, GetSmall(X2)))
    return FALSE;

  return TRUE;
}

bool akl_unix_umask(Arg)
     Argdecl;
{
  int i;
  Term X1;
  Term j;
  
  Deref(X1,A(1));

  if (IsVar(X1))
    {
      i = umask(0);
      (void)umask(i);
      MakeIntegerTerm(j, i);
      return cunify(Arg, j, A(0));
    }
  else {
    MakeIntegerTerm(j, umask(GetSmall(A(1))));
    return cunify(Arg, j, A(0));
  }
}


bool akl_unix_modified(Arg)
     Argdecl;
{
  struct stat statbuf;
  Term X0;
  Term stm;

  Deref(X0, A(0));
  if (stat(AtmPname(Atm(X0)), &statbuf))
    return FALSE;
  MakeIntegerTerm(stm, statbuf.st_mtime);
  Unify_constant(stm, A(1));  
  return TRUE;
}

#else

#if !defined(macintosh)
int access(pathname, flag)
    char *pathname;
    int flag; /* ignore this */
{
   FILE *fp = fopen(pathname, "r");
   if (fp != NULL) {
     fclose(fp);
     return 0;
   } else {
     return -1;
   }
}
#endif

#endif

/* $find_file(+LibDir,+Path,+Suffix,-Found,-AbsolutePath,-AbsoluteDir)
 * string LibDir	a library in which to search for path
 * string Path		a path, may be absolute or relative. If LibDir
 *			is specified then path must be relative to LibDir.
 * string Suffix        an optional suffix to Path
 * atom   Found         true or fail
 * string AbsolutePath  the absolute pathname of Path
 * string AbsoluteDir   the absolute pathname of Path/..
 *
 * Description: if LibDir is specified: try to find Path in LibDir. If Path
 * is a basename then first search for PathSuffix. If no file is found then
 * return FALSE.
 * If LibDir is not specified: if Path is a relative pathname then prefix
 * with current directory. If Path is a basename then first try appending
 * Suffix. If no file is found return the absolute path name for Path.
 */

bool akl_find_file(Arg)
     Argdecl;
{
    /* extern char *rindex(); */
    
  char *libDir, *path, *suffix;
  char pathBuf[MAXPATHLEN+8];
  char relBuf[2*MAXATOM+2];
  register char /* *cpSlash, */ *cp;
  Term X0, X1, X2;

  Deref(X0,A(0));
  libDir = AtmPname(Atm(X0));

  Deref(X1,A(1));
  path = AtmPname(Atm(X1));

  Deref(X2,A(2));
  suffix = AtmPname(Atm(X2));
  
  strcpy(relBuf,libDir);
  if (relBuf[strlen(relBuf)-1] != '/') strcat(relBuf,"/");
  strcat(relBuf,path);
  if (!expand_file_name(relBuf,pathBuf))
    return FALSE;
  

  /* I commented out this code since it discriminates against people
     who like to name files 'foo.bar.pl' etc. --Mats
     
     if((cpSlash = rindex(pathBuf,'/')) && !rindex(cpSlash,'.'))
     */

  
  {
    cp = pathBuf + strlen(pathBuf);
    strcpy(cp,suffix);
    if(access(pathBuf,F_OK))
      *cp = 0;
  }
  if(!access(pathBuf,F_OK))
    Unify_constant(term_true,A(3))
  else
    Unify_constant(term_fail,A(3))
  Unify_constant(MakeString(pathBuf),A(4));

  while (*cp!='/')
    --cp;
  *cp = 0;


  Unify_constant(MakeString(pathBuf),A(5));
  
  return TRUE;
}



#ifdef HAS_NO_GETWD
/* Define or fake the getwd() function here.
 * If we are running under UNIX System 5, we can do it with getcwd().
 */
#ifdef SYS5

char *getwd(buf)
    char *buf;
{
  return getcwd(buf, MAXPATHLEN+1);
}
#endif
#endif


void compute_cwd()
{
#ifdef unix
  getwd(cwd);
#endif
}


    
bool expand_file_name(name, target)
     char *name;
     char *target;
{
#ifndef unix
  strcpy(target,name);
#else
  extern char *getenv PROTO((char *));
  extern struct passwd *getpwnam PROTO((char *));

  register char *src, *dest, *temp;

  src = name, dest = target;
  if (src[0] != '/' && src[0] != '~')
    {
      strcpy(dest,cwd);
      dest += strlen(dest);
      *dest++ = '/';
    }

				/* phase 1: expand ~...  and $... */

 state0:			/* prev char is '/' or b.o.l. */
  switch (*dest++ = *src++)
    {
    case 0:
      src = dest = target;
      goto state20;
    case '/':
      goto state0;
    case '~':
      temp = dest;
      goto stateusr;
    case '$':
      temp = dest;
      goto stateenv;
    }

 state1:			/* inside file name component */
  switch (*dest++ = *src++)
    {
    case 0:
      src = dest = target;
      goto state20;
    case '/':
      goto state0;
    default:
      goto state1;
    }

 stateusr:			/* inside ~... component */
  switch (*dest++ = *src++)
    {
    case 0:
    case '/':
      --src, --dest, dest[0] = (char)0;
      if (temp == dest)
	{
	  --dest, dest[0] = (char)0;
	  if (!(temp = getenv("HOME")))
	    return FALSE;
	  strcat(dest,temp);
	  dest += strlen(dest);
	}
      else
	{
	  struct passwd *pw;

	  dest = temp-1, dest[0] = (char)0;
	  if (!(pw = getpwnam(temp)))
	    return FALSE;
	  strcat(dest,(char *)pw->pw_dir);
	  dest += strlen(dest);
	}	
      goto state0;
    default:
      goto stateusr;
    }

 stateenv:			/* inside $... component */
  switch (*dest++ = *src++)
    {
    case 0:
    case '/':
      --src, --dest, dest[0] = (char)0;
      if (temp == dest)
	dest[0] = '$';
      else
	{
	  dest = temp-1, dest[0] = (char)0;
	  if (!(temp = getenv(temp)))
	    return FALSE;
	  strcat(dest,temp);
	  dest += strlen(dest);
	}
      goto state0;
    default:
      goto stateenv;
    }

				/* phase 2: contract "." and ".." components */

 state10:			/* prev char is '/' */
  switch (*dest++ = *src++)
    {
    case 0:
      if (dest-2 > target)
	dest[-2] = 0;
      return TRUE;
    case '/':
#if aegis
      if (dest-3 < target || dest[-3] == '/')
	{
	  dest = target, *dest++ = '/', *dest++ = '/';
	  goto state10;
	}
#endif
      dest = target, *dest++ = '/';
      goto state10;
    case '.':
      if (src[0] == '/' || src[0] == (char)0)
	{
	  dest -= 2;
	  if (dest-1 <= target)
	    dest++;
	}
      else if (src[0] == '.' && (src[1] == '/' || src[1] == (char)0))
	{
	  src++, dest -= 2;
	  if (dest-1 <= target)
	    dest++;
	  while (--dest, dest[0] != '/')
	    ;
	  if (dest-1 <= target)
	    dest++;
	}
      if (dest[-1] == '/')
	goto state10;
    }

 state20:
  switch (*dest++ = *src++)
    {
    case 0:
      return TRUE;
    case '/':
      goto state10;
    default:
      goto state20;
    }
#endif
}


void break_to_gdb() {
#ifdef unix
  kill(getpid(), SIGTRAP);
#endif
}


bool akl_break_to_gdb(Arg)
     Argdecl;
{
  break_to_gdb();
  return TRUE;
}



void initialize_foreign() {

#ifdef unix
  define("$unix_cd",akl_unix_cd,2);
  define("$unix_shell",akl_unix_shell0,0);
  define("$unix_shell",akl_unix_shell2,2);
  define("$unix_system",akl_unix_system2,2);
  define("$unix_argv",akl_unix_argv,1);
  define("$unix_exit",akl_unix_exit,1);
  define("$unix_mktemp",akl_unix_mktemp,2);
  define("$unix_access",akl_unix_access,2);
  define("$unix_chmod",akl_unix_chmod,3);
  define("$unix_umask",akl_unix_umask,2);
  define("$unix_modified",akl_unix_modified,2);
#endif
  define("$find_file",akl_find_file,6);
  define("break_to_gdb",akl_break_to_gdb,0);
}
