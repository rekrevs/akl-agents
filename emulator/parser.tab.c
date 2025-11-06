/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 90 "parser.y"


#include <stdio.h>
#include <string.h>
#include "include.h"
#include "term.h"
#include "error.h"
#include "instructions.h"
#include "tree.h"
#include "config.h"
#include "predicate.h"
#include "exstate.h"
#include "storage.h"
#include "code.h"
#include "names.h"

#include "encodeinstr.h"
#include "instrdefs.h"
#include "fd_instr.h"
#include "fd.h"

#include "bam.h"

#define BITMASKBITS TADBITS
#define BitMaskWords(m)	(((m) + (BITMASKBITS-1)) / BITMASKBITS)

static char name[MAXATOMLEN];
static char pname[MAXATOMLEN];
static int abs_level;
static int arity;
static int parity;
static int ext;
static int size;
static int number;
static double fnumber;
static int index1;
static int index2;
static int index3;
static int offset1;
static int offset2;
static int label;
static int label1;
static int label2;
static int label3;
static int label4;
static int label5;
static int label6;
static int label7;
static int tablesize;
static uword bitmask;
static int bitmax;
static int bitmask_offset;
static Term const_term;
static Term fname;

/* Used by get_record and put_record... */
static int hv,features;
static Term *tbl;

#ifdef BAM
static bam_typed_type type_code;
#endif

int verbose = 0; 

static int yyerror();

/*
#define YYDEBUG 1
*/

/* Defined by (f)lex: */
extern int yylex PROTO((void));
extern char *yytext;
extern FILE *yyin;

#define yyparse parse


#line 151 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOKEN_SWITCH_ON_TERM = 3,       /* TOKEN_SWITCH_ON_TERM  */
  YYSYMBOL_TOKEN_TRY_SINGLE = 4,           /* TOKEN_TRY_SINGLE  */
  YYSYMBOL_TOKEN_TRY = 5,                  /* TOKEN_TRY  */
  YYSYMBOL_TOKEN_RETRY = 6,                /* TOKEN_RETRY  */
  YYSYMBOL_TOKEN_TRUST = 7,                /* TOKEN_TRUST  */
  YYSYMBOL_TOKEN_GUARD_COND = 8,           /* TOKEN_GUARD_COND  */
  YYSYMBOL_TOKEN_GUARD_COMMIT = 9,         /* TOKEN_GUARD_COMMIT  */
  YYSYMBOL_TOKEN_GUARD_WAIT = 10,          /* TOKEN_GUARD_WAIT  */
  YYSYMBOL_TOKEN_GUARD_UNORDER = 11,       /* TOKEN_GUARD_UNORDER  */
  YYSYMBOL_TOKEN_GUARD_ORDER = 12,         /* TOKEN_GUARD_ORDER  */
  YYSYMBOL_TOKEN_GUARD_UNIT = 13,          /* TOKEN_GUARD_UNIT  */
  YYSYMBOL_TOKEN_GUARD_CUT = 14,           /* TOKEN_GUARD_CUT  */
  YYSYMBOL_TOKEN_CALL = 15,                /* TOKEN_CALL  */
  YYSYMBOL_TOKEN_ALLOCATE = 16,            /* TOKEN_ALLOCATE  */
  YYSYMBOL_TOKEN_DEALLOCATE = 17,          /* TOKEN_DEALLOCATE  */
  YYSYMBOL_TOKEN_META_CALL = 18,           /* TOKEN_META_CALL  */
  YYSYMBOL_TOKEN_EXTERN_EXECUTE = 19,      /* TOKEN_EXTERN_EXECUTE  */
  YYSYMBOL_TOKEN_EXECUTE = 20,             /* TOKEN_EXECUTE  */
  YYSYMBOL_TOKEN_DEALLOC_EXECUTE = 21,     /* TOKEN_DEALLOC_EXECUTE  */
  YYSYMBOL_TOKEN_EXTERN_CALL = 22,         /* TOKEN_EXTERN_CALL  */
  YYSYMBOL_TOKEN_CALL_APPLY = 23,          /* TOKEN_CALL_APPLY  */
  YYSYMBOL_TOKEN_EXECUTE_APPLY = 24,       /* TOKEN_EXECUTE_APPLY  */
  YYSYMBOL_TOKEN_DEALLOC_EXECUTE_APPLY = 25, /* TOKEN_DEALLOC_EXECUTE_APPLY  */
  YYSYMBOL_TOKEN_PROCEED = 26,             /* TOKEN_PROCEED  */
  YYSYMBOL_TOKEN_DEALLOC_PROCEED = 27,     /* TOKEN_DEALLOC_PROCEED  */
  YYSYMBOL_TOKEN_FAIL = 28,                /* TOKEN_FAIL  */
  YYSYMBOL_TOKEN_GET_X_VARIABLE = 29,      /* TOKEN_GET_X_VARIABLE  */
  YYSYMBOL_TOKEN_GET_Y_VARIABLE = 30,      /* TOKEN_GET_Y_VARIABLE  */
  YYSYMBOL_TOKEN_GET2_Y_VARIABLE = 31,     /* TOKEN_GET2_Y_VARIABLE  */
  YYSYMBOL_TOKEN_GET3_Y_VARIABLE = 32,     /* TOKEN_GET3_Y_VARIABLE  */
  YYSYMBOL_TOKEN_GET4_Y_VARIABLE = 33,     /* TOKEN_GET4_Y_VARIABLE  */
  YYSYMBOL_TOKEN_GET5_Y_VARIABLE = 34,     /* TOKEN_GET5_Y_VARIABLE  */
  YYSYMBOL_TOKEN_GET_X_VALUE = 35,         /* TOKEN_GET_X_VALUE  */
  YYSYMBOL_TOKEN_GET_Y_VALUE = 36,         /* TOKEN_GET_Y_VALUE  */
  YYSYMBOL_TOKEN_GET_CONSTANT = 37,        /* TOKEN_GET_CONSTANT  */
  YYSYMBOL_TOKEN_GET_NIL = 38,             /* TOKEN_GET_NIL  */
  YYSYMBOL_TOKEN_GET_STRUCTURE = 39,       /* TOKEN_GET_STRUCTURE  */
  YYSYMBOL_TOKEN_GET_LIST = 40,            /* TOKEN_GET_LIST  */
  YYSYMBOL_TOKEN_GET_LIST_X0 = 41,         /* TOKEN_GET_LIST_X0  */
  YYSYMBOL_TOKEN_GET_NIL_X0 = 42,          /* TOKEN_GET_NIL_X0  */
  YYSYMBOL_TOKEN_GET_STRUCTURE_X0 = 43,    /* TOKEN_GET_STRUCTURE_X0  */
  YYSYMBOL_TOKEN_GET_CONSTANT_X0 = 44,     /* TOKEN_GET_CONSTANT_X0  */
  YYSYMBOL_TOKEN_GET_ABSTRACTION = 45,     /* TOKEN_GET_ABSTRACTION  */
  YYSYMBOL_TOKEN_PUT_X_VOID = 46,          /* TOKEN_PUT_X_VOID  */
  YYSYMBOL_TOKEN_PUT_Y_VOID = 47,          /* TOKEN_PUT_Y_VOID  */
  YYSYMBOL_TOKEN_PUT_X_VARIABLE = 48,      /* TOKEN_PUT_X_VARIABLE  */
  YYSYMBOL_TOKEN_PUT_Y_VARIABLE = 49,      /* TOKEN_PUT_Y_VARIABLE  */
  YYSYMBOL_TOKEN_PUT_X_VALUE = 50,         /* TOKEN_PUT_X_VALUE  */
  YYSYMBOL_TOKEN_PUT_Y_VALUE = 51,         /* TOKEN_PUT_Y_VALUE  */
  YYSYMBOL_TOKEN_PUT2_Y_VALUE = 52,        /* TOKEN_PUT2_Y_VALUE  */
  YYSYMBOL_TOKEN_PUT3_Y_VALUE = 53,        /* TOKEN_PUT3_Y_VALUE  */
  YYSYMBOL_TOKEN_PUT4_Y_VALUE = 54,        /* TOKEN_PUT4_Y_VALUE  */
  YYSYMBOL_TOKEN_PUT5_Y_VALUE = 55,        /* TOKEN_PUT5_Y_VALUE  */
  YYSYMBOL_TOKEN_PUT_CONSTANT = 56,        /* TOKEN_PUT_CONSTANT  */
  YYSYMBOL_TOKEN_PUT_NIL = 57,             /* TOKEN_PUT_NIL  */
  YYSYMBOL_TOKEN_PUT_STRUCTURE = 58,       /* TOKEN_PUT_STRUCTURE  */
  YYSYMBOL_TOKEN_PUT_LIST = 59,            /* TOKEN_PUT_LIST  */
  YYSYMBOL_TOKEN_UNIFY_VOID = 60,          /* TOKEN_UNIFY_VOID  */
  YYSYMBOL_TOKEN_PUT_ABSTRACTION = 61,     /* TOKEN_PUT_ABSTRACTION  */
  YYSYMBOL_TOKEN_UNIFY_Y_VARIABLE = 62,    /* TOKEN_UNIFY_Y_VARIABLE  */
  YYSYMBOL_TOKEN_UNIFY_X_VARIABLE = 63,    /* TOKEN_UNIFY_X_VARIABLE  */
  YYSYMBOL_TOKEN_UNIFY_X_VALUE = 64,       /* TOKEN_UNIFY_X_VALUE  */
  YYSYMBOL_TOKEN_UNIFY_Y_VALUE = 65,       /* TOKEN_UNIFY_Y_VALUE  */
  YYSYMBOL_TOKEN_UNIFY_CONSTANT = 66,      /* TOKEN_UNIFY_CONSTANT  */
  YYSYMBOL_TOKEN_UNIFY_NIL = 67,           /* TOKEN_UNIFY_NIL  */
  YYSYMBOL_TOKEN_UNIFY_LIST = 68,          /* TOKEN_UNIFY_LIST  */
  YYSYMBOL_TOKEN_UNIFY_STRUCTURE = 69,     /* TOKEN_UNIFY_STRUCTURE  */
  YYSYMBOL_TOKEN_SWITCH_ON_CONSTANT = 70,  /* TOKEN_SWITCH_ON_CONSTANT  */
  YYSYMBOL_TOKEN_SWITCH_ON_STRUCTURE = 71, /* TOKEN_SWITCH_ON_STRUCTURE  */
  YYSYMBOL_TOKEN_SEND3 = 72,               /* TOKEN_SEND3  */
  YYSYMBOL_TOKEN_SUSPEND_FLAT = 73,        /* TOKEN_SUSPEND_FLAT  */
  YYSYMBOL_TOKEN_ATOM_SWITCH = 74,         /* TOKEN_ATOM_SWITCH  */
  YYSYMBOL_TOKEN_FAIL_DEC = 75,            /* TOKEN_FAIL_DEC  */
  YYSYMBOL_TOKEN_LOAD_NIL = 76,            /* TOKEN_LOAD_NIL  */
  YYSYMBOL_TOKEN_LOAD_ATOM = 77,           /* TOKEN_LOAD_ATOM  */
  YYSYMBOL_TOKEN_LOAD_INTEGER = 78,        /* TOKEN_LOAD_INTEGER  */
  YYSYMBOL_TOKEN_READ_CAR = 79,            /* TOKEN_READ_CAR  */
  YYSYMBOL_TOKEN_READ_CDR = 80,            /* TOKEN_READ_CDR  */
  YYSYMBOL_TOKEN_READ_ARG = 81,            /* TOKEN_READ_ARG  */
  YYSYMBOL_TOKEN_SET_REG_REG = 82,         /* TOKEN_SET_REG_REG  */
  YYSYMBOL_TOKEN_SET_REG_YVAR = 83,        /* TOKEN_SET_REG_YVAR  */
  YYSYMBOL_TOKEN_SET_YVAR_REG = 84,        /* TOKEN_SET_YVAR_REG  */
  YYSYMBOL_TOKEN_TERM_SWITCH = 85,         /* TOKEN_TERM_SWITCH  */
  YYSYMBOL_TOKEN_ATOM_WITH_ELSE_SWITCH = 86, /* TOKEN_ATOM_WITH_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_ATOM_WITH_ATOM_ELSE_SWITCH = 87, /* TOKEN_ATOM_WITH_ATOM_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_ATOM_TYPE = 88,           /* TOKEN_ATOM_TYPE  */
  YYSYMBOL_TOKEN_ATOM_TYPE_WITH_ELSE = 89, /* TOKEN_ATOM_TYPE_WITH_ELSE  */
  YYSYMBOL_TOKEN_KNOWN_ATOM_SWITCH = 90,   /* TOKEN_KNOWN_ATOM_SWITCH  */
  YYSYMBOL_TOKEN_KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH = 91, /* TOKEN_KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_INTEGER_SWITCH = 92,      /* TOKEN_INTEGER_SWITCH  */
  YYSYMBOL_TOKEN_INTEGER_WITH_ELSE_SWITCH = 93, /* TOKEN_INTEGER_WITH_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_INTEGER_WITH_INTEGER_ELSE_SWITCH = 94, /* TOKEN_INTEGER_WITH_INTEGER_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_INTEGER_TYPE = 95,        /* TOKEN_INTEGER_TYPE  */
  YYSYMBOL_TOKEN_INTEGER_TYPE_WITH_ELSE = 96, /* TOKEN_INTEGER_TYPE_WITH_ELSE  */
  YYSYMBOL_TOKEN_KNOWN_INTEGER_SWITCH = 97, /* TOKEN_KNOWN_INTEGER_SWITCH  */
  YYSYMBOL_TOKEN_KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH = 98, /* TOKEN_KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_LIST_SWITCH = 99,         /* TOKEN_LIST_SWITCH  */
  YYSYMBOL_TOKEN_LIST_TYPE_WITH_ELSE = 100, /* TOKEN_LIST_TYPE_WITH_ELSE  */
  YYSYMBOL_TOKEN_STRUCT_TYPE = 101,        /* TOKEN_STRUCT_TYPE  */
  YYSYMBOL_TOKEN_STRUCT_TYPE_WITH_ELSE = 102, /* TOKEN_STRUCT_TYPE_WITH_ELSE  */
  YYSYMBOL_TOKEN_KNOWN_FUNCTOR_SWITCH = 103, /* TOKEN_KNOWN_FUNCTOR_SWITCH  */
  YYSYMBOL_TOKEN_KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH = 104, /* TOKEN_KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH  */
  YYSYMBOL_TOKEN_INT_COMPARE_REG_REG = 105, /* TOKEN_INT_COMPARE_REG_REG  */
  YYSYMBOL_TOKEN_INT_COMPARE_REG_INT = 106, /* TOKEN_INT_COMPARE_REG_INT  */
  YYSYMBOL_TOKEN_EQ_REG_REG = 107,         /* TOKEN_EQ_REG_REG  */
  YYSYMBOL_TOKEN_MATCH_REG_REG = 108,      /* TOKEN_MATCH_REG_REG  */
  YYSYMBOL_TOKEN_MATCH_REG_REG_OFF = 109,  /* TOKEN_MATCH_REG_REG_OFF  */
  YYSYMBOL_TOKEN_MATCH_REG_ATOM = 110,     /* TOKEN_MATCH_REG_ATOM  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_ATOM = 111, /* TOKEN_MATCH_REG_OFF_ATOM  */
  YYSYMBOL_TOKEN_MATCH_REG_INTEGER = 112,  /* TOKEN_MATCH_REG_INTEGER  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_INTEGER = 113, /* TOKEN_MATCH_REG_OFF_INTEGER  */
  YYSYMBOL_TOKEN_MATCH_REG_NIL = 114,      /* TOKEN_MATCH_REG_NIL  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_NIL = 115,  /* TOKEN_MATCH_REG_OFF_NIL  */
  YYSYMBOL_TOKEN_MATCH_REG_YVAR = 116,     /* TOKEN_MATCH_REG_YVAR  */
  YYSYMBOL_TOKEN_MATCH_YVAR_YVAR = 117,    /* TOKEN_MATCH_YVAR_YVAR  */
  YYSYMBOL_TOKEN_MATCH_YVAR_REG_OFF = 118, /* TOKEN_MATCH_YVAR_REG_OFF  */
  YYSYMBOL_TOKEN_MATCH_REG_H_LIST = 119,   /* TOKEN_MATCH_REG_H_LIST  */
  YYSYMBOL_TOKEN_MATCH_YVAR_H_LIST = 120,  /* TOKEN_MATCH_YVAR_H_LIST  */
  YYSYMBOL_TOKEN_MATCH_REG_H_STRUCT = 121, /* TOKEN_MATCH_REG_H_STRUCT  */
  YYSYMBOL_TOKEN_MATCH_YVAR_H_STRUCT = 122, /* TOKEN_MATCH_YVAR_H_STRUCT  */
  YYSYMBOL_TOKEN_MATCH_REG_STRUCT = 123,   /* TOKEN_MATCH_REG_STRUCT  */
  YYSYMBOL_TOKEN_MATCH_REG_STRUCT_LEVEL = 124, /* TOKEN_MATCH_REG_STRUCT_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_STRUCT = 125, /* TOKEN_MATCH_REG_OFF_STRUCT  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_STRUCT_LEVEL = 126, /* TOKEN_MATCH_REG_OFF_STRUCT_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_YVAR_STRUCT = 127,  /* TOKEN_MATCH_YVAR_STRUCT  */
  YYSYMBOL_TOKEN_MATCH_YVAR_STRUCT_LEVEL = 128, /* TOKEN_MATCH_YVAR_STRUCT_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_REG_LIST_CAR = 129, /* TOKEN_MATCH_REG_LIST_CAR  */
  YYSYMBOL_TOKEN_MATCH_REG_LIST_CAR_LEVEL = 130, /* TOKEN_MATCH_REG_LIST_CAR_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_LIST_CAR = 131, /* TOKEN_MATCH_REG_OFF_LIST_CAR  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_LIST_CAR_LEVEL = 132, /* TOKEN_MATCH_REG_OFF_LIST_CAR_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_YVAR_LIST_CAR = 133, /* TOKEN_MATCH_YVAR_LIST_CAR  */
  YYSYMBOL_TOKEN_MATCH_YVAR_LIST_CAR_LEVEL = 134, /* TOKEN_MATCH_YVAR_LIST_CAR_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_REG_LIST_CDR = 135, /* TOKEN_MATCH_REG_LIST_CDR  */
  YYSYMBOL_TOKEN_MATCH_REG_LIST_CDR_LEVEL = 136, /* TOKEN_MATCH_REG_LIST_CDR_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_LIST_CDR = 137, /* TOKEN_MATCH_REG_OFF_LIST_CDR  */
  YYSYMBOL_TOKEN_MATCH_REG_OFF_LIST_CDR_LEVEL = 138, /* TOKEN_MATCH_REG_OFF_LIST_CDR_LEVEL  */
  YYSYMBOL_TOKEN_MATCH_YVAR_LIST_CDR = 139, /* TOKEN_MATCH_YVAR_LIST_CDR  */
  YYSYMBOL_TOKEN_MATCH_YVAR_LIST_CDR_LEVEL = 140, /* TOKEN_MATCH_YVAR_LIST_CDR_LEVEL  */
  YYSYMBOL_TOKEN_PUSH_LIST_IMM = 141,      /* TOKEN_PUSH_LIST_IMM  */
  YYSYMBOL_TOKEN_PUSH_LIST = 142,          /* TOKEN_PUSH_LIST  */
  YYSYMBOL_TOKEN_PUSH_STRUCTURE_IMM = 143, /* TOKEN_PUSH_STRUCTURE_IMM  */
  YYSYMBOL_TOKEN_PUSH_STRUCTURE = 144,     /* TOKEN_PUSH_STRUCTURE  */
  YYSYMBOL_TOKEN_PUSH_NIL = 145,           /* TOKEN_PUSH_NIL  */
  YYSYMBOL_TOKEN_PUSH_REG = 146,           /* TOKEN_PUSH_REG  */
  YYSYMBOL_TOKEN_PUSH_YVAR = 147,          /* TOKEN_PUSH_YVAR  */
  YYSYMBOL_TOKEN_PUSH_VOID = 148,          /* TOKEN_PUSH_VOID  */
  YYSYMBOL_TOKEN_SET_REG_H_AND_PUSH_VOID = 149, /* TOKEN_SET_REG_H_AND_PUSH_VOID  */
  YYSYMBOL_TOKEN_SET_YVAR_H_AND_PUSH_VOID = 150, /* TOKEN_SET_YVAR_H_AND_PUSH_VOID  */
  YYSYMBOL_TOKEN_PUSH_INTEGER = 151,       /* TOKEN_PUSH_INTEGER  */
  YYSYMBOL_TOKEN_PUSH_FUNCTOR = 152,       /* TOKEN_PUSH_FUNCTOR  */
  YYSYMBOL_TOKEN_SET_REG_AND_YVAR_H_AND_PUSH_VOID = 153, /* TOKEN_SET_REG_AND_YVAR_H_AND_PUSH_VOID  */
  YYSYMBOL_TOKEN_PUSH_ATOM = 154,          /* TOKEN_PUSH_ATOM  */
  YYSYMBOL_TOKEN_SET_REG_H = 155,          /* TOKEN_SET_REG_H  */
  YYSYMBOL_TOKEN_SET_YVAR_H = 156,         /* TOKEN_SET_YVAR_H  */
  YYSYMBOL_TOKEN_BTRY = 157,               /* TOKEN_BTRY  */
  YYSYMBOL_TOKEN_BTRUST = 158,             /* TOKEN_BTRUST  */
  YYSYMBOL_TOKEN_TEST_TRY = 159,           /* TOKEN_TEST_TRY  */
  YYSYMBOL_TOKEN_TEST_TRUST = 160,         /* TOKEN_TEST_TRUST  */
  YYSYMBOL_TOKEN_S_TRY = 161,              /* TOKEN_S_TRY  */
  YYSYMBOL_TOKEN_S_TRUST = 162,            /* TOKEN_S_TRUST  */
  YYSYMBOL_TOKEN_BACK = 163,               /* TOKEN_BACK  */
  YYSYMBOL_TOKEN_GEN_TRY = 164,            /* TOKEN_GEN_TRY  */
  YYSYMBOL_TOKEN_GEN_TRUST = 165,          /* TOKEN_GEN_TRUST  */
  YYSYMBOL_TOKEN_BEXECUTE_SIMPLE = 166,    /* TOKEN_BEXECUTE_SIMPLE  */
  YYSYMBOL_TOKEN_BEXECUTE = 167,           /* TOKEN_BEXECUTE  */
  YYSYMBOL_TOKEN_BCALL = 168,              /* TOKEN_BCALL  */
  YYSYMBOL_TOKEN_JUMP_GC_AND_DUMP_CONT = 169, /* TOKEN_JUMP_GC_AND_DUMP_CONT  */
  YYSYMBOL_TOKEN_JUMP_GC = 170,            /* TOKEN_JUMP_GC  */
  YYSYMBOL_TOKEN_JUMP = 171,               /* TOKEN_JUMP  */
  YYSYMBOL_TOKEN_BPROCEED_SIMPLE = 172,    /* TOKEN_BPROCEED_SIMPLE  */
  YYSYMBOL_TOKEN_BPROCEED = 173,           /* TOKEN_BPROCEED  */
  YYSYMBOL_TOKEN_CREATE_CONT = 174,        /* TOKEN_CREATE_CONT  */
  YYSYMBOL_TOKEN_TEST_LEVEL = 175,         /* TOKEN_TEST_LEVEL  */
  YYSYMBOL_TOKEN_CONSTRAINT_STACK_TEST = 176, /* TOKEN_CONSTRAINT_STACK_TEST  */
  YYSYMBOL_TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK = 177, /* TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK  */
  YYSYMBOL_TOKEN_ALIAS_CHECK = 178,        /* TOKEN_ALIAS_CHECK  */
  YYSYMBOL_TOKEN_ALIAS_CHECK_REG = 179,    /* TOKEN_ALIAS_CHECK_REG  */
  YYSYMBOL_TOKEN_FAIL_ON_ALIAS = 180,      /* TOKEN_FAIL_ON_ALIAS  */
  YYSYMBOL_TOKEN_CHOICE_QUIET_CUT = 181,   /* TOKEN_CHOICE_QUIET_CUT  */
  YYSYMBOL_TOKEN_SUSPEND_LONE_VAR = 182,   /* TOKEN_SUSPEND_LONE_VAR  */
  YYSYMBOL_TOKEN_BSUSPEND = 183,           /* TOKEN_BSUSPEND  */
  YYSYMBOL_TOKEN_SUSPEND_WITHOUT_REG = 184, /* TOKEN_SUSPEND_WITHOUT_REG  */
  YYSYMBOL_TOKEN_CREATE_GUARD = 185,       /* TOKEN_CREATE_GUARD  */
  YYSYMBOL_TOKEN_FLAT_QUIET_CUT = 186,     /* TOKEN_FLAT_QUIET_CUT  */
  YYSYMBOL_TOKEN_FLAT_QUIET_COMMIT = 187,  /* TOKEN_FLAT_QUIET_COMMIT  */
  YYSYMBOL_TOKEN_FLAT_NOISY_WAIT = 188,    /* TOKEN_FLAT_NOISY_WAIT  */
  YYSYMBOL_TOKEN_SHORTCUT_QUIET_CUT = 189, /* TOKEN_SHORTCUT_QUIET_CUT  */
  YYSYMBOL_TOKEN_SHORTCUT_QUIET_COMMIT = 190, /* TOKEN_SHORTCUT_QUIET_COMMIT  */
  YYSYMBOL_TOKEN_SHORTCUT_NOISY_WAIT = 191, /* TOKEN_SHORTCUT_NOISY_WAIT  */
  YYSYMBOL_TOKEN_DEEP_QUIET_CUT = 192,     /* TOKEN_DEEP_QUIET_CUT  */
  YYSYMBOL_TOKEN_DEEP_QUIET_COMMIT = 193,  /* TOKEN_DEEP_QUIET_COMMIT  */
  YYSYMBOL_TOKEN_DEEP_NOISY_WAIT = 194,    /* TOKEN_DEEP_NOISY_WAIT  */
  YYSYMBOL_PREDICATE = 195,                /* PREDICATE  */
  YYSYMBOL_ATOMNAME = 196,                 /* ATOMNAME  */
  YYSYMBOL_NEWLINE = 197,                  /* NEWLINE  */
  YYSYMBOL_LPAR = 198,                     /* LPAR  */
  YYSYMBOL_RPAR = 199,                     /* RPAR  */
  YYSYMBOL_LHPAR = 200,                    /* LHPAR  */
  YYSYMBOL_RHPAR = 201,                    /* RHPAR  */
  YYSYMBOL_COMMA = 202,                    /* COMMA  */
  YYSYMBOL_DASH = 203,                     /* DASH  */
  YYSYMBOL_SLASH = 204,                    /* SLASH  */
  YYSYMBOL_LABEL = 205,                    /* LABEL  */
  YYSYMBOL_LABELTAG = 206,                 /* LABELTAG  */
  YYSYMBOL_NUMBER = 207,                   /* NUMBER  */
  YYSYMBOL_FLOAT = 208,                    /* FLOAT  */
  YYSYMBOL_END = 209,                      /* END  */
  YYSYMBOL_PLABEL = 210,                   /* PLABEL  */
  YYSYMBOL_CONSTANT = 211,                 /* CONSTANT  */
  YYSYMBOL_LIST = 212,                     /* LIST  */
  YYSYMBOL_STRUCT = 213,                   /* STRUCT  */
  YYSYMBOL_TYPED = 214,                    /* TYPED  */
  YYSYMBOL_INTEGER = 215,                  /* INTEGER  */
  YYSYMBOL_ATOM = 216,                     /* ATOM  */
  YYSYMBOL_FUNCTOR = 217,                  /* FUNCTOR  */
  YYSYMBOL_REG = 218,                      /* REG  */
  YYSYMBOL_VOID = 219,                     /* VOID  */
  YYSYMBOL_LABELED = 220,                  /* LABELED  */
  YYSYMBOL_TOKEN_GET_RECORD = 221,         /* TOKEN_GET_RECORD  */
  YYSYMBOL_TOKEN_PUT_RECORD = 222,         /* TOKEN_PUT_RECORD  */
  YYSYMBOL_FD_code_T = 223,                /* FD_code_T  */
  YYSYMBOL_FD_const_T = 224,               /* FD_const_T  */
  YYSYMBOL_FD_max_elem_T = 225,            /* FD_max_elem_T  */
  YYSYMBOL_FD_add_T = 226,                 /* FD_add_T  */
  YYSYMBOL_FD_sub_T = 227,                 /* FD_sub_T  */
  YYSYMBOL_FD_mult_T = 228,                /* FD_mult_T  */
  YYSYMBOL_FD_divd_T = 229,                /* FD_divd_T  */
  YYSYMBOL_FD_divu_T = 230,                /* FD_divu_T  */
  YYSYMBOL_FD_mod_T = 231,                 /* FD_mod_T  */
  YYSYMBOL_FD_val_T = 232,                 /* FD_val_T  */
  YYSYMBOL_FD_val_0_T = 233,               /* FD_val_0_T  */
  YYSYMBOL_FD_val_1_T = 234,               /* FD_val_1_T  */
  YYSYMBOL_FD_val_2_T = 235,               /* FD_val_2_T  */
  YYSYMBOL_FD_min_T = 236,                 /* FD_min_T  */
  YYSYMBOL_FD_max_T = 237,                 /* FD_max_T  */
  YYSYMBOL_FD_dom_min_T = 238,             /* FD_dom_min_T  */
  YYSYMBOL_FD_dom_max_T = 239,             /* FD_dom_max_T  */
  YYSYMBOL_FD_dom_min_0_T = 240,           /* FD_dom_min_0_T  */
  YYSYMBOL_FD_dom_max_0_T = 241,           /* FD_dom_max_0_T  */
  YYSYMBOL_FD_dom_min_1_T = 242,           /* FD_dom_min_1_T  */
  YYSYMBOL_FD_dom_max_1_T = 243,           /* FD_dom_max_1_T  */
  YYSYMBOL_FD_dom_min_2_T = 244,           /* FD_dom_min_2_T  */
  YYSYMBOL_FD_dom_max_2_T = 245,           /* FD_dom_max_2_T  */
  YYSYMBOL_FD_dom_T = 246,                 /* FD_dom_T  */
  YYSYMBOL_FD_dom_0_T = 247,               /* FD_dom_0_T  */
  YYSYMBOL_FD_dom_1_T = 248,               /* FD_dom_1_T  */
  YYSYMBOL_FD_dom_2_T = 249,               /* FD_dom_2_T  */
  YYSYMBOL_FD_range_T = 250,               /* FD_range_T  */
  YYSYMBOL_FD_outer_range_T = 251,         /* FD_outer_range_T  */
  YYSYMBOL_FD_setadd_T = 252,              /* FD_setadd_T  */
  YYSYMBOL_FD_setsub_T = 253,              /* FD_setsub_T  */
  YYSYMBOL_FD_setmod_T = 254,              /* FD_setmod_T  */
  YYSYMBOL_FD_compl_T = 255,               /* FD_compl_T  */
  YYSYMBOL_FD_union_T = 256,               /* FD_union_T  */
  YYSYMBOL_FD_inter_T = 257,               /* FD_inter_T  */
  YYSYMBOL_FD_check_T = 258,               /* FD_check_T  */
  YYSYMBOL_FD_label_T = 259,               /* FD_label_T  */
  YYSYMBOL_FD_element_i_T = 260,           /* FD_element_i_T  */
  YYSYMBOL_FD_element_x_T = 261,           /* FD_element_x_T  */
  YYSYMBOL_YYACCEPT = 262,                 /* $accept  */
  YYSYMBOL_definitions = 263,              /* definitions  */
  YYSYMBOL_264_1 = 264,                    /* $@1  */
  YYSYMBOL_265_2 = 265,                    /* $@2  */
  YYSYMBOL_266_3 = 266,                    /* $@3  */
  YYSYMBOL_267_4 = 267,                    /* $@4  */
  YYSYMBOL_268_5 = 268,                    /* $@5  */
  YYSYMBOL_code = 269,                     /* code  */
  YYSYMBOL_instruction = 270,              /* instruction  */
  YYSYMBOL_271_6 = 271,                    /* $@6  */
  YYSYMBOL_272_7 = 272,                    /* $@7  */
  YYSYMBOL_273_8 = 273,                    /* $@8  */
  YYSYMBOL_274_9 = 274,                    /* $@9  */
  YYSYMBOL_275_10 = 275,                   /* $@10  */
  YYSYMBOL_276_11 = 276,                   /* $@11  */
  YYSYMBOL_277_12 = 277,                   /* $@12  */
  YYSYMBOL_278_13 = 278,                   /* $@13  */
  YYSYMBOL_279_14 = 279,                   /* $@14  */
  YYSYMBOL_280_15 = 280,                   /* $@15  */
  YYSYMBOL_281_16 = 281,                   /* $@16  */
  YYSYMBOL_282_17 = 282,                   /* $@17  */
  YYSYMBOL_283_18 = 283,                   /* $@18  */
  YYSYMBOL_284_19 = 284,                   /* $@19  */
  YYSYMBOL_285_20 = 285,                   /* $@20  */
  YYSYMBOL_286_21 = 286,                   /* $@21  */
  YYSYMBOL_287_22 = 287,                   /* $@22  */
  YYSYMBOL_288_23 = 288,                   /* $@23  */
  YYSYMBOL_289_24 = 289,                   /* $@24  */
  YYSYMBOL_290_25 = 290,                   /* $@25  */
  YYSYMBOL_291_26 = 291,                   /* $@26  */
  YYSYMBOL_292_27 = 292,                   /* $@27  */
  YYSYMBOL_293_28 = 293,                   /* $@28  */
  YYSYMBOL_294_29 = 294,                   /* $@29  */
  YYSYMBOL_295_30 = 295,                   /* $@30  */
  YYSYMBOL_296_31 = 296,                   /* $@31  */
  YYSYMBOL_297_32 = 297,                   /* $@32  */
  YYSYMBOL_298_33 = 298,                   /* $@33  */
  YYSYMBOL_299_34 = 299,                   /* $@34  */
  YYSYMBOL_300_35 = 300,                   /* $@35  */
  YYSYMBOL_301_36 = 301,                   /* $@36  */
  YYSYMBOL_302_37 = 302,                   /* $@37  */
  YYSYMBOL_303_38 = 303,                   /* $@38  */
  YYSYMBOL_304_39 = 304,                   /* $@39  */
  YYSYMBOL_305_40 = 305,                   /* $@40  */
  YYSYMBOL_306_41 = 306,                   /* $@41  */
  YYSYMBOL_307_42 = 307,                   /* $@42  */
  YYSYMBOL_308_43 = 308,                   /* $@43  */
  YYSYMBOL_309_44 = 309,                   /* $@44  */
  YYSYMBOL_310_45 = 310,                   /* $@45  */
  YYSYMBOL_311_46 = 311,                   /* $@46  */
  YYSYMBOL_312_47 = 312,                   /* $@47  */
  YYSYMBOL_313_48 = 313,                   /* $@48  */
  YYSYMBOL_314_49 = 314,                   /* $@49  */
  YYSYMBOL_315_50 = 315,                   /* $@50  */
  YYSYMBOL_316_51 = 316,                   /* $@51  */
  YYSYMBOL_317_52 = 317,                   /* $@52  */
  YYSYMBOL_318_53 = 318,                   /* $@53  */
  YYSYMBOL_319_54 = 319,                   /* $@54  */
  YYSYMBOL_320_55 = 320,                   /* $@55  */
  YYSYMBOL_321_56 = 321,                   /* $@56  */
  YYSYMBOL_322_57 = 322,                   /* $@57  */
  YYSYMBOL_323_58 = 323,                   /* $@58  */
  YYSYMBOL_324_59 = 324,                   /* $@59  */
  YYSYMBOL_325_60 = 325,                   /* $@60  */
  YYSYMBOL_326_61 = 326,                   /* $@61  */
  YYSYMBOL_327_62 = 327,                   /* $@62  */
  YYSYMBOL_328_63 = 328,                   /* $@63  */
  YYSYMBOL_329_64 = 329,                   /* $@64  */
  YYSYMBOL_330_65 = 330,                   /* $@65  */
  YYSYMBOL_331_66 = 331,                   /* $@66  */
  YYSYMBOL_332_67 = 332,                   /* $@67  */
  YYSYMBOL_333_68 = 333,                   /* $@68  */
  YYSYMBOL_334_69 = 334,                   /* $@69  */
  YYSYMBOL_335_70 = 335,                   /* $@70  */
  YYSYMBOL_336_71 = 336,                   /* $@71  */
  YYSYMBOL_337_72 = 337,                   /* $@72  */
  YYSYMBOL_338_73 = 338,                   /* $@73  */
  YYSYMBOL_339_74 = 339,                   /* $@74  */
  YYSYMBOL_340_75 = 340,                   /* $@75  */
  YYSYMBOL_341_76 = 341,                   /* $@76  */
  YYSYMBOL_342_77 = 342,                   /* $@77  */
  YYSYMBOL_343_78 = 343,                   /* $@78  */
  YYSYMBOL_344_79 = 344,                   /* $@79  */
  YYSYMBOL_345_80 = 345,                   /* $@80  */
  YYSYMBOL_346_81 = 346,                   /* $@81  */
  YYSYMBOL_347_82 = 347,                   /* $@82  */
  YYSYMBOL_348_83 = 348,                   /* $@83  */
  YYSYMBOL_349_84 = 349,                   /* $@84  */
  YYSYMBOL_350_85 = 350,                   /* $@85  */
  YYSYMBOL_351_86 = 351,                   /* $@86  */
  YYSYMBOL_352_87 = 352,                   /* $@87  */
  YYSYMBOL_353_88 = 353,                   /* $@88  */
  YYSYMBOL_354_89 = 354,                   /* $@89  */
  YYSYMBOL_355_90 = 355,                   /* $@90  */
  YYSYMBOL_356_91 = 356,                   /* $@91  */
  YYSYMBOL_357_92 = 357,                   /* $@92  */
  YYSYMBOL_358_93 = 358,                   /* $@93  */
  YYSYMBOL_359_94 = 359,                   /* $@94  */
  YYSYMBOL_360_95 = 360,                   /* $@95  */
  YYSYMBOL_361_96 = 361,                   /* $@96  */
  YYSYMBOL_362_97 = 362,                   /* $@97  */
  YYSYMBOL_363_98 = 363,                   /* $@98  */
  YYSYMBOL_364_99 = 364,                   /* $@99  */
  YYSYMBOL_365_100 = 365,                  /* $@100  */
  YYSYMBOL_366_101 = 366,                  /* $@101  */
  YYSYMBOL_367_102 = 367,                  /* $@102  */
  YYSYMBOL_368_103 = 368,                  /* $@103  */
  YYSYMBOL_369_104 = 369,                  /* $@104  */
  YYSYMBOL_370_105 = 370,                  /* $@105  */
  YYSYMBOL_371_106 = 371,                  /* $@106  */
  YYSYMBOL_372_107 = 372,                  /* $@107  */
  YYSYMBOL_373_108 = 373,                  /* $@108  */
  YYSYMBOL_374_109 = 374,                  /* $@109  */
  YYSYMBOL_375_110 = 375,                  /* $@110  */
  YYSYMBOL_376_111 = 376,                  /* $@111  */
  YYSYMBOL_377_112 = 377,                  /* $@112  */
  YYSYMBOL_378_113 = 378,                  /* $@113  */
  YYSYMBOL_379_114 = 379,                  /* $@114  */
  YYSYMBOL_380_115 = 380,                  /* $@115  */
  YYSYMBOL_381_116 = 381,                  /* $@116  */
  YYSYMBOL_382_117 = 382,                  /* $@117  */
  YYSYMBOL_383_118 = 383,                  /* $@118  */
  YYSYMBOL_384_119 = 384,                  /* $@119  */
  YYSYMBOL_385_120 = 385,                  /* $@120  */
  YYSYMBOL_386_121 = 386,                  /* $@121  */
  YYSYMBOL_387_122 = 387,                  /* $@122  */
  YYSYMBOL_388_123 = 388,                  /* $@123  */
  YYSYMBOL_389_124 = 389,                  /* $@124  */
  YYSYMBOL_390_125 = 390,                  /* $@125  */
  YYSYMBOL_391_126 = 391,                  /* $@126  */
  YYSYMBOL_392_127 = 392,                  /* $@127  */
  YYSYMBOL_393_128 = 393,                  /* $@128  */
  YYSYMBOL_394_129 = 394,                  /* $@129  */
  YYSYMBOL_395_130 = 395,                  /* $@130  */
  YYSYMBOL_396_131 = 396,                  /* $@131  */
  YYSYMBOL_397_132 = 397,                  /* $@132  */
  YYSYMBOL_398_133 = 398,                  /* $@133  */
  YYSYMBOL_399_134 = 399,                  /* $@134  */
  YYSYMBOL_400_135 = 400,                  /* $@135  */
  YYSYMBOL_401_136 = 401,                  /* $@136  */
  YYSYMBOL_402_137 = 402,                  /* $@137  */
  YYSYMBOL_403_138 = 403,                  /* $@138  */
  YYSYMBOL_404_139 = 404,                  /* $@139  */
  YYSYMBOL_405_140 = 405,                  /* $@140  */
  YYSYMBOL_406_141 = 406,                  /* $@141  */
  YYSYMBOL_407_142 = 407,                  /* $@142  */
  YYSYMBOL_408_143 = 408,                  /* $@143  */
  YYSYMBOL_409_144 = 409,                  /* $@144  */
  YYSYMBOL_410_145 = 410,                  /* $@145  */
  YYSYMBOL_411_146 = 411,                  /* $@146  */
  YYSYMBOL_412_147 = 412,                  /* $@147  */
  YYSYMBOL_413_148 = 413,                  /* $@148  */
  YYSYMBOL_414_149 = 414,                  /* $@149  */
  YYSYMBOL_415_150 = 415,                  /* $@150  */
  YYSYMBOL_416_151 = 416,                  /* $@151  */
  YYSYMBOL_417_152 = 417,                  /* $@152  */
  YYSYMBOL_418_153 = 418,                  /* $@153  */
  YYSYMBOL_419_154 = 419,                  /* $@154  */
  YYSYMBOL_420_155 = 420,                  /* $@155  */
  YYSYMBOL_421_156 = 421,                  /* $@156  */
  YYSYMBOL_422_157 = 422,                  /* $@157  */
  YYSYMBOL_423_158 = 423,                  /* $@158  */
  YYSYMBOL_424_159 = 424,                  /* $@159  */
  YYSYMBOL_425_160 = 425,                  /* $@160  */
  YYSYMBOL_426_161 = 426,                  /* $@161  */
  YYSYMBOL_427_162 = 427,                  /* $@162  */
  YYSYMBOL_428_163 = 428,                  /* $@163  */
  YYSYMBOL_429_164 = 429,                  /* $@164  */
  YYSYMBOL_430_165 = 430,                  /* $@165  */
  YYSYMBOL_431_166 = 431,                  /* $@166  */
  YYSYMBOL_432_167 = 432,                  /* $@167  */
  YYSYMBOL_433_168 = 433,                  /* $@168  */
  YYSYMBOL_434_169 = 434,                  /* $@169  */
  YYSYMBOL_435_170 = 435,                  /* $@170  */
  YYSYMBOL_436_171 = 436,                  /* $@171  */
  YYSYMBOL_437_172 = 437,                  /* $@172  */
  YYSYMBOL_438_173 = 438,                  /* $@173  */
  YYSYMBOL_439_174 = 439,                  /* $@174  */
  YYSYMBOL_440_175 = 440,                  /* $@175  */
  YYSYMBOL_441_176 = 441,                  /* $@176  */
  YYSYMBOL_442_177 = 442,                  /* $@177  */
  YYSYMBOL_443_178 = 443,                  /* $@178  */
  YYSYMBOL_444_179 = 444,                  /* $@179  */
  YYSYMBOL_445_180 = 445,                  /* $@180  */
  YYSYMBOL_446_181 = 446,                  /* $@181  */
  YYSYMBOL_447_182 = 447,                  /* $@182  */
  YYSYMBOL_448_183 = 448,                  /* $@183  */
  YYSYMBOL_449_184 = 449,                  /* $@184  */
  YYSYMBOL_450_185 = 450,                  /* $@185  */
  YYSYMBOL_451_186 = 451,                  /* $@186  */
  YYSYMBOL_452_187 = 452,                  /* $@187  */
  YYSYMBOL_453_188 = 453,                  /* $@188  */
  YYSYMBOL_454_189 = 454,                  /* $@189  */
  YYSYMBOL_455_190 = 455,                  /* $@190  */
  YYSYMBOL_456_191 = 456,                  /* $@191  */
  YYSYMBOL_457_192 = 457,                  /* $@192  */
  YYSYMBOL_458_193 = 458,                  /* $@193  */
  YYSYMBOL_459_194 = 459,                  /* $@194  */
  YYSYMBOL_460_195 = 460,                  /* $@195  */
  YYSYMBOL_461_196 = 461,                  /* $@196  */
  YYSYMBOL_462_197 = 462,                  /* $@197  */
  YYSYMBOL_463_198 = 463,                  /* $@198  */
  YYSYMBOL_464_199 = 464,                  /* $@199  */
  YYSYMBOL_465_200 = 465,                  /* $@200  */
  YYSYMBOL_466_201 = 466,                  /* $@201  */
  YYSYMBOL_467_202 = 467,                  /* $@202  */
  YYSYMBOL_468_203 = 468,                  /* $@203  */
  YYSYMBOL_469_204 = 469,                  /* $@204  */
  YYSYMBOL_470_205 = 470,                  /* $@205  */
  YYSYMBOL_471_206 = 471,                  /* $@206  */
  YYSYMBOL_472_207 = 472,                  /* $@207  */
  YYSYMBOL_473_208 = 473,                  /* $@208  */
  YYSYMBOL_474_209 = 474,                  /* $@209  */
  YYSYMBOL_475_210 = 475,                  /* $@210  */
  YYSYMBOL_476_211 = 476,                  /* $@211  */
  YYSYMBOL_477_212 = 477,                  /* $@212  */
  YYSYMBOL_478_213 = 478,                  /* $@213  */
  YYSYMBOL_479_214 = 479,                  /* $@214  */
  YYSYMBOL_480_215 = 480,                  /* $@215  */
  YYSYMBOL_481_216 = 481,                  /* $@216  */
  YYSYMBOL_482_217 = 482,                  /* $@217  */
  YYSYMBOL_483_218 = 483,                  /* $@218  */
  YYSYMBOL_484_219 = 484,                  /* $@219  */
  YYSYMBOL_485_220 = 485,                  /* $@220  */
  YYSYMBOL_486_221 = 486,                  /* $@221  */
  YYSYMBOL_487_222 = 487,                  /* $@222  */
  YYSYMBOL_488_223 = 488,                  /* $@223  */
  YYSYMBOL_489_224 = 489,                  /* $@224  */
  YYSYMBOL_490_225 = 490,                  /* $@225  */
  YYSYMBOL_491_226 = 491,                  /* $@226  */
  YYSYMBOL_492_227 = 492,                  /* $@227  */
  YYSYMBOL_493_228 = 493,                  /* $@228  */
  YYSYMBOL_494_229 = 494,                  /* $@229  */
  YYSYMBOL_495_230 = 495,                  /* $@230  */
  YYSYMBOL_496_231 = 496,                  /* $@231  */
  YYSYMBOL_497_232 = 497,                  /* $@232  */
  YYSYMBOL_498_233 = 498,                  /* $@233  */
  YYSYMBOL_499_234 = 499,                  /* $@234  */
  YYSYMBOL_500_235 = 500,                  /* $@235  */
  YYSYMBOL_501_236 = 501,                  /* $@236  */
  YYSYMBOL_502_237 = 502,                  /* $@237  */
  YYSYMBOL_503_238 = 503,                  /* $@238  */
  YYSYMBOL_504_239 = 504,                  /* $@239  */
  YYSYMBOL_505_240 = 505,                  /* $@240  */
  YYSYMBOL_506_241 = 506,                  /* $@241  */
  YYSYMBOL_507_242 = 507,                  /* $@242  */
  YYSYMBOL_508_243 = 508,                  /* $@243  */
  YYSYMBOL_509_244 = 509,                  /* $@244  */
  YYSYMBOL_510_245 = 510,                  /* $@245  */
  YYSYMBOL_511_246 = 511,                  /* $@246  */
  YYSYMBOL_512_247 = 512,                  /* $@247  */
  YYSYMBOL_513_248 = 513,                  /* $@248  */
  YYSYMBOL_514_249 = 514,                  /* $@249  */
  YYSYMBOL_515_250 = 515,                  /* $@250  */
  YYSYMBOL_516_251 = 516,                  /* $@251  */
  YYSYMBOL_517_252 = 517,                  /* $@252  */
  YYSYMBOL_518_253 = 518,                  /* $@253  */
  YYSYMBOL_519_254 = 519,                  /* $@254  */
  YYSYMBOL_520_255 = 520,                  /* $@255  */
  YYSYMBOL_521_256 = 521,                  /* $@256  */
  YYSYMBOL_522_257 = 522,                  /* $@257  */
  YYSYMBOL_523_258 = 523,                  /* $@258  */
  YYSYMBOL_524_259 = 524,                  /* $@259  */
  YYSYMBOL_525_260 = 525,                  /* $@260  */
  YYSYMBOL_526_261 = 526,                  /* $@261  */
  YYSYMBOL_527_262 = 527,                  /* $@262  */
  YYSYMBOL_528_263 = 528,                  /* $@263  */
  YYSYMBOL_529_264 = 529,                  /* $@264  */
  YYSYMBOL_530_265 = 530,                  /* $@265  */
  YYSYMBOL_531_266 = 531,                  /* $@266  */
  YYSYMBOL_532_267 = 532,                  /* $@267  */
  YYSYMBOL_533_268 = 533,                  /* $@268  */
  YYSYMBOL_534_269 = 534,                  /* $@269  */
  YYSYMBOL_535_270 = 535,                  /* $@270  */
  YYSYMBOL_536_271 = 536,                  /* $@271  */
  YYSYMBOL_537_272 = 537,                  /* $@272  */
  YYSYMBOL_538_273 = 538,                  /* $@273  */
  YYSYMBOL_539_274 = 539,                  /* $@274  */
  YYSYMBOL_540_275 = 540,                  /* $@275  */
  YYSYMBOL_541_276 = 541,                  /* $@276  */
  YYSYMBOL_542_277 = 542,                  /* $@277  */
  YYSYMBOL_543_278 = 543,                  /* $@278  */
  YYSYMBOL_544_279 = 544,                  /* $@279  */
  YYSYMBOL_545_280 = 545,                  /* $@280  */
  YYSYMBOL_546_281 = 546,                  /* $@281  */
  YYSYMBOL_547_282 = 547,                  /* $@282  */
  YYSYMBOL_548_283 = 548,                  /* $@283  */
  YYSYMBOL_549_284 = 549,                  /* $@284  */
  YYSYMBOL_550_285 = 550,                  /* $@285  */
  YYSYMBOL_551_286 = 551,                  /* $@286  */
  YYSYMBOL_552_287 = 552,                  /* $@287  */
  YYSYMBOL_553_288 = 553,                  /* $@288  */
  YYSYMBOL_554_289 = 554,                  /* $@289  */
  YYSYMBOL_555_290 = 555,                  /* $@290  */
  YYSYMBOL_556_291 = 556,                  /* $@291  */
  YYSYMBOL_557_292 = 557,                  /* $@292  */
  YYSYMBOL_558_293 = 558,                  /* $@293  */
  YYSYMBOL_559_294 = 559,                  /* $@294  */
  YYSYMBOL_560_295 = 560,                  /* $@295  */
  YYSYMBOL_561_296 = 561,                  /* $@296  */
  YYSYMBOL_562_297 = 562,                  /* $@297  */
  YYSYMBOL_563_298 = 563,                  /* $@298  */
  YYSYMBOL_564_299 = 564,                  /* $@299  */
  YYSYMBOL_565_300 = 565,                  /* $@300  */
  YYSYMBOL_566_301 = 566,                  /* $@301  */
  YYSYMBOL_567_302 = 567,                  /* $@302  */
  YYSYMBOL_568_303 = 568,                  /* $@303  */
  YYSYMBOL_569_304 = 569,                  /* $@304  */
  YYSYMBOL_570_305 = 570,                  /* $@305  */
  YYSYMBOL_571_306 = 571,                  /* $@306  */
  YYSYMBOL_572_307 = 572,                  /* $@307  */
  YYSYMBOL_573_308 = 573,                  /* $@308  */
  YYSYMBOL_574_309 = 574,                  /* $@309  */
  YYSYMBOL_575_310 = 575,                  /* $@310  */
  YYSYMBOL_576_311 = 576,                  /* $@311  */
  YYSYMBOL_577_312 = 577,                  /* $@312  */
  YYSYMBOL_578_313 = 578,                  /* $@313  */
  YYSYMBOL_579_314 = 579,                  /* $@314  */
  YYSYMBOL_580_315 = 580,                  /* $@315  */
  YYSYMBOL_581_316 = 581,                  /* $@316  */
  YYSYMBOL_582_317 = 582,                  /* $@317  */
  YYSYMBOL_583_318 = 583,                  /* $@318  */
  YYSYMBOL_584_319 = 584,                  /* $@319  */
  YYSYMBOL_585_320 = 585,                  /* $@320  */
  YYSYMBOL_586_321 = 586,                  /* $@321  */
  YYSYMBOL_587_322 = 587,                  /* $@322  */
  YYSYMBOL_588_323 = 588,                  /* $@323  */
  YYSYMBOL_589_324 = 589,                  /* $@324  */
  YYSYMBOL_590_325 = 590,                  /* $@325  */
  YYSYMBOL_591_326 = 591,                  /* $@326  */
  YYSYMBOL_592_327 = 592,                  /* $@327  */
  YYSYMBOL_593_328 = 593,                  /* $@328  */
  YYSYMBOL_594_329 = 594,                  /* $@329  */
  YYSYMBOL_595_330 = 595,                  /* $@330  */
  YYSYMBOL_596_331 = 596,                  /* $@331  */
  YYSYMBOL_597_332 = 597,                  /* $@332  */
  YYSYMBOL_598_333 = 598,                  /* $@333  */
  YYSYMBOL_599_334 = 599,                  /* $@334  */
  YYSYMBOL_600_335 = 600,                  /* $@335  */
  YYSYMBOL_601_336 = 601,                  /* $@336  */
  YYSYMBOL_602_337 = 602,                  /* $@337  */
  YYSYMBOL_603_338 = 603,                  /* $@338  */
  YYSYMBOL_604_339 = 604,                  /* $@339  */
  YYSYMBOL_605_340 = 605,                  /* $@340  */
  YYSYMBOL_606_341 = 606,                  /* $@341  */
  YYSYMBOL_607_342 = 607,                  /* $@342  */
  YYSYMBOL_608_343 = 608,                  /* $@343  */
  YYSYMBOL_609_344 = 609,                  /* $@344  */
  YYSYMBOL_610_345 = 610,                  /* $@345  */
  YYSYMBOL_611_346 = 611,                  /* $@346  */
  YYSYMBOL_612_347 = 612,                  /* $@347  */
  YYSYMBOL_613_348 = 613,                  /* $@348  */
  YYSYMBOL_614_349 = 614,                  /* $@349  */
  YYSYMBOL_615_350 = 615,                  /* $@350  */
  YYSYMBOL_616_351 = 616,                  /* $@351  */
  YYSYMBOL_617_352 = 617,                  /* $@352  */
  YYSYMBOL_618_353 = 618,                  /* $@353  */
  YYSYMBOL_619_354 = 619,                  /* $@354  */
  YYSYMBOL_620_355 = 620,                  /* $@355  */
  YYSYMBOL_621_356 = 621,                  /* $@356  */
  YYSYMBOL_622_357 = 622,                  /* $@357  */
  YYSYMBOL_623_358 = 623,                  /* $@358  */
  YYSYMBOL_624_359 = 624,                  /* $@359  */
  YYSYMBOL_625_360 = 625,                  /* $@360  */
  YYSYMBOL_626_361 = 626,                  /* $@361  */
  YYSYMBOL_627_362 = 627,                  /* $@362  */
  YYSYMBOL_628_363 = 628,                  /* $@363  */
  YYSYMBOL_629_364 = 629,                  /* $@364  */
  YYSYMBOL_630_365 = 630,                  /* $@365  */
  YYSYMBOL_631_366 = 631,                  /* $@366  */
  YYSYMBOL_632_367 = 632,                  /* $@367  */
  YYSYMBOL_633_368 = 633,                  /* $@368  */
  YYSYMBOL_634_369 = 634,                  /* $@369  */
  YYSYMBOL_635_370 = 635,                  /* $@370  */
  YYSYMBOL_636_371 = 636,                  /* $@371  */
  YYSYMBOL_637_372 = 637,                  /* $@372  */
  YYSYMBOL_638_373 = 638,                  /* $@373  */
  YYSYMBOL_639_374 = 639,                  /* $@374  */
  YYSYMBOL_640_375 = 640,                  /* $@375  */
  YYSYMBOL_641_376 = 641,                  /* $@376  */
  YYSYMBOL_642_377 = 642,                  /* $@377  */
  YYSYMBOL_643_378 = 643,                  /* $@378  */
  YYSYMBOL_644_379 = 644,                  /* $@379  */
  YYSYMBOL_645_380 = 645,                  /* $@380  */
  YYSYMBOL_646_381 = 646,                  /* $@381  */
  YYSYMBOL_647_382 = 647,                  /* $@382  */
  YYSYMBOL_648_383 = 648,                  /* $@383  */
  YYSYMBOL_649_384 = 649,                  /* $@384  */
  YYSYMBOL_650_385 = 650,                  /* $@385  */
  YYSYMBOL_651_386 = 651,                  /* $@386  */
  YYSYMBOL_652_387 = 652,                  /* $@387  */
  YYSYMBOL_653_388 = 653,                  /* $@388  */
  YYSYMBOL_654_389 = 654,                  /* $@389  */
  YYSYMBOL_655_390 = 655,                  /* $@390  */
  YYSYMBOL_656_391 = 656,                  /* $@391  */
  YYSYMBOL_657_392 = 657,                  /* $@392  */
  YYSYMBOL_658_393 = 658,                  /* $@393  */
  YYSYMBOL_659_394 = 659,                  /* $@394  */
  YYSYMBOL_660_395 = 660,                  /* $@395  */
  YYSYMBOL_661_396 = 661,                  /* $@396  */
  YYSYMBOL_662_397 = 662,                  /* $@397  */
  YYSYMBOL_663_398 = 663,                  /* $@398  */
  YYSYMBOL_664_399 = 664,                  /* $@399  */
  YYSYMBOL_label_dest = 665,               /* label_dest  */
  YYSYMBOL_666_400 = 666,                  /* $@400  */
  YYSYMBOL_bam_const = 667,                /* bam_const  */
  YYSYMBOL_bam_const_table_entry = 668,    /* bam_const_table_entry  */
  YYSYMBOL_bam_const_table = 669,          /* bam_const_table  */
  YYSYMBOL_bam_functor = 670,              /* bam_functor  */
  YYSYMBOL_671_401 = 671,                  /* $@401  */
  YYSYMBOL_bam_functor_table_entry = 672,  /* bam_functor_table_entry  */
  YYSYMBOL_bam_functor_table = 673,        /* bam_functor_table  */
  YYSYMBOL_bam_binding = 674,              /* bam_binding  */
  YYSYMBOL_type_id = 675,                  /* type_id  */
  YYSYMBOL_pseudo_term = 676,              /* pseudo_term  */
  YYSYMBOL_677_402 = 677,                  /* $@402  */
  YYSYMBOL_bam_integer_set = 678,          /* bam_integer_set  */
  YYSYMBOL_integer_set_item = 679,         /* integer_set_item  */
  YYSYMBOL_integer_set = 680,              /* integer_set  */
  YYSYMBOL_constant_entry = 681,           /* constant_entry  */
  YYSYMBOL_682_403 = 682,                  /* $@403  */
  YYSYMBOL_683_404 = 683,                  /* $@404  */
  YYSYMBOL_684_405 = 684,                  /* $@405  */
  YYSYMBOL_685_406 = 685,                  /* $@406  */
  YYSYMBOL_constant_table = 686,           /* constant_table  */
  YYSYMBOL_functor_arity = 687,            /* functor_arity  */
  YYSYMBOL_688_407 = 688,                  /* $@407  */
  YYSYMBOL_689_408 = 689,                  /* $@408  */
  YYSYMBOL_690_409 = 690,                  /* $@409  */
  YYSYMBOL_691_410 = 691,                  /* $@410  */
  YYSYMBOL_692_411 = 692,                  /* $@411  */
  YYSYMBOL_functor_entry = 693,            /* functor_entry  */
  YYSYMBOL_694_412 = 694,                  /* $@412  */
  YYSYMBOL_functor_table = 695,            /* functor_table  */
  YYSYMBOL_feat_const = 696,               /* feat_const  */
  YYSYMBOL_feat_const_list_entry = 697,    /* feat_const_list_entry  */
  YYSYMBOL_feat_const_list = 698,          /* feat_const_list  */
  YYSYMBOL_fd_entry = 699,                 /* fd_entry  */
  YYSYMBOL_fd_instr = 700                  /* fd_instr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1757

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  262
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  439
/* YYNRULES -- Number of rules.  */
#define YYNRULES  714
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1927

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   516


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   273,   273,   275,   282,   284,   280,   293,   295,   297,
     291,   306,   307,   312,   314,   316,   318,   320,   322,   324,
     311,   330,   329,   335,   334,   340,   339,   345,   344,   350,
     352,   354,   349,   359,   361,   363,   358,   368,   370,   367,
     374,   377,   380,   383,   387,   386,   391,   395,   394,   400,
     402,   399,   407,   409,   406,   414,   416,   413,   421,   423,
     420,   428,   430,   427,   435,   437,   434,   442,   444,   441,
     449,   451,   448,   455,   458,   462,   464,   461,   469,   471,
     468,   476,   478,   475,   482,   486,   488,   485,   493,   495,
     492,   500,   502,   505,   507,   499,   511,   513,   516,   518,
     521,   523,   510,   527,   529,   532,   534,   537,   539,   542,
     544,   526,   548,   550,   553,   555,   558,   560,   563,   565,
     568,   570,   547,   574,   576,   573,   581,   583,   580,   588,
     590,   587,   595,   597,   594,   602,   604,   601,   609,   608,
     614,   613,   619,   621,   623,   618,   628,   630,   635,   627,
     645,   648,   652,   651,   657,   656,   662,   661,   667,   669,
     666,   674,   676,   678,   673,   683,   685,   682,   690,   689,
     695,   694,   700,   702,   699,   707,   709,   706,   714,   716,
     713,   721,   723,   720,   728,   730,   733,   735,   727,   739,
     741,   744,   746,   749,   751,   738,   755,   757,   760,   762,
     765,   767,   770,   772,   754,   776,   778,   781,   783,   786,
     788,   791,   793,   796,   798,   775,   802,   804,   801,   809,
     811,   808,   816,   818,   815,   823,   822,   828,   830,   832,
     827,   837,   839,   844,   836,   855,   854,   860,   862,   864,
     859,   869,   871,   868,   875,   879,   878,   884,   883,   889,
     888,   894,   893,   899,   898,   904,   903,   909,   908,   913,
     916,   920,   922,   919,   927,   930,   926,   935,   938,   934,
     942,   946,   948,   950,   945,   954,   953,   959,   958,   965,
     967,   966,   971,   973,   970,   976,   978,   975,   981,   983,
     980,   987,   989,   986,   993,   995,   997,   992,  1001,  1003,
    1000,  1007,  1009,  1006,  1013,  1015,  1012,  1019,  1021,  1023,
    1025,  1027,  1029,  1018,  1034,  1036,  1039,  1033,  1042,  1044,
    1046,  1049,  1041,  1052,  1054,  1056,  1059,  1051,  1062,  1064,
    1061,  1068,  1070,  1072,  1067,  1076,  1078,  1081,  1075,  1084,
    1086,  1089,  1083,  1092,  1094,  1097,  1091,  1100,  1102,  1104,
    1107,  1099,  1110,  1112,  1114,  1117,  1109,  1120,  1122,  1119,
    1126,  1128,  1130,  1125,  1134,  1136,  1139,  1133,  1142,  1144,
    1147,  1141,  1150,  1152,  1154,  1149,  1158,  1160,  1162,  1157,
    1166,  1168,  1165,  1172,  1174,  1176,  1171,  1180,  1182,  1185,
    1179,  1188,  1190,  1193,  1187,  1196,  1198,  1200,  1195,  1207,
    1209,  1206,  1216,  1218,  1220,  1215,  1227,  1229,  1226,  1233,
    1235,  1237,  1232,  1241,  1243,  1240,  1246,  1248,  1245,  1253,
    1255,  1252,  1258,  1260,  1257,  1265,  1264,  1269,  1271,  1268,
    1275,  1277,  1274,  1281,  1283,  1280,  1287,  1289,  1291,  1286,
    1295,  1297,  1294,  1301,  1303,  1300,  1307,  1309,  1306,  1313,
    1315,  1312,  1319,  1321,  1323,  1318,  1329,  1331,  1333,  1335,
    1328,  1341,  1343,  1345,  1347,  1340,  1353,  1355,  1357,  1359,
    1361,  1352,  1367,  1369,  1371,  1366,  1377,  1379,  1381,  1383,
    1376,  1389,  1391,  1388,  1395,  1397,  1399,  1394,  1404,  1406,
    1408,  1403,  1413,  1415,  1417,  1419,  1412,  1424,  1426,  1423,
    1430,  1432,  1434,  1429,  1439,  1441,  1438,  1445,  1447,  1449,
    1444,  1454,  1456,  1458,  1453,  1463,  1465,  1467,  1469,  1462,
    1474,  1476,  1473,  1480,  1482,  1484,  1479,  1488,  1490,  1489,
    1493,  1495,  1494,  1498,  1500,  1499,  1504,  1503,  1507,  1509,
    1508,  1513,  1512,  1517,  1519,  1516,  1522,  1525,  1528,  1532,
    1531,  1536,  1535,  1540,  1539,  1544,  1543,  1548,  1547,  1554,
    1553,  1559,  1562,  1565,  1569,  1568,  1574,  1575,  1578,  1582,
    1581,  1587,  1590,  1593,  1596,  1597,  1599,  1598,  1603,  1602,
    1608,  1607,  1613,  1616,  1618,  1617,  1622,  1621,  1626,  1625,
    1630,  1632,  1629,  1636,  1638,  1635,  1642,  1641,  1646,  1645,
    1650,  1649,  1654,  1656,  1653,  1662,  1664,  1661,  1669,  1672,
    1675,  1678,  1679,  1680,  1682,  1684,  1681,  1688,  1695,  1694,
    1697,  1703,  1706,  1712,  1718,  1719,  1725,  1724,  1732,  1738,
    1739,  1743,  1746,  1749,  1752,  1758,  1759,  1760,  1764,  1767,
    1770,  1773,  1777,  1776,  1780,  1783,  1790,  1797,  1815,  1816,
    1822,  1824,  1821,  1828,  1830,  1827,  1836,  1837,  1842,  1844,
    1841,  1848,  1853,  1858,  1847,  1868,  1867,  1873,  1874,  1878,
    1882,  1890,  1896,  1897,  1902,  1904,  1908,  1910,  1912,  1914,
    1916,  1918,  1920,  1922,  1924,  1926,  1928,  1930,  1932,  1934,
    1936,  1938,  1940,  1942,  1944,  1946,  1948,  1950,  1952,  1954,
    1956,  1958,  1960,  1962,  1964,  1966,  1968,  1970,  1972,  1974,
    1976,  1978,  1980,  1982,  1987
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_SWITCH_ON_TERM",
  "TOKEN_TRY_SINGLE", "TOKEN_TRY", "TOKEN_RETRY", "TOKEN_TRUST",
  "TOKEN_GUARD_COND", "TOKEN_GUARD_COMMIT", "TOKEN_GUARD_WAIT",
  "TOKEN_GUARD_UNORDER", "TOKEN_GUARD_ORDER", "TOKEN_GUARD_UNIT",
  "TOKEN_GUARD_CUT", "TOKEN_CALL", "TOKEN_ALLOCATE", "TOKEN_DEALLOCATE",
  "TOKEN_META_CALL", "TOKEN_EXTERN_EXECUTE", "TOKEN_EXECUTE",
  "TOKEN_DEALLOC_EXECUTE", "TOKEN_EXTERN_CALL", "TOKEN_CALL_APPLY",
  "TOKEN_EXECUTE_APPLY", "TOKEN_DEALLOC_EXECUTE_APPLY", "TOKEN_PROCEED",
  "TOKEN_DEALLOC_PROCEED", "TOKEN_FAIL", "TOKEN_GET_X_VARIABLE",
  "TOKEN_GET_Y_VARIABLE", "TOKEN_GET2_Y_VARIABLE", "TOKEN_GET3_Y_VARIABLE",
  "TOKEN_GET4_Y_VARIABLE", "TOKEN_GET5_Y_VARIABLE", "TOKEN_GET_X_VALUE",
  "TOKEN_GET_Y_VALUE", "TOKEN_GET_CONSTANT", "TOKEN_GET_NIL",
  "TOKEN_GET_STRUCTURE", "TOKEN_GET_LIST", "TOKEN_GET_LIST_X0",
  "TOKEN_GET_NIL_X0", "TOKEN_GET_STRUCTURE_X0", "TOKEN_GET_CONSTANT_X0",
  "TOKEN_GET_ABSTRACTION", "TOKEN_PUT_X_VOID", "TOKEN_PUT_Y_VOID",
  "TOKEN_PUT_X_VARIABLE", "TOKEN_PUT_Y_VARIABLE", "TOKEN_PUT_X_VALUE",
  "TOKEN_PUT_Y_VALUE", "TOKEN_PUT2_Y_VALUE", "TOKEN_PUT3_Y_VALUE",
  "TOKEN_PUT4_Y_VALUE", "TOKEN_PUT5_Y_VALUE", "TOKEN_PUT_CONSTANT",
  "TOKEN_PUT_NIL", "TOKEN_PUT_STRUCTURE", "TOKEN_PUT_LIST",
  "TOKEN_UNIFY_VOID", "TOKEN_PUT_ABSTRACTION", "TOKEN_UNIFY_Y_VARIABLE",
  "TOKEN_UNIFY_X_VARIABLE", "TOKEN_UNIFY_X_VALUE", "TOKEN_UNIFY_Y_VALUE",
  "TOKEN_UNIFY_CONSTANT", "TOKEN_UNIFY_NIL", "TOKEN_UNIFY_LIST",
  "TOKEN_UNIFY_STRUCTURE", "TOKEN_SWITCH_ON_CONSTANT",
  "TOKEN_SWITCH_ON_STRUCTURE", "TOKEN_SEND3", "TOKEN_SUSPEND_FLAT",
  "TOKEN_ATOM_SWITCH", "TOKEN_FAIL_DEC", "TOKEN_LOAD_NIL",
  "TOKEN_LOAD_ATOM", "TOKEN_LOAD_INTEGER", "TOKEN_READ_CAR",
  "TOKEN_READ_CDR", "TOKEN_READ_ARG", "TOKEN_SET_REG_REG",
  "TOKEN_SET_REG_YVAR", "TOKEN_SET_YVAR_REG", "TOKEN_TERM_SWITCH",
  "TOKEN_ATOM_WITH_ELSE_SWITCH", "TOKEN_ATOM_WITH_ATOM_ELSE_SWITCH",
  "TOKEN_ATOM_TYPE", "TOKEN_ATOM_TYPE_WITH_ELSE",
  "TOKEN_KNOWN_ATOM_SWITCH", "TOKEN_KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH",
  "TOKEN_INTEGER_SWITCH", "TOKEN_INTEGER_WITH_ELSE_SWITCH",
  "TOKEN_INTEGER_WITH_INTEGER_ELSE_SWITCH", "TOKEN_INTEGER_TYPE",
  "TOKEN_INTEGER_TYPE_WITH_ELSE", "TOKEN_KNOWN_INTEGER_SWITCH",
  "TOKEN_KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH", "TOKEN_LIST_SWITCH",
  "TOKEN_LIST_TYPE_WITH_ELSE", "TOKEN_STRUCT_TYPE",
  "TOKEN_STRUCT_TYPE_WITH_ELSE", "TOKEN_KNOWN_FUNCTOR_SWITCH",
  "TOKEN_KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH",
  "TOKEN_INT_COMPARE_REG_REG", "TOKEN_INT_COMPARE_REG_INT",
  "TOKEN_EQ_REG_REG", "TOKEN_MATCH_REG_REG", "TOKEN_MATCH_REG_REG_OFF",
  "TOKEN_MATCH_REG_ATOM", "TOKEN_MATCH_REG_OFF_ATOM",
  "TOKEN_MATCH_REG_INTEGER", "TOKEN_MATCH_REG_OFF_INTEGER",
  "TOKEN_MATCH_REG_NIL", "TOKEN_MATCH_REG_OFF_NIL", "TOKEN_MATCH_REG_YVAR",
  "TOKEN_MATCH_YVAR_YVAR", "TOKEN_MATCH_YVAR_REG_OFF",
  "TOKEN_MATCH_REG_H_LIST", "TOKEN_MATCH_YVAR_H_LIST",
  "TOKEN_MATCH_REG_H_STRUCT", "TOKEN_MATCH_YVAR_H_STRUCT",
  "TOKEN_MATCH_REG_STRUCT", "TOKEN_MATCH_REG_STRUCT_LEVEL",
  "TOKEN_MATCH_REG_OFF_STRUCT", "TOKEN_MATCH_REG_OFF_STRUCT_LEVEL",
  "TOKEN_MATCH_YVAR_STRUCT", "TOKEN_MATCH_YVAR_STRUCT_LEVEL",
  "TOKEN_MATCH_REG_LIST_CAR", "TOKEN_MATCH_REG_LIST_CAR_LEVEL",
  "TOKEN_MATCH_REG_OFF_LIST_CAR", "TOKEN_MATCH_REG_OFF_LIST_CAR_LEVEL",
  "TOKEN_MATCH_YVAR_LIST_CAR", "TOKEN_MATCH_YVAR_LIST_CAR_LEVEL",
  "TOKEN_MATCH_REG_LIST_CDR", "TOKEN_MATCH_REG_LIST_CDR_LEVEL",
  "TOKEN_MATCH_REG_OFF_LIST_CDR", "TOKEN_MATCH_REG_OFF_LIST_CDR_LEVEL",
  "TOKEN_MATCH_YVAR_LIST_CDR", "TOKEN_MATCH_YVAR_LIST_CDR_LEVEL",
  "TOKEN_PUSH_LIST_IMM", "TOKEN_PUSH_LIST", "TOKEN_PUSH_STRUCTURE_IMM",
  "TOKEN_PUSH_STRUCTURE", "TOKEN_PUSH_NIL", "TOKEN_PUSH_REG",
  "TOKEN_PUSH_YVAR", "TOKEN_PUSH_VOID", "TOKEN_SET_REG_H_AND_PUSH_VOID",
  "TOKEN_SET_YVAR_H_AND_PUSH_VOID", "TOKEN_PUSH_INTEGER",
  "TOKEN_PUSH_FUNCTOR", "TOKEN_SET_REG_AND_YVAR_H_AND_PUSH_VOID",
  "TOKEN_PUSH_ATOM", "TOKEN_SET_REG_H", "TOKEN_SET_YVAR_H", "TOKEN_BTRY",
  "TOKEN_BTRUST", "TOKEN_TEST_TRY", "TOKEN_TEST_TRUST", "TOKEN_S_TRY",
  "TOKEN_S_TRUST", "TOKEN_BACK", "TOKEN_GEN_TRY", "TOKEN_GEN_TRUST",
  "TOKEN_BEXECUTE_SIMPLE", "TOKEN_BEXECUTE", "TOKEN_BCALL",
  "TOKEN_JUMP_GC_AND_DUMP_CONT", "TOKEN_JUMP_GC", "TOKEN_JUMP",
  "TOKEN_BPROCEED_SIMPLE", "TOKEN_BPROCEED", "TOKEN_CREATE_CONT",
  "TOKEN_TEST_LEVEL", "TOKEN_CONSTRAINT_STACK_TEST",
  "TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK", "TOKEN_ALIAS_CHECK",
  "TOKEN_ALIAS_CHECK_REG", "TOKEN_FAIL_ON_ALIAS", "TOKEN_CHOICE_QUIET_CUT",
  "TOKEN_SUSPEND_LONE_VAR", "TOKEN_BSUSPEND", "TOKEN_SUSPEND_WITHOUT_REG",
  "TOKEN_CREATE_GUARD", "TOKEN_FLAT_QUIET_CUT", "TOKEN_FLAT_QUIET_COMMIT",
  "TOKEN_FLAT_NOISY_WAIT", "TOKEN_SHORTCUT_QUIET_CUT",
  "TOKEN_SHORTCUT_QUIET_COMMIT", "TOKEN_SHORTCUT_NOISY_WAIT",
  "TOKEN_DEEP_QUIET_CUT", "TOKEN_DEEP_QUIET_COMMIT",
  "TOKEN_DEEP_NOISY_WAIT", "PREDICATE", "ATOMNAME", "NEWLINE", "LPAR",
  "RPAR", "LHPAR", "RHPAR", "COMMA", "DASH", "SLASH", "LABEL", "LABELTAG",
  "NUMBER", "FLOAT", "END", "PLABEL", "CONSTANT", "LIST", "STRUCT",
  "TYPED", "INTEGER", "ATOM", "FUNCTOR", "REG", "VOID", "LABELED",
  "TOKEN_GET_RECORD", "TOKEN_PUT_RECORD", "FD_code_T", "FD_const_T",
  "FD_max_elem_T", "FD_add_T", "FD_sub_T", "FD_mult_T", "FD_divd_T",
  "FD_divu_T", "FD_mod_T", "FD_val_T", "FD_val_0_T", "FD_val_1_T",
  "FD_val_2_T", "FD_min_T", "FD_max_T", "FD_dom_min_T", "FD_dom_max_T",
  "FD_dom_min_0_T", "FD_dom_max_0_T", "FD_dom_min_1_T", "FD_dom_max_1_T",
  "FD_dom_min_2_T", "FD_dom_max_2_T", "FD_dom_T", "FD_dom_0_T",
  "FD_dom_1_T", "FD_dom_2_T", "FD_range_T", "FD_outer_range_T",
  "FD_setadd_T", "FD_setsub_T", "FD_setmod_T", "FD_compl_T", "FD_union_T",
  "FD_inter_T", "FD_check_T", "FD_label_T", "FD_element_i_T",
  "FD_element_x_T", "$accept", "definitions", "$@1", "$@2", "$@3", "$@4",
  "$@5", "code", "instruction", "$@6", "$@7", "$@8", "$@9", "$@10", "$@11",
  "$@12", "$@13", "$@14", "$@15", "$@16", "$@17", "$@18", "$@19", "$@20",
  "$@21", "$@22", "$@23", "$@24", "$@25", "$@26", "$@27", "$@28", "$@29",
  "$@30", "$@31", "$@32", "$@33", "$@34", "$@35", "$@36", "$@37", "$@38",
  "$@39", "$@40", "$@41", "$@42", "$@43", "$@44", "$@45", "$@46", "$@47",
  "$@48", "$@49", "$@50", "$@51", "$@52", "$@53", "$@54", "$@55", "$@56",
  "$@57", "$@58", "$@59", "$@60", "$@61", "$@62", "$@63", "$@64", "$@65",
  "$@66", "$@67", "$@68", "$@69", "$@70", "$@71", "$@72", "$@73", "$@74",
  "$@75", "$@76", "$@77", "$@78", "$@79", "$@80", "$@81", "$@82", "$@83",
  "$@84", "$@85", "$@86", "$@87", "$@88", "$@89", "$@90", "$@91", "$@92",
  "$@93", "$@94", "$@95", "$@96", "$@97", "$@98", "$@99", "$@100", "$@101",
  "$@102", "$@103", "$@104", "$@105", "$@106", "$@107", "$@108", "$@109",
  "$@110", "$@111", "$@112", "$@113", "$@114", "$@115", "$@116", "$@117",
  "$@118", "$@119", "$@120", "$@121", "$@122", "$@123", "$@124", "$@125",
  "$@126", "$@127", "$@128", "$@129", "$@130", "$@131", "$@132", "$@133",
  "$@134", "$@135", "$@136", "$@137", "$@138", "$@139", "$@140", "$@141",
  "$@142", "$@143", "$@144", "$@145", "$@146", "$@147", "$@148", "$@149",
  "$@150", "$@151", "$@152", "$@153", "$@154", "$@155", "$@156", "$@157",
  "$@158", "$@159", "$@160", "$@161", "$@162", "$@163", "$@164", "$@165",
  "$@166", "$@167", "$@168", "$@169", "$@170", "$@171", "$@172", "$@173",
  "$@174", "$@175", "$@176", "$@177", "$@178", "$@179", "$@180", "$@181",
  "$@182", "$@183", "$@184", "$@185", "$@186", "$@187", "$@188", "$@189",
  "$@190", "$@191", "$@192", "$@193", "$@194", "$@195", "$@196", "$@197",
  "$@198", "$@199", "$@200", "$@201", "$@202", "$@203", "$@204", "$@205",
  "$@206", "$@207", "$@208", "$@209", "$@210", "$@211", "$@212", "$@213",
  "$@214", "$@215", "$@216", "$@217", "$@218", "$@219", "$@220", "$@221",
  "$@222", "$@223", "$@224", "$@225", "$@226", "$@227", "$@228", "$@229",
  "$@230", "$@231", "$@232", "$@233", "$@234", "$@235", "$@236", "$@237",
  "$@238", "$@239", "$@240", "$@241", "$@242", "$@243", "$@244", "$@245",
  "$@246", "$@247", "$@248", "$@249", "$@250", "$@251", "$@252", "$@253",
  "$@254", "$@255", "$@256", "$@257", "$@258", "$@259", "$@260", "$@261",
  "$@262", "$@263", "$@264", "$@265", "$@266", "$@267", "$@268", "$@269",
  "$@270", "$@271", "$@272", "$@273", "$@274", "$@275", "$@276", "$@277",
  "$@278", "$@279", "$@280", "$@281", "$@282", "$@283", "$@284", "$@285",
  "$@286", "$@287", "$@288", "$@289", "$@290", "$@291", "$@292", "$@293",
  "$@294", "$@295", "$@296", "$@297", "$@298", "$@299", "$@300", "$@301",
  "$@302", "$@303", "$@304", "$@305", "$@306", "$@307", "$@308", "$@309",
  "$@310", "$@311", "$@312", "$@313", "$@314", "$@315", "$@316", "$@317",
  "$@318", "$@319", "$@320", "$@321", "$@322", "$@323", "$@324", "$@325",
  "$@326", "$@327", "$@328", "$@329", "$@330", "$@331", "$@332", "$@333",
  "$@334", "$@335", "$@336", "$@337", "$@338", "$@339", "$@340", "$@341",
  "$@342", "$@343", "$@344", "$@345", "$@346", "$@347", "$@348", "$@349",
  "$@350", "$@351", "$@352", "$@353", "$@354", "$@355", "$@356", "$@357",
  "$@358", "$@359", "$@360", "$@361", "$@362", "$@363", "$@364", "$@365",
  "$@366", "$@367", "$@368", "$@369", "$@370", "$@371", "$@372", "$@373",
  "$@374", "$@375", "$@376", "$@377", "$@378", "$@379", "$@380", "$@381",
  "$@382", "$@383", "$@384", "$@385", "$@386", "$@387", "$@388", "$@389",
  "$@390", "$@391", "$@392", "$@393", "$@394", "$@395", "$@396", "$@397",
  "$@398", "$@399", "label_dest", "$@400", "bam_const",
  "bam_const_table_entry", "bam_const_table", "bam_functor", "$@401",
  "bam_functor_table_entry", "bam_functor_table", "bam_binding", "type_id",
  "pseudo_term", "$@402", "bam_integer_set", "integer_set_item",
  "integer_set", "constant_entry", "$@403", "$@404", "$@405", "$@406",
  "constant_table", "functor_arity", "$@407", "$@408", "$@409", "$@410",
  "$@411", "functor_entry", "$@412", "functor_table", "feat_const",
  "feat_const_list_entry", "feat_const_list", "fd_entry", "fd_instr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-1501)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
   -1501,    13, -1501,  -175, -1501,  -141, -1501, -1501,  -163,  -131,
    -136,  -130, -1501, -1501, -1501,   -82,   282,   -42,   -73,   -34,
     -31,   -30,   -29, -1501, -1501, -1501,   -28,   -27,   -26, -1501,
     -25,   -24, -1501,   -23,   -22,   -21,   -20,   -19,   -18,   -17,
     -16, -1501, -1501, -1501,   -15,   -14,   -13,   -12,   -11,   -10,
      -9,    -8,    -7,    -6,    -5,    -4, -1501, -1501,    -3,    -2,
      -1,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    11,    12,    14,    15, -1501,    16,    17,    18,    19,
      20,    21, -1501, -1501,    22,    23,    25, -1501,    26,    27,
   -1501,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      82,    83,    84,   279,   280,   281,   284,   285,   286,   287,
     288,   290,   291,   292,   293,   298, -1501,   299, -1501,   300,
   -1501,   308,   309, -1501,   310,   311,   312,   318,   319,   320,
     325,   326,   331,   332,   333,   334,   335,   336,   337,   338,
   -1501,   339,   340,   341,   342,   343,   344, -1501, -1501,   345,
     346,   347,   348, -1501,   349,   350,   353,   354,   355,   356,
     359,   360,   365,   366,   369,   370,   371, -1501, -1501, -1501,
   -1501,   372,   373,   376,   377,   381,   385,   388, -1501,   389,
     390,   391,   586, -1501, -1501, -1501,   273,   578,   579,   580,
     582,   583,   584,   585,  -140,   588,   590,   -56,  -139,  -135,
     597,   591,   596,   598,   606,   607,   608,   609,   610,   611,
     612,   613,  -161,   614,   600,   615,   627,  -156,  -133,   617,
     618,   619,   620,   621,   622,   623,   624,   625,   626,  -148,
     628,   638,   629,   -79,   630,   631,   632,   633,  -143,   645,
     635,   636,   637,   639,   640,   641,   642,   643,   644,   646,
     647,   648,   649,   650,   651,   652,   653,   654,   655,   656,
     658,   659,   660,   661,   662,   663,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   675,   676,   679,   680,   681,
     682,   683,   684,   685,   686,   687,   688,   689,   690,   691,
     692,   693,   694,   695,   696,   697,   698,   699,   700,   701,
     704,   705,   706,   707,   708,   709,   710,   711,   712,   713,
     714,   715,   716,   717,   718,   -78,   730,   720,   -78,   721,
     722,   -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,   730,
     730,   730,   -61,   -61,   -61,   723,   724,   -61,   -61,   725,
     726,   727,   728,   729,   -61,   731,   -61,   -61,   -61,   -61,
     -61,   -61,   732,   733,   -78,   -61,   -61,   730,   734,   -61,
     741,   746,   736, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,   605, -1501,
     745, -1501,   747, -1501, -1501, -1501,   749,   743,   748, -1501,
   -1501,   750,   752,   753,   751,   756,   757,   755,   759,   760,
     761, -1501, -1501,   762,   763, -1501, -1501, -1501, -1501, -1501,
     764, -1501,   765,   766,   767,   771,   772,   773, -1501, -1501,
     774,   775,   776,   777, -1501,   778, -1501, -1501, -1501,   589,
     779,   780,   781,   783,   784,   782,   785,   786,   744,   787,
     791,   793,   789,   790,   794,   797,   799,   800,   795,   804,
     806,   807,   808,   809,   810,   811,   812,   813,   814,   815,
     816,   817,   796,   818,   821,   819,   822,   825,   826,   823,
     824,   829,   830,   831,   832,   833,   836,   837,   838,   839,
     840,   841,   842,   843,   847,   828,   850,   846,   849,   853,
     854,   855,   856,   857,   859,   861,   858,   862,   863,   864,
     865,   869,   867,   868,   870,   871,   872,   873,   874,   875,
     876,   877,   878,   879,   880,   881,   882,   883,   884,   885,
     886,   887,   888,   889,   890,   891,   892,   893,   894,   895,
     896,   897,   898,   899,   900,   901,   902,   903,   904,   908,
     906,   907,   909,   910,   911,   912,   913,   914,   915,   916,
     917,   918,   919,   920,   921,   922,   923,   924,   925,   926,
     927,   928,   929,   930,   931,   932,   936,   937,   938,   939,
     940,   941, -1501,   942, -1501,   943, -1501,   944,   947,   754,
     758,   934,   945,   946, -1501, -1501, -1501,   935, -1501, -1501,
     948, -1501, -1501, -1501,   950,   949,   951, -1501,   952,   953,
     954,   955,   957,   956,   961,   958,   959,   960, -1501, -1501,
   -1501,   962,   966, -1501, -1501, -1501, -1501,   963, -1501,   967,
     968,   969, -1501,   965, -1501, -1501, -1501, -1501,   970,   971,
     972,   973,   974, -1501, -1501,   975,   976,   977,   978,   979,
     980,   981,   982,   983,   984,   985,   986,   987,   989,   990,
     991,   992,   993,   994,   995, -1501,   998, -1501,   999, -1501,
   -1501, -1501,  1000,  1001, -1501, -1501,  1002,  1003,  1004,  1005,
    1006,  1007,  1008,  1009,  1010,  1011,  1012, -1501,  1013, -1501,
    1014,  1015, -1501, -1501, -1501, -1501, -1501, -1501, -1501,  1016,
     964,  1024,  1018,  1019, -1501,   -78,   -78,  1020,  1021,  1022,
    1023,  1025,  1026,   -61,   -61,   -61,   -61,   -61,  1027,  1028,
    1029,   -61,   -61,   -61,   -61,  1030,  1031,   -61,   -61,   -61,
     -61,  1032,  1033,  1034,   -78,  1035,  1036,  1037,   -78,  1038,
     -78,  1039, -1501,  1040,  1041,  1042,  1043,  1044,  1045,  1046,
    1047,   730,   730,  1049,  1050,   730,   730,   -61,   -61,  1051,
    1053,   -61,   -61,   -61,   -61,  1054,  1055,   -61,   -61, -1501,
   -1501, -1501, -1501, -1501, -1501,  1056,  1057, -1501, -1501, -1501,
   -1501, -1501,   -61,   -61, -1501, -1501, -1501,   -61, -1501,  -193,
   -1501, -1501,   -61,   -61, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501,  1058,  1059,  -145, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
     788,  1069, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501,  1066, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,  1067,
    1068, -1501, -1501,  1070,  1071,  1073,  1074, -1501, -1501,  1075,
    1077,  1079,  1080, -1501, -1501,  1081,  1082, -1501, -1501,  1072,
    1083,  1084,  1088,  1089,   860,  1086,  1091,  1090,  1093,  1094,
    1096,  1097,  1098,  1099,  1100,  1101,  1104,  1102,  1103,  1107,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501,  -145,  1105,  1106,  1108,  1110,  1112,  1113,
    1111,  1115,  1116,  1117,  1118,  1119,  1120,  1121,  1123,  1124,
    1125,  1126,  1127,  1128,  1129,  1133,  1134,  1135,  1136,  1137,
    1138,  1139,  1140,  1142,  1144,  1145,  1146,  1147,  1148,  1149,
    1150,  1151,  1155,  1156,  1157,  1158,  1159,  1160,  1163,   -77,
   -1501,  -132,  1122, -1501,  -126,  1161,  1095,  1165,  1166,  1167,
    1168,  1169,  1170,  1171,  1173,  1172,  1174,  1175,  1176,  1177,
    1154,  1178,  1179,  1181,  1182,  1186,  1184,  1185,  1187,  1189,
    1190,  1188,  1191,  1192,  1193,  1195,   -61,  1196,  1200,  1198,
    1202,  1201,  1203,  1204,  1205,  1206,  1208,  1207,  1209,  1211,
    1212,  1213,   -61,   -61,  1214,  1216,   -61,   -61,  1092,  1130,
    1217,  1218,  1132,  1141,  1215,  1219,  1221,  1222,  1220,  1223,
    1226, -1501,  -193,  -193, -1501, -1501,  1180,  1194, -1501,  1228,
     -78,   -61,   -61,   -92, -1501,  1224,  1225, -1501, -1501,  1227,
    1229, -1501,  1231,  1233, -1501,  1230,  1232,  1234, -1501, -1501,
   -1501,  1131, -1501, -1501, -1501, -1501,  1153, -1501, -1501, -1501,
   -1501, -1501,  1235,  1236,  1237,  1238, -1501, -1501, -1501, -1501,
   -1501,  1239, -1501,  1240, -1501, -1501, -1501, -1501, -1501,  1241,
    1242,  1243,  1244, -1501, -1501, -1501,  1245,  1246, -1501, -1501,
   -1501, -1501,  1252,   788, -1501,  1253,  1069,  1249,  1257, -1501,
   -1501, -1501, -1501,  1251, -1501, -1501, -1501,   -61,  1254,  1255,
   -1501,   -61,  1259,  1260,  1263,  1258,  1261, -1501,   -61,  1264,
    1266,   -61,   -61, -1501,   -61,  1267,  1269,   -61, -1501,   -61,
   -1501,  1265, -1501,   -78, -1501,   -78, -1501, -1501, -1501,  1268,
   -1501, -1501, -1501, -1501,  1271,  1272,   730,   730,  1274,  1275,
   -1501, -1501,   -61,   -61, -1501, -1501, -1501, -1501,   -61,   -61,
   -1501, -1501, -1501,  1279,  1280,  1273,  1273,  1273,  1282,  1283,
    1284, -1501, -1501, -1501,  1285, -1501, -1501, -1501, -1501,  1270,
    1287, -1501, -1501, -1501,  1286,  1289, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,  1288,  1290,
    1291, -1501,  -166,  1292, -1501, -1501,   -78, -1501, -1501, -1501,
   -1501, -1501,   -78,   -78,   -78, -1501, -1501, -1501,   -78,   -78,
   -1501, -1501, -1501,   730,   730, -1501,  1293, -1501, -1501,  1294,
    1295, -1501,  1296,  1297,  1298,  1299,  1300,  1301,  1303,  1304,
    1307,  1308,  1306,  1309,  1313,  1311,  1312,  1314,  1316,  1315,
   -1501, -1501, -1501,    10, -1501,  1317,  1319,  1320, -1501, -1501,
   -1501, -1501,  1323,  1324,  1210,  1321,   -76,   -76,  1325,  1326,
    1327, -1501, -1501,  1328,  1329,  1330,  1331,  1335,  1336,  1334,
    1337,  1338,  1339,  1343,  1344,  1322,  1340, -1501,  1341,  1342,
   -1501, -1501,  1345,  1348, -1501,   -75,  1346,  1349,  1350,  1351,
    1347,   -69,   -67,   -65,  1352,  1353,  1358,   -63,   -60,  1359,
    1360,  1361,  1362, -1501,   -58,   -55,  1363,   -61,  1364,  1368,
   -1501, -1501,  1369, -1501, -1501,   -61,   -61, -1501, -1501, -1501,
    1354,  1355,  1356, -1501,  1365, -1501,  1366,  1367,  1370, -1501,
    1371,  1372,  1273,  1376,  1377, -1501, -1501,  1318,  1379, -1501,
   -1501, -1501, -1501,   -53,   -51,  1373, -1501, -1501,  1374,  1375,
    1378,  1380, -1501, -1501,  1381,  1382,  1383,  1384, -1501, -1501,
   -1501, -1501,  1385, -1501, -1501,  1387, -1501,   -61, -1501,   -78,
   -1501,   -61,  1392,  1393, -1501, -1501, -1501, -1501,  1394,  1395,
   -1501, -1501, -1501, -1501, -1501, -1501,   -61, -1501,   730, -1501,
     -61,  1397,   -61, -1501, -1501,  1396,  1398,  1399,  1400,  1401,
    1402, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501,  1273,  1273,  1403,   -76,  1404, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501,  1408,  1409, -1501,
    1407,  1410, -1501, -1501,  1411, -1501, -1501,   -78,   -78,  1412,
    1414,  1415,   -78,   -78,  1416,  1417, -1501,  1418, -1501,  1419,
    1420, -1501,  1421,  1390,  1422,  1423,  1424,  1425,  1426,  1427,
    1428,  1432,  1429,  1436,  1437,  1435,  1439,  1438,  1440,  1433,
    1441,  1442,  1443,  1445,  1444,  1446,  1448,  1452,  1450,  1451,
    1453, -1501, -1501,  1447,  1449, -1501,  1455,   -49,   -47, -1501,
   -1501, -1501,   -45,   -43, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,  1454, -1501,
   -1501, -1501,  1456, -1501,  1459,  1460, -1501, -1501,  1457, -1501,
    1458,  1461,  1462, -1501,  1463,  1464,  1465,   -76, -1501,   -61,
   -1501, -1501, -1501, -1501,  1467,  1471,  1472,  1473,  1468,  1474,
   -1501, -1501, -1501, -1501,  1478,  1479, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501,   -41,  1480, -1501,  1481,  1482,  1483,  1484,
   -1501,  1477,  1485,  1486, -1501,  1487,  1488,  1489, -1501, -1501,
    1493,  1494,  1495,  1496,  1497,  1498,  1499, -1501, -1501,  1500,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
    1501,  1502,  1503,  1504,  1505,  1506,  1507,  1513,   -61,  1490,
    1491,  1514,  1492, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
    1510, -1501, -1501, -1501,  1511, -1501,  1517,  1508,  1518,  1519,
    1523,  1521,  1522, -1501,  1526, -1501,  1520, -1501,  1524,  1525,
   -1501,  1527,  1528,  1529, -1501,  1530, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501,  1531,  1534,  1535,  1536,  1537, -1501,  1533,
    1538,  1539,  1540, -1501, -1501, -1501, -1501,  1542,  1541,  1543,
    1546, -1501,  1544, -1501,  1545, -1501, -1501,  1547,  1548,  1549,
    1550, -1501, -1501,  1554,  1555, -1501, -1501
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,     3,     0,     4,     7,     0,     0,
       0,     0,     5,     8,    11,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,     0,     0,     0,    43,
       0,     0,    46,     0,     0,     0,     0,     0,     0,     0,
       0,    73,    74,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   151,   150,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   244,     0,     0,     0,     0,
       0,     0,   259,   260,     0,     0,     0,   270,     0,     0,
     279,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   527,     0,   530,     0,
     533,     0,     0,   538,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     566,     0,     0,     0,     0,     0,     0,   574,   575,     0,
       0,     0,     0,   583,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   611,   612,   613,
       6,     0,     0,     0,     0,     0,     0,     0,   644,     0,
       0,     0,     0,    12,   617,     9,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    11,    13,    21,    23,    25,    27,    29,
      33,    37,    49,    52,    44,    47,    70,    55,    58,    61,
      64,    67,    75,    78,    81,    85,    88,    91,    96,   103,
     112,   123,   126,   135,   129,   132,   138,   142,   140,   158,
     156,   152,   154,   161,   165,   168,   170,   172,   175,   178,
     181,   184,   189,   196,   205,   222,   216,   219,   225,   227,
     235,   237,   241,   247,   245,   249,   251,   257,   253,   255,
     261,   264,   267,   271,   314,   280,   282,   285,   288,   291,
     294,   298,   301,   304,   307,   318,   323,   328,   331,   335,
     339,   343,   347,   352,   357,   360,   364,   368,   372,   376,
     380,   383,   387,   391,   395,   399,   402,   406,   409,   413,
     416,   419,   422,   425,   427,   430,   433,   436,   440,   443,
     446,   449,   452,   456,   461,   466,   472,   476,   481,   484,
     488,   492,   497,   500,   504,   507,   511,   515,   520,   523,
     528,   531,   534,   536,   539,   541,   622,   621,     0,   626,
       0,   543,     0,   549,   551,   620,     0,     0,     0,   557,
     559,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   576,   578,     0,     0,   584,   586,   588,   590,   593,
       0,   598,     0,     0,     0,     0,     0,     0,   277,   614,
       0,     0,     0,     0,   642,     0,   146,   231,   275,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   547,     0,   548,     0,   546,     0,     0,     0,
       0,     0,     0,     0,   561,   562,   563,     0,   567,   568,
       0,   571,   572,   573,     0,     0,     0,   582,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   608,   609,
     610,     0,     0,   638,   640,   641,   639,     0,   645,     0,
       0,     0,    10,     0,    22,    24,    26,    28,     0,     0,
       0,     0,     0,    45,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   139,     0,   141,     0,   157,
     153,   155,     0,     0,   169,   171,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   226,     0,   236,
       0,     0,   248,   246,   250,   252,   258,   254,   256,     0,
       0,     0,     0,     0,   281,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   426,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   529,
     532,   535,   537,   540,   542,     0,     0,   550,   552,   618,
     553,   555,     0,     0,   564,   569,   577,     0,   580,     0,
     587,   589,     0,     0,   596,   599,   600,   602,   605,   278,
     615,   643,     0,     0,     0,    14,    30,    34,    38,    50,
      53,    71,    56,    59,    62,    65,    68,    76,    79,    82,
      86,    89,    92,    97,   104,   113,   124,   127,   136,   130,
     133,   143,   159,   162,   166,   173,   176,   179,   182,   185,
     190,   197,   206,   223,   217,   220,   228,   238,   242,   262,
       0,     0,   272,   315,   283,   286,   289,   292,   295,   299,
     302,   305,   308,   319,   324,   329,   332,   336,   340,   344,
     348,   353,   358,   361,   365,   369,   373,   377,   381,   384,
     388,   392,   396,     0,   403,   407,   410,   414,   417,   420,
     423,   428,   431,   434,   437,   441,   444,   447,   450,     0,
       0,   462,   467,     0,     0,     0,     0,   489,   493,     0,
       0,     0,     0,   512,   516,     0,     0,   627,   544,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     147,   232,   674,   676,   677,   684,   685,   686,   687,   688,
     689,   690,   680,   681,   682,   683,   691,   692,   693,   697,
     694,   698,   695,   699,   696,   700,   701,   702,   703,   704,
     705,   706,   707,   708,   709,   710,   711,   712,   713,   714,
     679,   678,   276,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     656,     0,     0,   667,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   619,     0,     0,   558,   560,     0,     0,   579,     0,
       0,     0,     0,     0,   585,     0,     0,   597,   601,     0,
       0,   616,     0,     0,   675,     0,     0,     0,    39,    51,
      54,     0,    57,    60,    63,    66,     0,    77,    80,    83,
      87,    90,     0,     0,     0,     0,   125,   128,   137,   131,
     134,     0,   160,     0,   167,   174,   177,   180,   183,     0,
       0,     0,     0,   224,   218,   221,     0,     0,   243,   263,
     650,   653,     0,     0,   665,     0,     0,     0,     0,   284,
     287,   290,   293,     0,   300,   303,   306,     0,     0,     0,
     330,     0,     0,     0,     0,     0,     0,   359,     0,     0,
       0,     0,     0,   382,     0,     0,     0,     0,   400,     0,
     408,     0,   415,     0,   421,     0,   429,   432,   435,     0,
     442,   445,   448,   451,     0,     0,     0,     0,     0,     0,
     482,   485,     0,     0,   498,   501,   505,   508,     0,     0,
     521,   524,   545,     0,     0,     0,     0,     0,     0,     0,
       0,   637,   635,   636,     0,   591,   594,   603,   606,     0,
       0,    15,    31,    35,     0,     0,    93,    98,   105,   114,
     144,   163,   186,   191,   198,   207,   229,   239,     0,     0,
       0,   657,     0,     0,   668,   273,     0,   296,   309,   320,
     325,   333,     0,     0,     0,   349,   354,   362,     0,     0,
     374,   378,   385,     0,     0,   397,     0,   404,   411,     0,
       0,   438,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     554,   556,   647,     0,   648,   646,     0,     0,   631,   632,
     633,   634,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    69,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   265,     0,     0,
     666,   268,     0,     0,   624,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   629,     0,     0,     0,     0,     0,     0,
     418,   424,     0,   453,   457,     0,     0,   473,   477,   483,
       0,     0,     0,   499,     0,   506,     0,     0,     0,   522,
       0,     0,     0,     0,     0,   592,   595,     0,     0,   670,
     669,   671,   672,     0,     0,     0,    32,    36,     0,     0,
       0,     0,   145,   164,     0,     0,     0,     0,   230,   240,
     651,   654,     0,   661,   658,     0,   274,     0,   316,     0,
     297,     0,     0,     0,   334,   337,   341,   345,     0,     0,
     363,   366,   370,   375,   379,   386,     0,   389,     0,   393,
       0,     0,     0,   412,   439,     0,     0,     0,     0,     0,
       0,   486,   490,   494,   502,   509,   513,   517,   525,   565,
     649,   570,   581,     0,     0,     0,     0,     0,    16,    94,
      99,   106,   115,   187,   192,   199,   208,     0,     0,   266,
       0,     0,   269,   623,     0,   625,   310,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   628,     0,   630,     0,
       0,   401,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     673,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   652,   655,     0,     0,   317,     0,     0,     0,   338,
     342,   346,     0,     0,   367,   371,   390,   394,   398,   405,
     454,   458,   463,   468,   474,   478,   487,   491,     0,   503,
     510,   514,     0,   526,     0,     0,   148,   233,     0,    95,
       0,     0,     0,   188,     0,     0,     0,     0,   659,     0,
     321,   326,   350,   355,     0,     0,     0,     0,     0,     0,
     495,   518,   604,   607,     0,     0,    17,   100,   107,   116,
     193,   200,   209,     0,     0,   311,     0,     0,     0,     0,
     455,     0,     0,     0,   475,     0,     0,     0,   149,   234,
       0,     0,     0,     0,     0,     0,     0,   662,   660,     0,
     322,   327,   351,   356,   459,   464,   469,   479,   496,   519,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    18,   101,   108,   117,   194,   201,   210,
       0,   312,   460,   465,     0,   480,     0,     0,     0,     0,
       0,     0,     0,   663,     0,   470,     0,   102,     0,     0,
     195,     0,     0,     0,   313,     0,    19,   109,   118,   202,
     211,   664,   471,     0,     0,     0,     0,     0,    20,     0,
       0,     0,     0,   110,   119,   203,   212,     0,     0,     0,
       0,   111,     0,   204,     0,   120,   213,     0,     0,     0,
       0,   121,   214,     0,     0,   122,   215
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1501, -1501, -1501, -1501, -1501, -1501, -1501,   442, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501, -1501, -1501,  -362, -1501,  -316,  -767, -1409,  -339, -1501,
    -675,  -486, -1100, -1501, -1501, -1501, -1398,  -521, -1501,  -262,
   -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501, -1501,
   -1501,  -226, -1501, -1501, -1501, -1501, -1500,    88, -1501
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     8,    14,     9,    15,   403,    16,   223,   600,
    1144,  1508,  1722,  1820,  1866,  1893,   601,   602,   603,   604,
     605,  1145,  1509,   606,  1146,  1510,   607,  1147,   610,   611,
     608,  1148,   609,  1149,   613,  1151,   614,  1152,   615,  1153,
     616,  1154,   617,  1155,   612,  1150,   618,  1156,   619,  1157,
     620,  1158,   621,  1159,   622,  1160,   623,  1161,  1513,  1723,
     624,  1162,  1514,  1724,  1821,  1867,   625,  1163,  1515,  1725,
    1822,  1868,  1894,  1907,   626,  1164,  1516,  1726,  1823,  1869,
    1895,  1908,  1917,  1923,   627,  1165,   628,  1166,   630,  1168,
     631,  1169,   629,  1167,   632,   634,   633,  1170,  1517,   789,
    1282,  1794,   637,   638,   636,   635,  1171,   639,  1172,  1518,
     640,  1173,   641,   642,   643,  1174,   644,  1175,   645,  1176,
     646,  1177,   647,  1178,  1519,  1727,   648,  1179,  1520,  1728,
    1824,  1870,   649,  1180,  1521,  1729,  1825,  1871,  1896,  1909,
     650,  1181,  1522,  1730,  1826,  1872,  1897,  1910,  1918,  1924,
     652,  1183,   653,  1184,   651,  1182,   654,   655,  1185,  1523,
     790,  1283,  1795,   656,   657,  1186,  1524,   658,  1187,   660,
     659,   661,   662,   664,   665,   663,   666,  1188,   667,  1612,
     668,  1615,   669,  1195,  1532,   791,   781,   671,   672,  1197,
     673,  1198,   674,  1199,   675,  1200,   676,  1201,  1536,   677,
    1202,   678,  1203,   679,  1204,   680,  1205,  1537,  1736,  1829,
    1874,   670,  1196,  1684,   681,  1206,  1538,  1806,   682,  1207,
    1539,  1807,   683,  1208,   684,  1209,  1540,   685,  1210,  1689,
     686,  1211,  1690,   687,  1212,  1691,   688,  1213,  1544,  1808,
     689,  1214,  1545,  1809,   690,  1215,   691,  1216,  1546,   692,
    1217,  1694,   693,  1218,  1695,   694,  1219,  1549,   695,  1220,
    1550,   696,  1221,   697,  1222,  1551,   698,  1223,  1697,   699,
    1224,  1699,   700,  1225,  1556,   701,  1466,   702,  1227,  1558,
     703,  1228,   704,  1229,  1559,   705,  1230,   706,  1231,   707,
    1232,   708,  1233,   709,   710,  1234,   711,  1235,   712,  1236,
     713,  1237,  1562,   714,  1238,   715,  1239,   716,  1240,   717,
    1241,   718,  1645,  1784,   719,  1646,  1785,  1849,   720,  1244,
    1786,  1850,   721,  1245,  1787,  1851,  1885,   722,  1649,  1788,
     723,  1650,  1789,  1852,   724,  1478,   725,  1479,  1709,   726,
    1250,  1710,   727,  1251,  1711,  1816,   728,  1482,   729,  1483,
    1712,   730,  1484,   731,  1485,  1713,   732,  1256,  1714,   733,
    1257,  1715,  1817,   734,  1488,   735,  1489,  1716,   736,   737,
     738,   739,   740,   741,   745,  1260,   747,   748,  1080,  1081,
     752,   753,  1084,  1085,   764,   765,  1087,   768,   769,   770,
     771,  1502,   772,  1503,  1095,   774,  1096,  1097,  1504,  1098,
    1505,   782,  1099,   557,  1079,  1533,  1534,  1535,  1552,   743,
    1553,  1554,  1092,  1414,   224,   787,  1493,  1494,  1495,  1190,
    1438,  1677,  1439,  1678,  1191,  1530,  1681,  1804,  1680,  1847,
    1883,  1193,  1442,  1194,  1591,  1592,  1593,  1142,  1143
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     558,   559,   560,   561,   562,   563,   564,  1594,  1496,  1497,
     568,   569,   570,     2,   555,   573,   574,   550,  1088,  1089,
    1090,  1091,   580,     5,   582,   583,   584,   585,   586,   587,
     565,   566,   567,   591,   592,   433,  1528,   595,  1529,   548,
     440,    10,   552,  1541,  1542,  1543,   434,   435,   455,  1547,
    1548,   441,   442,   467,  1102,     6,   412,   417,   593,   456,
     457,   419,  1103,   443,   468,   469,     7,   413,   418,  1332,
    1333,    12,   420,    11,   444,  1335,  1336,    13,   590,  1104,
    1105,  1106,  1107,  1108,  1109,  1110,  1111,  1112,  1113,  1114,
    1115,  1116,  1117,  1118,  1119,  1120,  1121,  1122,  1123,  1124,
    1125,  1126,  1127,  1128,  1129,  1130,  1131,  1132,  1133,  1134,
    1135,  1136,  1137,  1138,  1139,  1140,  1141,   461,   546,  1330,
    1589,  1411,    17,  1412,  1413,   226,  1618,  1619,   462,   547,
    1331,  1590,  1625,  1619,  1626,  1619,  1627,  1619,  1631,  1619,
     416,  1632,  1619,  1637,  1638,   556,  1639,  1638,  1665,  1666,
    1667,  1666,  1780,  1619,  1781,  1619,  1782,  1619,  1783,  1619,
    1827,  1666,  1403,  1404,   227,   225,  1720,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,     3,   269,
     270,  1581,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,     4,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,  1717,  1718,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,  1803,  1737,  1738,
     333,   334,   335,  1742,  1743,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   336,   337,   338,
     404,   210,   339,   340,   341,   342,   343,   211,   344,   345,
     346,   347,   212,   213,   214,   215,   348,   349,   350,   216,
     217,   218,   219,   220,   221,   222,   351,   352,   353,   354,
     355,  1022,  1023,  1024,  1025,  1026,   356,   357,   358,  1030,
    1031,  1032,  1033,   359,   360,  1036,  1037,  1038,  1039,   361,
     362,   363,   364,   365,   366,   367,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,  1014,
    1015,   381,   382,   383,   384,  1065,  1066,   385,   386,  1069,
    1070,  1071,  1072,   387,   388,  1075,  1076,   389,   390,   391,
     392,   393,  1059,  1060,   394,   395,  1063,  1064,  1043,   396,
    1082,  1083,  1047,   397,  1049,  1086,   398,   399,   400,   401,
    1093,  1094,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   402,   405,   406,   407,   792,   408,
     409,   410,   411,   421,   211,   414,   437,   415,   422,   212,
     213,   214,   215,   423,   742,   424,   216,   217,   218,   219,
     220,   221,   222,   425,   426,   427,   428,   429,   430,   431,
     432,   436,   438,   439,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   459,   458,   460,   463,   464,   465,
     466,   470,   471,   472,   473,   599,   474,   475,   476,   477,
     478,   479,  1685,   480,   481,   482,   483,   484,   485,   486,
     487,   488,   489,   490,  1368,   491,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,   502,   503,   504,   505,
    1384,  1385,   506,   507,  1388,  1389,   508,   509,   510,   511,
     512,   513,   514,   515,   516,   517,   518,   519,   520,   521,
     522,   523,   524,   525,   526,   527,   528,   529,   530,  1409,
    1410,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,   541,   542,   543,   544,   545,   549,   551,   553,   554,
     571,   572,   575,   576,   577,   578,   579,   596,   581,   588,
     589,   594,   597,   598,   744,   750,   746,   749,   801,   754,
     751,   755,   756,   757,  1408,   758,   759,   760,   761,   762,
     763,   939,   767,  1698,   766,   940,   773,   775,   776,   777,
     778,   779,   780,   783,   784,   785,   786,   788,  1555,   794,
     795,   793,   796,   797,   798,  1448,  1189,   799,   800,  1451,
     803,   802,   804,   805,   806,   825,  1457,   811,   807,  1460,
    1461,   808,  1462,   809,   810,  1465,   812,  1467,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   822,   823,   824,
     827,   829,   826,   828,   830,   831,   833,   832,   834,   835,
    1480,  1481,   848,   836,   837,   838,  1486,  1487,   839,   840,
     841,   842,   843,   844,   845,   846,   847,  1474,  1475,   849,
     850,   851,   852,   853,   854,   855,   856,  1469,   857,  1470,
     858,  1660,   859,  1266,   860,   861,   862,   863,   864,   865,
     866,  1441,   867,   868,   869,   870,   871,   872,   873,   874,
     875,   876,   877,   878,   879,   880,   881,   882,   883,   884,
     885,   886,   887,   888,   889,   890,   891,   892,   893,   894,
     895,   896,   897,   898,   899,   900,   901,   902,   903,   904,
    1444,   905,   906,   907,   908,   909,   910,   911,   912,   913,
     914,   915,   916,   917,   918,   919,   920,   921,   922,   923,
     924,   925,   926,   927,   928,   929,   930,   931,   932,   933,
     934,   941,   944,   937,   935,   936,   938,   942,   943,   946,
       0,   947,   950,   951,   949,   945,     0,   952,   948,   953,
     955,   959,   961,   954,  1010,   956,   957,   958,   960,   962,
     963,   964,   965,     0,     0,     0,     0,   966,   967,   968,
     969,   970,   971,   972,   973,   974,   975,   976,   977,   978,
     979,   980,   981,   982,   983,  1641,   984,   985,   986,   987,
     988,   989,   990,  1647,  1648,   991,   992,   993,   994,   995,
     996,   997,   998,   999,  1000,  1001,  1002,  1003,  1004,  1005,
    1006,  1007,  1008,  1009,  1011,  1012,  1013,  1016,  1017,  1018,
    1019,  1284,  1020,  1021,  1027,  1028,  1029,  1034,  1035,  1040,
    1041,  1042,  1044,  1045,  1046,  1048,  1050,  1051,  1052,  1053,
    1054,  1055,  1056,  1057,  1058,  1683,  1061,  1062,  1067,  1686,
    1068,  1073,  1074,  1077,  1078,  1100,  1101,  1192,  1226,  1242,
    1243,  1261,  1246,  1247,  1696,  1248,  1249,  1252,  1700,  1253,
    1702,  1254,  1255,  1258,  1259,  1262,  1263,  1264,  1265,  1267,
    1268,  1270,  1271,  1269,  1272,  1273,     0,  1274,  1338,  1390,
    1277,  1275,  1276,  1278,  1279,  1280,  1281,  1285,  1286,  1288,
    1287,  1289,  1290,  1291,  1292,  1293,  1294,  1295,  1334,  1297,
    1298,  1296,  1299,  1300,  1301,     0,     0,  1424,  1302,  1303,
    1304,  1305,  1306,  1307,  1308,  1309,  1310,  1391,  1312,  1394,
    1311,  1314,  1313,  1315,  1316,  1317,  1318,     0,  1395,  1425,
    1319,  1320,  1321,  1322,  1323,  1324,  1325,  1352,     0,  1328,
    1326,  1327,  1329,  1337,  1339,  1340,  1341,  1342,     0,  1344,
    1345,  1343,  1346,     0,  1347,  1350,  1348,  1349,     0,  1351,
    1405,  1353,  1354,  1355,  1356,  1357,  1358,  1363,  1359,     0,
    1360,  1361,  1362,  1364,  1406,  1365,  1366,  1367,  1369,  1370,
    1371,  1372,  1374,  1373,  1376,  1377,  1375,  1378,  1380,  1379,
    1381,  1382,  1383,  1587,     0,     0,  1386,  1805,  1387,  1392,
    1393,     0,  1396,  1398,  1399,  1402,  1397,  1400,  1407,     0,
    1401,  1415,  1416,  1419,  1417,  1420,  1418,  1421,     0,  1422,
       0,  1423,  1426,  1427,  1428,  1429,  1430,  1431,  1432,  1433,
    1434,  1435,  1436,  1437,  1440,  1443,  1445,  1446,  1447,  1452,
    1453,  1449,  1450,  1454,  1458,  1455,  1459,  1463,  1456,  1464,
    1506,     0,  1468,  1472,  1473,  1471,  1476,  1477,  1490,  1491,
    1492,  1498,  1499,  1500,  1501,  1511,  1861,  1507,  1512,     0,
    1525,     0,  1526,  1560,  1561,  1557,     0,     0,  1527,  1531,
    1565,  1566,  1569,  1563,  1564,  1573,  1570,  1567,  1568,  1571,
    1572,  1574,  1575,  1576,  1577,  1579,  1578,  1580,  1663,  1582,
    1583,  1584,  1585,  1586,  1588,  1596,  1597,  1595,     0,  1610,
    1598,  1599,  1600,  1601,  1602,  1603,  1604,     0,     0,  1605,
    1606,  1607,  1608,  1609,  1616,  1620,  1624,  1611,  1613,  1614,
    1617,  1621,     0,  1622,  1623,  1628,  1629,  1630,  1633,  1634,
    1635,  1651,  1652,  1653,  1636,  1640,  1642,  1643,  1644,     0,
       0,  1659,  1654,  1655,  1656,  1661,  1662,  1657,  1658,  1664,
    1668,  1669,  1670,     0,  1679,  1671,  1682,  1672,  1673,  1674,
    1675,  1676,  1687,  1688,  1692,  1693,  1701,  1750,  1703,     0,
    1704,  1705,  1706,  1707,  1708,  1719,  1721,  1731,  1732,  1733,
    1735,  1739,  1734,  1740,  1741,  1744,  1745,  1746,  1747,  1748,
    1749,     0,     0,     0,     0,     0,  1756,  1757,  1759,  1751,
    1752,  1753,  1754,  1755,  1758,  1760,  1761,  1762,  1763,  1764,
    1766,  1765,     0,  1666,  1769,  1768,  1770,  1777,  1771,  1767,
    1772,  1773,  1774,  1775,     0,  1776,  1778,  1779,  1792,  1793,
       0,  1790,     0,  1791,  1796,  1797,  1810,  1814,  1798,  1799,
    1800,  1801,  1802,  1811,  1812,  1813,  1815,  1818,  1819,  1828,
    1830,  1831,  1832,  1833,  1834,     0,     0,  1838,  1839,  1862,
    1863,  1865,  1835,  1836,  1837,  1840,  1841,  1842,  1843,  1844,
    1845,  1846,  1848,     0,     0,     0,     0,  1877,  1853,  1854,
    1855,  1856,  1857,  1858,  1859,  1860,  1864,  1873,  1875,  1876,
    1878,  1879,  1880,  1881,  1882,  1884,     0,  1886,  1891,  1892,
    1898,  1887,  1888,     0,  1889,  1890,  1899,  1900,  1901,  1902,
    1903,  1911,  1913,  1912,     0,  1904,  1905,  1906,  1914,  1919,
    1920,  1915,  1916,  1925,  1926,     0,  1921,  1922
};

static const yytype_int16 yycheck[] =
{
     362,   363,   364,   365,   366,   367,   368,  1507,  1406,  1407,
     372,   373,   374,     0,    75,   377,   378,   356,   211,   212,
     213,   214,   384,   198,   386,   387,   388,   389,   390,   391,
     369,   370,   371,   395,   396,   196,   202,   399,   204,   355,
     196,   204,   358,  1452,  1453,  1454,   207,   208,   196,  1458,
    1459,   207,   208,   196,   199,   196,   196,   196,   397,   207,
     208,   196,   207,   196,   207,   208,   207,   207,   207,   201,
     202,   207,   207,   204,   207,   201,   202,   207,   394,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   196,   196,   196,
     196,   213,   204,   215,   216,   198,   201,   202,   207,   207,
     207,   207,   201,   202,   201,   202,   201,   202,   201,   202,
     196,   201,   202,   201,   202,   206,   201,   202,   201,   202,
     201,   202,   201,   202,   201,   202,   201,   202,   201,   202,
     201,   202,  1262,  1263,   198,   207,  1666,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   195,   198,
     198,   201,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   209,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,  1663,  1664,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,  1777,  1687,  1688,
     198,   198,   198,  1692,  1693,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   198,   198,   198,
     207,   199,   198,   198,   198,   198,   198,   205,   198,   198,
     198,   198,   210,   211,   212,   213,   198,   198,   198,   217,
     218,   219,   220,   221,   222,   223,   198,   198,   198,   198,
     198,   873,   874,   875,   876,   877,   198,   198,   198,   881,
     882,   883,   884,   198,   198,   887,   888,   889,   890,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   865,
     866,   198,   198,   198,   198,   917,   918,   198,   198,   921,
     922,   923,   924,   198,   198,   927,   928,   198,   198,   198,
     198,   198,   911,   912,   198,   198,   915,   916,   894,   198,
     942,   943,   898,   198,   900,   947,   198,   198,   198,   198,
     952,   953,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   198,   207,   207,   207,   199,   207,
     207,   207,   207,   196,   205,   207,   196,   207,   207,   210,
     211,   212,   213,   207,   199,   207,   217,   218,   219,   220,
     221,   222,   223,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   196,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   196,   207,   207,   207,   207,   207,
     207,   196,   207,   207,   207,   403,   207,   207,   207,   207,
     207,   207,  1619,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,  1226,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
    1242,  1243,   207,   207,  1246,  1247,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,  1271,
    1272,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   196,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   196,   207,   207,
     207,   207,   196,   207,   199,   202,   199,   198,   204,   199,
     202,   199,   199,   202,  1270,   199,   199,   202,   199,   199,
     199,   207,   199,  1638,   202,   207,   202,   202,   202,   202,
     199,   199,   199,   199,   199,   199,   199,   199,  1464,   199,
     199,   202,   199,   199,   202,  1347,   198,   202,   202,  1351,
     199,   204,   199,   204,   204,   199,  1358,   202,   204,  1361,
    1362,   204,  1364,   204,   204,  1367,   202,  1369,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     199,   199,   204,   204,   199,   199,   202,   204,   199,   199,
    1392,  1393,   204,   202,   202,   202,  1398,  1399,   202,   202,
     202,   202,   202,   202,   202,   202,   199,  1386,  1387,   199,
     204,   202,   199,   199,   199,   199,   199,  1373,   199,  1375,
     199,  1582,   204,   203,   202,   202,   202,   202,   199,   202,
     202,  1333,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   199,   202,   202,
    1336,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   199,   199,   199,   199,   199,
     199,   207,   207,   199,   202,   202,   199,   202,   202,   199,
      -1,   202,   199,   199,   202,   207,    -1,   202,   207,   202,
     199,   199,   199,   207,   200,   207,   207,   207,   202,   202,
     202,   202,   207,    -1,    -1,    -1,    -1,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,  1557,   207,   207,   207,   207,
     207,   207,   207,  1565,  1566,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   200,   207,   207,   207,   207,   207,
     207,  1143,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,  1617,   207,   207,   207,  1621,
     207,   207,   207,   207,   207,   207,   207,   198,   202,   202,
     202,   199,   202,   202,  1636,   202,   202,   202,  1640,   202,
    1642,   202,   202,   202,   202,   202,   202,   199,   199,   203,
     199,   198,   198,   203,   198,   198,    -1,   199,   203,   207,
     199,   202,   202,   199,   202,   202,   199,   202,   202,   199,
     202,   199,   199,   202,   199,   199,   199,   199,   196,   199,
     199,   202,   199,   199,   199,    -1,    -1,   196,   202,   202,
     202,   202,   199,   199,   199,   199,   199,   207,   199,   207,
     202,   199,   202,   199,   199,   199,   199,    -1,   207,   196,
     202,   202,   202,   202,   199,   199,   199,   203,    -1,   199,
     202,   202,   199,   202,   199,   199,   199,   199,    -1,   199,
     199,   202,   199,    -1,   202,   199,   202,   202,    -1,   202,
     200,   203,   203,   202,   202,   199,   202,   199,   203,    -1,
     203,   202,   202,   202,   200,   203,   203,   202,   202,   199,
     202,   199,   199,   202,   199,   199,   202,   199,   199,   202,
     199,   199,   199,   203,    -1,    -1,   202,  1779,   202,   202,
     202,    -1,   207,   202,   202,   199,   207,   207,   200,    -1,
     207,   207,   207,   202,   207,   202,   207,   207,    -1,   207,
      -1,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   202,   202,   207,   200,   207,   200,
     200,   207,   207,   200,   200,   207,   200,   200,   207,   200,
     200,    -1,   207,   202,   202,   207,   202,   202,   199,   199,
     207,   199,   199,   199,   199,   199,  1848,   200,   199,    -1,
     202,    -1,   202,   199,   199,   202,    -1,    -1,   207,   207,
     202,   202,   199,   207,   207,   199,   202,   207,   207,   202,
     202,   202,   199,   202,   202,   199,   202,   202,   200,   202,
     201,   201,   199,   199,   203,   199,   199,   202,    -1,   207,
     202,   202,   202,   202,   199,   199,   202,    -1,    -1,   202,
     202,   202,   199,   199,   199,   199,   199,   207,   207,   207,
     202,   202,    -1,   203,   203,   203,   203,   199,   199,   199,
     199,   207,   207,   207,   202,   202,   202,   199,   199,    -1,
      -1,   199,   207,   207,   207,   199,   199,   207,   207,   200,
     207,   207,   207,    -1,   199,   207,   199,   207,   207,   207,
     207,   207,   200,   200,   200,   200,   199,   207,   202,    -1,
     202,   202,   202,   202,   202,   202,   202,   199,   199,   202,
     199,   199,   202,   199,   199,   199,   199,   199,   199,   199,
     199,    -1,    -1,    -1,    -1,    -1,   199,   199,   199,   207,
     207,   207,   207,   207,   202,   199,   199,   202,   199,   201,
     207,   201,    -1,   202,   199,   202,   202,   200,   202,   207,
     202,   199,   202,   202,    -1,   202,   207,   202,   199,   199,
      -1,   207,    -1,   207,   207,   207,   199,   199,   207,   207,
     207,   207,   207,   202,   202,   202,   202,   199,   199,   199,
     199,   199,   199,   199,   207,    -1,    -1,   199,   199,   199,
     199,   199,   207,   207,   207,   202,   202,   202,   202,   202,
     202,   202,   202,    -1,    -1,    -1,    -1,   199,   207,   207,
     207,   207,   207,   207,   207,   202,   202,   207,   207,   202,
     202,   202,   199,   202,   202,   199,    -1,   207,   199,   199,
     199,   207,   207,    -1,   207,   207,   202,   202,   202,   202,
     207,   199,   199,   202,    -1,   207,   207,   207,   202,   202,
     202,   207,   207,   199,   199,    -1,   207,   207
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   263,     0,   195,   209,   198,   196,   207,   264,   266,
     204,   204,   207,   207,   265,   267,   269,   204,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     199,   205,   210,   211,   212,   213,   217,   218,   219,   220,
     221,   222,   223,   270,   676,   207,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   268,   207,   207,   207,   207,   207,   207,
     207,   207,   196,   207,   207,   207,   196,   196,   207,   196,
     207,   196,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   196,   207,   208,   207,   196,   207,   196,
     196,   207,   208,   196,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   196,   207,   208,   207,   196,
     207,   196,   207,   207,   207,   207,   207,   196,   207,   208,
     196,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   196,   207,   667,   196,
     670,   207,   667,   207,   207,    75,   206,   665,   665,   665,
     665,   665,   665,   665,   665,   670,   670,   670,   665,   665,
     665,   207,   207,   665,   665,   207,   207,   207,   207,   207,
     665,   207,   665,   665,   665,   665,   665,   665,   207,   207,
     667,   665,   665,   670,   207,   665,   196,   196,   207,   269,
     271,   278,   279,   280,   281,   282,   285,   288,   292,   294,
     290,   291,   306,   296,   298,   300,   302,   304,   308,   310,
     312,   314,   316,   318,   322,   328,   336,   346,   348,   354,
     350,   352,   356,   358,   357,   367,   366,   364,   365,   369,
     372,   374,   375,   376,   378,   380,   382,   384,   388,   394,
     402,   416,   412,   414,   418,   419,   425,   426,   429,   432,
     431,   433,   434,   437,   435,   436,   438,   440,   442,   444,
     473,   449,   450,   452,   454,   456,   458,   461,   463,   465,
     467,   476,   480,   484,   486,   489,   492,   495,   498,   502,
     506,   508,   511,   514,   517,   520,   523,   525,   528,   531,
     534,   537,   539,   542,   544,   547,   549,   551,   553,   555,
     556,   558,   560,   562,   565,   567,   569,   571,   573,   576,
     580,   584,   589,   592,   596,   598,   601,   604,   608,   610,
     613,   615,   618,   621,   625,   627,   630,   631,   632,   633,
     634,   635,   199,   671,   199,   636,   199,   638,   639,   198,
     202,   202,   642,   643,   199,   199,   199,   202,   199,   199,
     202,   199,   199,   199,   646,   647,   202,   199,   649,   650,
     651,   652,   654,   202,   657,   202,   202,   202,   199,   199,
     199,   448,   663,   199,   199,   199,   199,   677,   199,   361,
     422,   447,   199,   202,   199,   199,   199,   199,   202,   202,
     202,   204,   204,   199,   199,   204,   204,   204,   204,   204,
     204,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   199,   204,   199,   204,   199,
     199,   199,   204,   202,   199,   199,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   199,   204,   199,
     204,   202,   199,   199,   199,   199,   199,   199,   199,   204,
     202,   202,   202,   202,   199,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   199,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   199,
     199,   199,   199,   199,   199,   202,   202,   199,   199,   207,
     207,   207,   202,   202,   207,   207,   199,   202,   207,   202,
     199,   199,   202,   202,   207,   199,   207,   207,   207,   199,
     202,   199,   202,   202,   202,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     200,   200,   207,   207,   667,   667,   207,   207,   207,   207,
     207,   207,   665,   665,   665,   665,   665,   207,   207,   207,
     665,   665,   665,   665,   207,   207,   665,   665,   665,   665,
     207,   207,   207,   667,   207,   207,   207,   667,   207,   667,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   670,
     670,   207,   207,   670,   670,   665,   665,   207,   207,   665,
     665,   665,   665,   207,   207,   665,   665,   207,   207,   666,
     640,   641,   665,   665,   644,   645,   665,   648,   211,   212,
     213,   214,   674,   665,   665,   656,   658,   659,   661,   664,
     207,   207,   199,   207,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   699,   700,   272,   283,   286,   289,   293,   295,
     307,   297,   299,   301,   303,   305,   309,   311,   313,   315,
     317,   319,   323,   329,   337,   347,   349,   355,   351,   353,
     359,   368,   370,   373,   377,   379,   381,   383,   385,   389,
     395,   403,   417,   413,   415,   420,   427,   430,   439,   198,
     681,   686,   198,   693,   695,   445,   474,   451,   453,   455,
     457,   459,   462,   464,   466,   468,   477,   481,   485,   487,
     490,   493,   496,   499,   503,   507,   509,   512,   515,   518,
     521,   524,   526,   529,   532,   535,   202,   540,   543,   545,
     548,   550,   552,   554,   557,   559,   561,   563,   566,   568,
     570,   572,   202,   202,   581,   585,   202,   202,   202,   202,
     602,   605,   202,   202,   202,   202,   619,   622,   202,   202,
     637,   199,   202,   202,   199,   199,   203,   203,   199,   203,
     198,   198,   198,   198,   199,   202,   202,   199,   199,   202,
     202,   199,   362,   423,   699,   202,   202,   202,   199,   199,
     199,   202,   199,   199,   199,   199,   202,   199,   199,   199,
     199,   199,   202,   202,   202,   202,   199,   199,   199,   199,
     199,   202,   199,   202,   199,   199,   199,   199,   199,   202,
     202,   202,   202,   199,   199,   199,   202,   202,   199,   199,
     196,   207,   201,   202,   196,   201,   202,   202,   203,   199,
     199,   199,   199,   202,   199,   199,   199,   202,   202,   202,
     199,   202,   203,   203,   203,   202,   202,   199,   202,   203,
     203,   202,   202,   199,   202,   203,   203,   202,   665,   202,
     199,   202,   199,   202,   199,   202,   199,   199,   199,   202,
     199,   199,   199,   199,   665,   665,   202,   202,   665,   665,
     207,   207,   202,   202,   207,   207,   207,   207,   202,   202,
     207,   207,   199,   674,   674,   200,   200,   200,   667,   665,
     665,   213,   215,   216,   675,   207,   207,   207,   207,   202,
     202,   207,   207,   207,   196,   196,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   682,   684,
     202,   681,   694,   202,   693,   207,   200,   207,   665,   207,
     207,   665,   200,   200,   200,   207,   207,   665,   200,   200,
     665,   665,   665,   200,   200,   665,   538,   665,   207,   667,
     667,   207,   202,   202,   670,   670,   202,   202,   597,   599,
     665,   665,   609,   611,   614,   616,   665,   665,   626,   628,
     199,   199,   207,   678,   679,   680,   678,   678,   199,   199,
     199,   199,   653,   655,   660,   662,   200,   200,   273,   284,
     287,   199,   199,   320,   324,   330,   338,   360,   371,   386,
     390,   396,   404,   421,   428,   202,   202,   207,   202,   204,
     687,   207,   446,   667,   668,   669,   460,   469,   478,   482,
     488,   669,   669,   669,   500,   504,   510,   669,   669,   519,
     522,   527,   670,   672,   673,   673,   536,   202,   541,   546,
     199,   199,   564,   207,   207,   202,   202,   207,   207,   199,
     202,   202,   202,   199,   202,   199,   202,   202,   202,   199,
     202,   201,   202,   201,   201,   199,   199,   203,   203,   196,
     207,   696,   697,   698,   698,   202,   199,   199,   202,   202,
     202,   202,   199,   199,   202,   202,   202,   202,   199,   199,
     207,   207,   441,   207,   207,   443,   199,   202,   201,   202,
     199,   202,   203,   203,   199,   201,   201,   201,   203,   203,
     199,   201,   201,   199,   199,   199,   202,   201,   202,   201,
     202,   665,   202,   199,   199,   574,   577,   665,   665,   590,
     593,   207,   207,   207,   207,   207,   207,   207,   207,   199,
     679,   199,   199,   200,   200,   201,   202,   201,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   683,   685,   199,
     690,   688,   199,   665,   475,   668,   665,   200,   200,   491,
     494,   497,   200,   200,   513,   516,   665,   530,   672,   533,
     665,   199,   665,   202,   202,   202,   202,   202,   202,   600,
     603,   606,   612,   617,   620,   623,   629,   678,   678,   202,
     698,   202,   274,   321,   325,   331,   339,   387,   391,   397,
     405,   199,   199,   202,   202,   199,   470,   669,   669,   199,
     199,   199,   669,   669,   199,   199,   199,   199,   199,   199,
     207,   207,   207,   207,   207,   207,   199,   199,   202,   199,
     199,   199,   202,   199,   201,   201,   207,   207,   202,   199,
     202,   202,   202,   199,   202,   202,   202,   200,   207,   202,
     201,   201,   201,   201,   575,   578,   582,   586,   591,   594,
     207,   207,   199,   199,   363,   424,   207,   207,   207,   207,
     207,   207,   207,   698,   689,   665,   479,   483,   501,   505,
     199,   202,   202,   202,   199,   202,   607,   624,   199,   199,
     275,   326,   332,   340,   392,   398,   406,   201,   199,   471,
     199,   199,   199,   199,   207,   207,   207,   207,   199,   199,
     202,   202,   202,   202,   202,   202,   202,   691,   202,   579,
     583,   587,   595,   207,   207,   207,   207,   207,   207,   207,
     202,   665,   199,   199,   202,   199,   276,   327,   333,   341,
     393,   399,   407,   207,   472,   207,   202,   199,   202,   202,
     199,   202,   202,   692,   199,   588,   207,   207,   207,   207,
     207,   199,   199,   277,   334,   342,   400,   408,   199,   202,
     202,   202,   202,   207,   207,   207,   207,   335,   343,   401,
     409,   199,   202,   199,   202,   207,   207,   344,   410,   202,
     202,   207,   207,   345,   411,   199,   199
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   262,   263,   263,   264,   265,   263,   266,   267,   268,
     263,   269,   269,   271,   272,   273,   274,   275,   276,   277,
     270,   278,   270,   279,   270,   280,   270,   281,   270,   282,
     283,   284,   270,   285,   286,   287,   270,   288,   289,   270,
     270,   270,   270,   270,   290,   270,   270,   291,   270,   292,
     293,   270,   294,   295,   270,   296,   297,   270,   298,   299,
     270,   300,   301,   270,   302,   303,   270,   304,   305,   270,
     306,   307,   270,   270,   270,   308,   309,   270,   310,   311,
     270,   312,   313,   270,   270,   314,   315,   270,   316,   317,
     270,   318,   319,   320,   321,   270,   322,   323,   324,   325,
     326,   327,   270,   328,   329,   330,   331,   332,   333,   334,
     335,   270,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   270,   346,   347,   270,   348,   349,   270,   350,
     351,   270,   352,   353,   270,   354,   355,   270,   356,   270,
     357,   270,   358,   359,   360,   270,   361,   362,   363,   270,
     270,   270,   364,   270,   365,   270,   366,   270,   367,   368,
     270,   369,   370,   371,   270,   372,   373,   270,   374,   270,
     375,   270,   376,   377,   270,   378,   379,   270,   380,   381,
     270,   382,   383,   270,   384,   385,   386,   387,   270,   388,
     389,   390,   391,   392,   393,   270,   394,   395,   396,   397,
     398,   399,   400,   401,   270,   402,   403,   404,   405,   406,
     407,   408,   409,   410,   411,   270,   412,   413,   270,   414,
     415,   270,   416,   417,   270,   418,   270,   419,   420,   421,
     270,   422,   423,   424,   270,   425,   270,   426,   427,   428,
     270,   429,   430,   270,   270,   431,   270,   432,   270,   433,
     270,   434,   270,   435,   270,   436,   270,   437,   270,   270,
     270,   438,   439,   270,   440,   441,   270,   442,   443,   270,
     270,   444,   445,   446,   270,   447,   270,   448,   270,   270,
     449,   270,   450,   451,   270,   452,   453,   270,   454,   455,
     270,   456,   457,   270,   458,   459,   460,   270,   461,   462,
     270,   463,   464,   270,   465,   466,   270,   467,   468,   469,
     470,   471,   472,   270,   473,   474,   475,   270,   476,   477,
     478,   479,   270,   480,   481,   482,   483,   270,   484,   485,
     270,   486,   487,   488,   270,   489,   490,   491,   270,   492,
     493,   494,   270,   495,   496,   497,   270,   498,   499,   500,
     501,   270,   502,   503,   504,   505,   270,   506,   507,   270,
     508,   509,   510,   270,   511,   512,   513,   270,   514,   515,
     516,   270,   517,   518,   519,   270,   520,   521,   522,   270,
     523,   524,   270,   525,   526,   527,   270,   528,   529,   530,
     270,   531,   532,   533,   270,   534,   535,   536,   270,   537,
     538,   270,   539,   540,   541,   270,   542,   543,   270,   544,
     545,   546,   270,   547,   548,   270,   549,   550,   270,   551,
     552,   270,   553,   554,   270,   555,   270,   556,   557,   270,
     558,   559,   270,   560,   561,   270,   562,   563,   564,   270,
     565,   566,   270,   567,   568,   270,   569,   570,   270,   571,
     572,   270,   573,   574,   575,   270,   576,   577,   578,   579,
     270,   580,   581,   582,   583,   270,   584,   585,   586,   587,
     588,   270,   589,   590,   591,   270,   592,   593,   594,   595,
     270,   596,   597,   270,   598,   599,   600,   270,   601,   602,
     603,   270,   604,   605,   606,   607,   270,   608,   609,   270,
     610,   611,   612,   270,   613,   614,   270,   615,   616,   617,
     270,   618,   619,   620,   270,   621,   622,   623,   624,   270,
     625,   626,   270,   627,   628,   629,   270,   270,   630,   270,
     270,   631,   270,   270,   632,   270,   633,   270,   270,   634,
     270,   635,   270,   636,   637,   270,   270,   270,   270,   638,
     270,   639,   270,   640,   270,   641,   270,   642,   270,   643,
     270,   270,   270,   270,   644,   270,   270,   270,   270,   645,
     270,   270,   270,   270,   270,   270,   646,   270,   647,   270,
     648,   270,   270,   270,   649,   270,   650,   270,   651,   270,
     652,   653,   270,   654,   655,   270,   656,   270,   657,   270,
     658,   270,   659,   660,   270,   661,   662,   270,   270,   270,
     270,   270,   270,   270,   663,   664,   270,   270,   666,   665,
     665,   667,   667,   668,   669,   669,   671,   670,   672,   673,
     673,   674,   674,   674,   674,   675,   675,   675,   676,   676,
     676,   676,   677,   676,   676,   676,   678,   679,   680,   680,
     682,   683,   681,   684,   685,   681,   686,   686,   688,   689,
     687,   690,   691,   692,   687,   694,   693,   695,   695,   696,
     696,   697,   698,   698,   699,   699,   700,   700,   700,   700,
     700,   700,   700,   700,   700,   700,   700,   700,   700,   700,
     700,   700,   700,   700,   700,   700,   700,   700,   700,   700,
     700,   700,   700,   700,   700,   700,   700,   700,   700,   700,
     700,   700,   700,   700,   700
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     0,     0,    10,     0,     0,     0,
      13,     0,     2,     0,     0,     0,     0,     0,     0,     0,
      23,     0,     5,     0,     5,     0,     5,     0,     5,     0,
       0,     0,    11,     0,     0,     0,    11,     0,     0,     8,
       1,     1,     1,     1,     0,     5,     1,     0,     5,     0,
       0,     8,     0,     0,     8,     0,     0,     8,     0,     0,
       8,     0,     0,     8,     0,     0,     8,     0,     0,    10,
       0,     0,    10,     1,     1,     0,     0,     8,     0,     0,
       8,     0,     0,     8,     1,     0,     0,     8,     0,     0,
       8,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,    20,     0,     0,     0,     0,     0,     0,     0,
       0,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,     0,     0,     8,     0,     0,     8,     0,
       0,     8,     0,     0,     8,     0,     0,     8,     0,     5,
       0,     5,     0,     0,     0,    11,     0,     0,     0,    15,
       1,     1,     0,     5,     0,     5,     0,     5,     0,     0,
       8,     0,     0,     0,    11,     0,     0,     8,     0,     5,
       0,     5,     0,     0,     8,     0,     0,     8,     0,     0,
       8,     0,     0,     8,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,    20,     0,     0,     0,     0,
       0,     0,     0,     0,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,     0,     0,     8,     0,
       0,     8,     0,     0,     8,     0,     5,     0,     0,     0,
      11,     0,     0,     0,    15,     0,     5,     0,     0,     0,
      11,     0,     0,     8,     1,     0,     5,     0,     5,     0,
       5,     0,     5,     0,     5,     0,     5,     0,     5,     1,
       1,     0,     0,     8,     0,     0,    12,     0,     0,    12,
       1,     0,     0,     0,    11,     0,     6,     0,     5,     1,
       0,     5,     0,     0,     8,     0,     0,     8,     0,     0,
       8,     0,     0,     8,     0,     0,     0,    11,     0,     0,
       8,     0,     0,     8,     0,     0,     8,     0,     0,     0,
       0,     0,     0,    20,     0,     0,     0,    13,     0,     0,
       0,     0,    16,     0,     0,     0,     0,    16,     0,     0,
       8,     0,     0,     0,    11,     0,     0,     0,    13,     0,
       0,     0,    13,     0,     0,     0,    13,     0,     0,     0,
       0,    16,     0,     0,     0,     0,    16,     0,     0,     8,
       0,     0,     0,    11,     0,     0,     0,    13,     0,     0,
       0,    13,     0,     0,     0,    11,     0,     0,     0,    11,
       0,     0,     8,     0,     0,     0,    11,     0,     0,     0,
      13,     0,     0,     0,    13,     0,     0,     0,    13,     0,
       0,    12,     0,     0,     0,    13,     0,     0,     8,     0,
       0,     0,    11,     0,     0,     8,     0,     0,    10,     0,
       0,     8,     0,     0,    10,     0,     5,     0,     0,     8,
       0,     0,     8,     0,     0,     8,     0,     0,     0,    11,
       0,     0,     8,     0,     0,     8,     0,     0,     8,     0,
       0,     8,     0,     0,     0,    15,     0,     0,     0,     0,
      18,     0,     0,     0,     0,    18,     0,     0,     0,     0,
       0,    21,     0,     0,     0,    15,     0,     0,     0,     0,
      18,     0,     0,    10,     0,     0,     0,    13,     0,     0,
       0,    13,     0,     0,     0,     0,    16,     0,     0,    10,
       0,     0,     0,    13,     0,     0,    10,     0,     0,     0,
      13,     0,     0,     0,    13,     0,     0,     0,     0,    16,
       0,     0,    10,     0,     0,     0,    13,     1,     0,     5,
       1,     0,     5,     1,     0,     5,     0,     5,     1,     0,
       5,     0,     5,     0,     0,     8,     4,     4,     4,     0,
       5,     0,     5,     0,     9,     0,     9,     0,     7,     0,
       7,     4,     4,     4,     0,    11,     1,     4,     4,     0,
      11,     4,     4,     4,     1,     1,     0,     5,     0,     7,
       0,    11,     4,     1,     0,     7,     0,     5,     0,     5,
       0,     0,    10,     0,     0,    10,     0,     7,     0,     5,
       0,     7,     0,     0,    14,     0,     0,    14,     4,     4,
       4,     1,     1,     1,     0,     0,     7,     1,     0,     5,
       1,     1,     1,     3,     1,     3,     0,     4,     3,     1,
       3,     4,     4,     4,     4,     1,     1,     1,     4,     4,
       4,     4,     0,     5,     1,     4,     1,     1,     1,     3,
       0,     0,     7,     0,     0,     7,     1,     3,     0,     0,
       7,     0,     0,     0,    12,     0,     4,     1,     3,     1,
       1,     1,     1,     3,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* definitions: definitions END  */
#line 277 "parser.y"
          { return(0);
	  }
#line 3224 "parser.tab.c"
    break;

  case 4: /* $@1: %empty  */
#line 282 "parser.y"
          { strcpy(pname, yytext);
	  }
#line 3231 "parser.tab.c"
    break;

  case 5: /* $@2: %empty  */
#line 284 "parser.y"
          { parity = atoi(yytext); 
	    Define_Predicate(pname,parity);
	  }
#line 3239 "parser.tab.c"
    break;

  case 6: /* definitions: definitions PREDICATE LPAR ATOMNAME $@1 SLASH NUMBER $@2 code RPAR  */
#line 287 "parser.y"
          { End_Definition;
            if(verbose)
    		printf("{ %s/%d defined }\n",pname,parity);
	  }
#line 3248 "parser.tab.c"
    break;

  case 7: /* $@3: %empty  */
#line 293 "parser.y"
          { abs_level = atoi(yytext);
          }
#line 3255 "parser.tab.c"
    break;

  case 8: /* $@4: %empty  */
#line 295 "parser.y"
          { parity = atoi(yytext);
	  }
#line 3262 "parser.tab.c"
    break;

  case 9: /* $@5: %empty  */
#line 297 "parser.y"
          { ext = atoi(yytext);
            Define_Abstraction(abs_level,parity,ext);
	  }
#line 3270 "parser.tab.c"
    break;

  case 10: /* definitions: definitions PREDICATE LPAR NUMBER $@3 SLASH NUMBER $@4 SLASH NUMBER $@5 code RPAR  */
#line 300 "parser.y"
          { End_Definition;
            if(verbose)
    		printf("{ abstraction %d defined }\n", abs_level);
	  }
#line 3279 "parser.tab.c"
    break;

  case 13: /* $@6: %empty  */
#line 312 "parser.y"
          { label1 = atoi(yytext);
          }
#line 3286 "parser.tab.c"
    break;

  case 14: /* $@7: %empty  */
#line 314 "parser.y"
          { label2 = atoi(yytext);
	  }
#line 3293 "parser.tab.c"
    break;

  case 15: /* $@8: %empty  */
#line 316 "parser.y"
          { label3 = atoi(yytext);
	  }
#line 3300 "parser.tab.c"
    break;

  case 16: /* $@9: %empty  */
#line 318 "parser.y"
          { label4 = atoi(yytext);
	  }
#line 3307 "parser.tab.c"
    break;

  case 17: /* $@10: %empty  */
#line 320 "parser.y"
          { label5 = atoi(yytext);
	  }
#line 3314 "parser.tab.c"
    break;

  case 18: /* $@11: %empty  */
#line 322 "parser.y"
          { label6 = atoi(yytext);
	  }
#line 3321 "parser.tab.c"
    break;

  case 19: /* $@12: %empty  */
#line 324 "parser.y"
          { label7 = atoi(yytext);
          }
#line 3328 "parser.tab.c"
    break;

  case 20: /* instruction: TOKEN_SWITCH_ON_TERM LPAR NUMBER $@6 COMMA NUMBER $@7 COMMA NUMBER $@8 COMMA NUMBER $@9 COMMA NUMBER $@10 COMMA NUMBER $@11 COMMA NUMBER $@12 RPAR  */
#line 326 "parser.y"
          { Instr_Labels_7(SWITCH_ON_TERM,label1,label2,label3,
			   label4,label5,label6,label7);
	  }
#line 3336 "parser.tab.c"
    break;

  case 21: /* $@13: %empty  */
#line 330 "parser.y"
          { label1 = atoi(yytext);
          }
#line 3343 "parser.tab.c"
    break;

  case 22: /* instruction: TOKEN_TRY_SINGLE LPAR NUMBER $@13 RPAR  */
#line 332 "parser.y"
          { Instr_Label(TRY_SINGLE,label1);
          }
#line 3350 "parser.tab.c"
    break;

  case 23: /* $@14: %empty  */
#line 335 "parser.y"
          { label = atoi(yytext);
          }
#line 3357 "parser.tab.c"
    break;

  case 24: /* instruction: TOKEN_TRY LPAR NUMBER $@14 RPAR  */
#line 337 "parser.y"
          { Instr_Label(TRY,label);
          }
#line 3364 "parser.tab.c"
    break;

  case 25: /* $@15: %empty  */
#line 340 "parser.y"
          { label = atoi(yytext);
          }
#line 3371 "parser.tab.c"
    break;

  case 26: /* instruction: TOKEN_RETRY LPAR NUMBER $@15 RPAR  */
#line 342 "parser.y"
          { Instr_Label(RETRY,label);
          }
#line 3378 "parser.tab.c"
    break;

  case 27: /* $@16: %empty  */
#line 345 "parser.y"
          { label = atoi(yytext);
          }
#line 3385 "parser.tab.c"
    break;

  case 28: /* instruction: TOKEN_TRUST LPAR NUMBER $@16 RPAR  */
#line 347 "parser.y"
          { Instr_Label(TRUST,label);
          }
#line 3392 "parser.tab.c"
    break;

  case 29: /* $@17: %empty  */
#line 350 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3399 "parser.tab.c"
    break;

  case 30: /* $@18: %empty  */
#line 352 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 3406 "parser.tab.c"
    break;

  case 31: /* $@19: %empty  */
#line 354 "parser.y"
          { index3 = atoi(yytext);
	  }
#line 3413 "parser.tab.c"
    break;

  case 32: /* instruction: TOKEN_GUARD_UNORDER LPAR NUMBER $@17 COMMA NUMBER $@18 COMMA NUMBER $@19 RPAR  */
#line 356 "parser.y"
          { Instr_Indices_3(GUARD_UNORDER,index1,index2,index3);
          }
#line 3420 "parser.tab.c"
    break;

  case 33: /* $@20: %empty  */
#line 359 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3427 "parser.tab.c"
    break;

  case 34: /* $@21: %empty  */
#line 361 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 3434 "parser.tab.c"
    break;

  case 35: /* $@22: %empty  */
#line 363 "parser.y"
          { index3 = atoi(yytext);
	  }
#line 3441 "parser.tab.c"
    break;

  case 36: /* instruction: TOKEN_GUARD_ORDER LPAR NUMBER $@20 COMMA NUMBER $@21 COMMA NUMBER $@22 RPAR  */
#line 365 "parser.y"
          { Instr_Indices_3(GUARD_ORDER,index1,index2,index3);
          }
#line 3448 "parser.tab.c"
    break;

  case 37: /* $@23: %empty  */
#line 368 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3455 "parser.tab.c"
    break;

  case 38: /* $@24: %empty  */
#line 370 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 3462 "parser.tab.c"
    break;

  case 39: /* instruction: TOKEN_GUARD_UNIT LPAR NUMBER $@23 COMMA NUMBER $@24 RPAR  */
#line 372 "parser.y"
          { Instr_Index_Index(GUARD_UNIT,index1,index2);
          }
#line 3469 "parser.tab.c"
    break;

  case 40: /* instruction: TOKEN_GUARD_COND  */
#line 375 "parser.y"
          { Instr_None(GUARD_COND);
	  }
#line 3476 "parser.tab.c"
    break;

  case 41: /* instruction: TOKEN_GUARD_COMMIT  */
#line 378 "parser.y"
          { Instr_None(GUARD_COMMIT);
	  }
#line 3483 "parser.tab.c"
    break;

  case 42: /* instruction: TOKEN_GUARD_WAIT  */
#line 381 "parser.y"
          { Instr_None(GUARD_WAIT)
	  }
#line 3490 "parser.tab.c"
    break;

  case 43: /* instruction: TOKEN_GUARD_CUT  */
#line 384 "parser.y"
          { Instr_None(GUARD_CUT);
	  }
#line 3497 "parser.tab.c"
    break;

  case 44: /* $@25: %empty  */
#line 387 "parser.y"
          { size = atoi(yytext);
	  }
#line 3504 "parser.tab.c"
    break;

  case 45: /* instruction: TOKEN_ALLOCATE LPAR NUMBER $@25 RPAR  */
#line 389 "parser.y"
          { Instr_Size(ALLOCATE,size);
          }
#line 3511 "parser.tab.c"
    break;

  case 46: /* instruction: TOKEN_DEALLOCATE  */
#line 392 "parser.y"
          { Instr_None(DEALLOCATE);
	  }
#line 3518 "parser.tab.c"
    break;

  case 47: /* $@26: %empty  */
#line 395 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3525 "parser.tab.c"
    break;

  case 48: /* instruction: TOKEN_META_CALL LPAR NUMBER $@26 RPAR  */
#line 397 "parser.y"
          { Instr_Index(META_CALL,index1);
          }
#line 3532 "parser.tab.c"
    break;

  case 49: /* $@27: %empty  */
#line 400 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 3539 "parser.tab.c"
    break;

  case 50: /* $@28: %empty  */
#line 402 "parser.y"
          { arity = atoi(yytext);
          }
#line 3546 "parser.tab.c"
    break;

  case 51: /* instruction: TOKEN_CALL LPAR ATOMNAME $@27 SLASH NUMBER $@28 RPAR  */
#line 404 "parser.y"
          { Instr_Pred(CALL,GetPred(fname,arity));
	  }
#line 3553 "parser.tab.c"
    break;

  case 52: /* $@29: %empty  */
#line 407 "parser.y"
          { label = atoi(yytext);
	  }
#line 3560 "parser.tab.c"
    break;

  case 53: /* $@30: %empty  */
#line 409 "parser.y"
          { arity = atoi(yytext);
          }
#line 3567 "parser.tab.c"
    break;

  case 54: /* instruction: TOKEN_CALL LPAR NUMBER $@29 SLASH NUMBER $@30 RPAR  */
#line 411 "parser.y"
          { Instr_Pred(CALL,GetAbstraction(label));
	  }
#line 3574 "parser.tab.c"
    break;

  case 55: /* $@31: %empty  */
#line 414 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 3581 "parser.tab.c"
    break;

  case 56: /* $@32: %empty  */
#line 416 "parser.y"
          { arity = atoi(yytext);
          }
#line 3588 "parser.tab.c"
    break;

  case 57: /* instruction: TOKEN_EXECUTE LPAR ATOMNAME $@31 SLASH NUMBER $@32 RPAR  */
#line 418 "parser.y"
          { Instr_Pred(EXECUTE,GetPred(fname,arity));
          }
#line 3595 "parser.tab.c"
    break;

  case 58: /* $@33: %empty  */
#line 421 "parser.y"
          { label = atoi(yytext);
	  }
#line 3602 "parser.tab.c"
    break;

  case 59: /* $@34: %empty  */
#line 423 "parser.y"
          { arity = atoi(yytext);
          }
#line 3609 "parser.tab.c"
    break;

  case 60: /* instruction: TOKEN_EXECUTE LPAR NUMBER $@33 SLASH NUMBER $@34 RPAR  */
#line 425 "parser.y"
          { Instr_Pred(EXECUTE,GetAbstraction(label));
          }
#line 3616 "parser.tab.c"
    break;

  case 61: /* $@35: %empty  */
#line 428 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 3623 "parser.tab.c"
    break;

  case 62: /* $@36: %empty  */
#line 430 "parser.y"
          { arity = atoi(yytext);
          }
#line 3630 "parser.tab.c"
    break;

  case 63: /* instruction: TOKEN_DEALLOC_EXECUTE LPAR ATOMNAME $@35 SLASH NUMBER $@36 RPAR  */
#line 432 "parser.y"
          { Instr_Pred(DEALLOC_EXECUTE,GetPred(fname,arity));
          }
#line 3637 "parser.tab.c"
    break;

  case 64: /* $@37: %empty  */
#line 435 "parser.y"
          { label = atoi(yytext);
	  }
#line 3644 "parser.tab.c"
    break;

  case 65: /* $@38: %empty  */
#line 437 "parser.y"
          { arity = atoi(yytext);
          }
#line 3651 "parser.tab.c"
    break;

  case 66: /* instruction: TOKEN_DEALLOC_EXECUTE LPAR NUMBER $@37 SLASH NUMBER $@38 RPAR  */
#line 439 "parser.y"
          { Instr_Pred(DEALLOC_EXECUTE,GetAbstraction(label));
          }
#line 3658 "parser.tab.c"
    break;

  case 67: /* $@39: %empty  */
#line 442 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 3665 "parser.tab.c"
    break;

  case 68: /* $@40: %empty  */
#line 444 "parser.y"
          { arity = atoi(yytext);
          }
#line 3672 "parser.tab.c"
    break;

  case 69: /* instruction: TOKEN_EXTERN_CALL LPAR ATOMNAME $@39 SLASH NUMBER $@40 COMMA ATOMNAME RPAR  */
#line 446 "parser.y"
          { Instr_Pred(CALL,GetPred(fname,arity));
	  }
#line 3679 "parser.tab.c"
    break;

  case 70: /* $@41: %empty  */
#line 449 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 3686 "parser.tab.c"
    break;

  case 71: /* $@42: %empty  */
#line 451 "parser.y"
          { arity = atoi(yytext);
          }
#line 3693 "parser.tab.c"
    break;

  case 72: /* instruction: TOKEN_EXTERN_EXECUTE LPAR ATOMNAME $@41 SLASH NUMBER $@42 COMMA ATOMNAME RPAR  */
#line 453 "parser.y"
          { Instr_Pred(EXECUTE,GetPred(fname,arity));
          }
#line 3700 "parser.tab.c"
    break;

  case 73: /* instruction: TOKEN_PROCEED  */
#line 456 "parser.y"
          { Instr_None(PROCEED);
	  }
#line 3707 "parser.tab.c"
    break;

  case 74: /* instruction: TOKEN_DEALLOC_PROCEED  */
#line 459 "parser.y"
          { Instr_None(DEALLOC_PROCEED);
	  }
#line 3714 "parser.tab.c"
    break;

  case 75: /* $@43: %empty  */
#line 462 "parser.y"
          { index1 = atoi(yytext);
          }
#line 3721 "parser.tab.c"
    break;

  case 76: /* $@44: %empty  */
#line 464 "parser.y"
          { size = atoi(yytext);
          }
#line 3728 "parser.tab.c"
    break;

  case 77: /* instruction: TOKEN_CALL_APPLY LPAR NUMBER $@43 COMMA NUMBER $@44 RPAR  */
#line 466 "parser.y"
          { Instr_Index_Size(CALL_APPLY,index1,size);
	  }
#line 3735 "parser.tab.c"
    break;

  case 78: /* $@45: %empty  */
#line 469 "parser.y"
          { index1 = atoi(yytext);
          }
#line 3742 "parser.tab.c"
    break;

  case 79: /* $@46: %empty  */
#line 471 "parser.y"
          { size = atoi(yytext);
          }
#line 3749 "parser.tab.c"
    break;

  case 80: /* instruction: TOKEN_EXECUTE_APPLY LPAR NUMBER $@45 COMMA NUMBER $@46 RPAR  */
#line 473 "parser.y"
          { Instr_Index_Size(EXECUTE_APPLY,index1,size);
	  }
#line 3756 "parser.tab.c"
    break;

  case 81: /* $@47: %empty  */
#line 476 "parser.y"
          { index1 = atoi(yytext);
          }
#line 3763 "parser.tab.c"
    break;

  case 82: /* $@48: %empty  */
#line 478 "parser.y"
          { size = atoi(yytext);
          }
#line 3770 "parser.tab.c"
    break;

  case 83: /* instruction: TOKEN_DEALLOC_EXECUTE_APPLY LPAR NUMBER $@47 COMMA NUMBER $@48 RPAR  */
#line 480 "parser.y"
          { Instr_Index_Size(DEALLOC_EXECUTE_APPLY,index1,size);
	  }
#line 3777 "parser.tab.c"
    break;

  case 84: /* instruction: TOKEN_FAIL  */
#line 483 "parser.y"
          { Instr_None(FAIL)
	  }
#line 3784 "parser.tab.c"
    break;

  case 85: /* $@49: %empty  */
#line 486 "parser.y"
          { index1 =  atoi(yytext);
	  }
#line 3791 "parser.tab.c"
    break;

  case 86: /* $@50: %empty  */
#line 488 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 3798 "parser.tab.c"
    break;

  case 87: /* instruction: TOKEN_GET_X_VARIABLE LPAR NUMBER $@49 COMMA NUMBER $@50 RPAR  */
#line 490 "parser.y"
          { Instr_Index_Index(GET_X_VARIABLE,index1,index2);
          }
#line 3805 "parser.tab.c"
    break;

  case 88: /* $@51: %empty  */
#line 493 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3812 "parser.tab.c"
    break;

  case 89: /* $@52: %empty  */
#line 495 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 3819 "parser.tab.c"
    break;

  case 90: /* instruction: TOKEN_GET_Y_VARIABLE LPAR NUMBER $@51 COMMA NUMBER $@52 RPAR  */
#line 497 "parser.y"
          { Instr_Index_Index(GET_Y_VARIABLE,index1,index2);
          }
#line 3826 "parser.tab.c"
    break;

  case 91: /* $@53: %empty  */
#line 500 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3833 "parser.tab.c"
    break;

  case 92: /* $@54: %empty  */
#line 502 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(GET2_Y_VARIABLE,index1,index2);
	  }
#line 3841 "parser.tab.c"
    break;

  case 93: /* $@55: %empty  */
#line 505 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3848 "parser.tab.c"
    break;

  case 94: /* $@56: %empty  */
#line 507 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3856 "parser.tab.c"
    break;

  case 96: /* $@57: %empty  */
#line 511 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3863 "parser.tab.c"
    break;

  case 97: /* $@58: %empty  */
#line 513 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(GET3_Y_VARIABLE,index1,index2);
	  }
#line 3871 "parser.tab.c"
    break;

  case 98: /* $@59: %empty  */
#line 516 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3878 "parser.tab.c"
    break;

  case 99: /* $@60: %empty  */
#line 518 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3886 "parser.tab.c"
    break;

  case 100: /* $@61: %empty  */
#line 521 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3893 "parser.tab.c"
    break;

  case 101: /* $@62: %empty  */
#line 523 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3901 "parser.tab.c"
    break;

  case 103: /* $@63: %empty  */
#line 527 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3908 "parser.tab.c"
    break;

  case 104: /* $@64: %empty  */
#line 529 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(GET4_Y_VARIABLE,index1,index2);
	  }
#line 3916 "parser.tab.c"
    break;

  case 105: /* $@65: %empty  */
#line 532 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3923 "parser.tab.c"
    break;

  case 106: /* $@66: %empty  */
#line 534 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3931 "parser.tab.c"
    break;

  case 107: /* $@67: %empty  */
#line 537 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3938 "parser.tab.c"
    break;

  case 108: /* $@68: %empty  */
#line 539 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3946 "parser.tab.c"
    break;

  case 109: /* $@69: %empty  */
#line 542 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3953 "parser.tab.c"
    break;

  case 110: /* $@70: %empty  */
#line 544 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3961 "parser.tab.c"
    break;

  case 112: /* $@71: %empty  */
#line 548 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3968 "parser.tab.c"
    break;

  case 113: /* $@72: %empty  */
#line 550 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(GET5_Y_VARIABLE,index1,index2);
	  }
#line 3976 "parser.tab.c"
    break;

  case 114: /* $@73: %empty  */
#line 553 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3983 "parser.tab.c"
    break;

  case 115: /* $@74: %empty  */
#line 555 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 3991 "parser.tab.c"
    break;

  case 116: /* $@75: %empty  */
#line 558 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 3998 "parser.tab.c"
    break;

  case 117: /* $@76: %empty  */
#line 560 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4006 "parser.tab.c"
    break;

  case 118: /* $@77: %empty  */
#line 563 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4013 "parser.tab.c"
    break;

  case 119: /* $@78: %empty  */
#line 565 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4021 "parser.tab.c"
    break;

  case 120: /* $@79: %empty  */
#line 568 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4028 "parser.tab.c"
    break;

  case 121: /* $@80: %empty  */
#line 570 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4036 "parser.tab.c"
    break;

  case 123: /* $@81: %empty  */
#line 574 "parser.y"
          { index1 =  atoi(yytext);
	  }
#line 4043 "parser.tab.c"
    break;

  case 124: /* $@82: %empty  */
#line 576 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 4050 "parser.tab.c"
    break;

  case 125: /* instruction: TOKEN_GET_X_VALUE LPAR NUMBER $@81 COMMA NUMBER $@82 RPAR  */
#line 578 "parser.y"
          { Instr_Index_Index(GET_X_VALUE,index1,index2);
          }
#line 4057 "parser.tab.c"
    break;

  case 126: /* $@83: %empty  */
#line 581 "parser.y"
          { index1 =  atoi(yytext);
	  }
#line 4064 "parser.tab.c"
    break;

  case 127: /* $@84: %empty  */
#line 583 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 4071 "parser.tab.c"
    break;

  case 128: /* instruction: TOKEN_GET_Y_VALUE LPAR NUMBER $@83 COMMA NUMBER $@84 RPAR  */
#line 585 "parser.y"
          { Instr_Index_Index(GET_Y_VALUE,index1,index2);
          }
#line 4078 "parser.tab.c"
    break;

  case 129: /* $@85: %empty  */
#line 588 "parser.y"
          { const_term = ConstInt(yytext);
	  }
#line 4085 "parser.tab.c"
    break;

  case 130: /* $@86: %empty  */
#line 590 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4092 "parser.tab.c"
    break;

  case 131: /* instruction: TOKEN_GET_CONSTANT LPAR NUMBER $@85 COMMA NUMBER $@86 RPAR  */
#line 592 "parser.y"
          { Instr_Term_Index(GET_CONSTANT,const_term,index1);
          }
#line 4099 "parser.tab.c"
    break;

  case 132: /* $@87: %empty  */
#line 595 "parser.y"
          { const_term = ConstFlt(yytext);
	  }
#line 4106 "parser.tab.c"
    break;

  case 133: /* $@88: %empty  */
#line 597 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4113 "parser.tab.c"
    break;

  case 134: /* instruction: TOKEN_GET_CONSTANT LPAR FLOAT $@87 COMMA NUMBER $@88 RPAR  */
#line 599 "parser.y"
          { Instr_Term_Index(GET_CONSTANT,const_term,index1);
          }
#line 4120 "parser.tab.c"
    break;

  case 135: /* $@89: %empty  */
#line 602 "parser.y"
          { const_term = ConstAtom(yytext);
	  }
#line 4127 "parser.tab.c"
    break;

  case 136: /* $@90: %empty  */
#line 604 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4134 "parser.tab.c"
    break;

  case 137: /* instruction: TOKEN_GET_CONSTANT LPAR ATOMNAME $@89 COMMA NUMBER $@90 RPAR  */
#line 606 "parser.y"
          { Instr_Term_Index(GET_CONSTANT,const_term,index1);
          }
#line 4141 "parser.tab.c"
    break;

  case 138: /* $@91: %empty  */
#line 609 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4148 "parser.tab.c"
    break;

  case 139: /* instruction: TOKEN_GET_NIL LPAR NUMBER $@91 RPAR  */
#line 611 "parser.y"
          { Instr_Index(GET_NIL,index1);
          }
#line 4155 "parser.tab.c"
    break;

  case 140: /* $@92: %empty  */
#line 614 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4162 "parser.tab.c"
    break;

  case 141: /* instruction: TOKEN_GET_LIST LPAR NUMBER $@92 RPAR  */
#line 616 "parser.y"
          { Instr_Index(GET_LIST,index1);
          }
#line 4169 "parser.tab.c"
    break;

  case 142: /* $@93: %empty  */
#line 619 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4176 "parser.tab.c"
    break;

  case 143: /* $@94: %empty  */
#line 621 "parser.y"
          { arity = atoi(yytext);
	  }
#line 4183 "parser.tab.c"
    break;

  case 144: /* $@95: %empty  */
#line 623 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4190 "parser.tab.c"
    break;

  case 145: /* instruction: TOKEN_GET_STRUCTURE LPAR ATOMNAME $@93 SLASH NUMBER $@94 COMMA NUMBER $@95 RPAR  */
#line 625 "parser.y"
          { Instr_Functor_Index(GET_STRUCTURE,ConstFunctor(fname,arity),index1);
          }
#line 4197 "parser.tab.c"
    break;

  case 146: /* $@96: %empty  */
#line 628 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4204 "parser.tab.c"
    break;

  case 147: /* $@97: %empty  */
#line 630 "parser.y"
          { arity = atoi(yytext);	
	    features = 0;
	    hv = 0;
	    tbl = (Term*)(malloc(arity * sizeof(Term)));
	  }
#line 4214 "parser.tab.c"
    break;

  case 148: /* $@98: %empty  */
#line 635 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4221 "parser.tab.c"
    break;

  case 149: /* instruction: TOKEN_GET_RECORD LPAR ATOMNAME $@96 COMMA NUMBER $@97 COMMA LHPAR feat_const_list RHPAR COMMA NUMBER $@98 RPAR  */
#line 637 "parser.y"
          { if (features == 0)
	      hv = HashNameArity(Atm(fname),arity);
	   else 
	      hv = hv + (arity -features);
	    Instr_Functor_Index(GET_STRUCTURE,
	      create_functor(hv,fname,features,arity,tbl),index1);
	    free(tbl);
          }
#line 4234 "parser.tab.c"
    break;

  case 150: /* instruction: TOKEN_GET_NIL_X0  */
#line 646 "parser.y"
          { Instr_None(GET_NIL_X0);
	  }
#line 4241 "parser.tab.c"
    break;

  case 151: /* instruction: TOKEN_GET_LIST_X0  */
#line 649 "parser.y"
          { Instr_None(GET_LIST_X0);
	  }
#line 4248 "parser.tab.c"
    break;

  case 152: /* $@99: %empty  */
#line 652 "parser.y"
          { const_term = ConstInt(yytext);
	  }
#line 4255 "parser.tab.c"
    break;

  case 153: /* instruction: TOKEN_GET_CONSTANT_X0 LPAR NUMBER $@99 RPAR  */
#line 654 "parser.y"
          { Instr_Term(GET_CONSTANT_X0,const_term);
          }
#line 4262 "parser.tab.c"
    break;

  case 154: /* $@100: %empty  */
#line 657 "parser.y"
          { const_term = ConstFlt(yytext);
	  }
#line 4269 "parser.tab.c"
    break;

  case 155: /* instruction: TOKEN_GET_CONSTANT_X0 LPAR FLOAT $@100 RPAR  */
#line 659 "parser.y"
          { Instr_Term(GET_CONSTANT_X0,const_term);
          }
#line 4276 "parser.tab.c"
    break;

  case 156: /* $@101: %empty  */
#line 662 "parser.y"
          { const_term = ConstAtom(yytext);
	  }
#line 4283 "parser.tab.c"
    break;

  case 157: /* instruction: TOKEN_GET_CONSTANT_X0 LPAR ATOMNAME $@101 RPAR  */
#line 664 "parser.y"
          { Instr_Term(GET_CONSTANT_X0,const_term);
          }
#line 4290 "parser.tab.c"
    break;

  case 158: /* $@102: %empty  */
#line 667 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4297 "parser.tab.c"
    break;

  case 159: /* $@103: %empty  */
#line 669 "parser.y"
          { arity = atoi(yytext);
	  }
#line 4304 "parser.tab.c"
    break;

  case 160: /* instruction: TOKEN_GET_STRUCTURE_X0 LPAR ATOMNAME $@102 SLASH NUMBER $@103 RPAR  */
#line 671 "parser.y"
          { Instr_Functor(GET_STRUCTURE_X0,ConstFunctor(fname,arity));
	  }
#line 4311 "parser.tab.c"
    break;

  case 161: /* $@104: %empty  */
#line 674 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4318 "parser.tab.c"
    break;

  case 162: /* $@105: %empty  */
#line 676 "parser.y"
          { arity = atoi(yytext);
	  }
#line 4325 "parser.tab.c"
    break;

  case 163: /* $@106: %empty  */
#line 678 "parser.y"
          { index1 = atoi(yytext);
          }
#line 4332 "parser.tab.c"
    break;

  case 164: /* instruction: TOKEN_GET_ABSTRACTION LPAR ATOMNAME $@104 SLASH NUMBER $@105 COMMA NUMBER $@106 RPAR  */
#line 680 "parser.y"
          { Instr_Pred_Index(GET_ABSTRACTION,GetPred(fname,arity),index1);
          }
#line 4339 "parser.tab.c"
    break;

  case 165: /* $@107: %empty  */
#line 683 "parser.y"
          { label = atoi(yytext);
	  }
#line 4346 "parser.tab.c"
    break;

  case 166: /* $@108: %empty  */
#line 685 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4353 "parser.tab.c"
    break;

  case 167: /* instruction: TOKEN_GET_ABSTRACTION LPAR NUMBER $@107 COMMA NUMBER $@108 RPAR  */
#line 687 "parser.y"
          { Instr_Pred_Index(GET_ABSTRACTION,GetAbstraction(label),index1);
          }
#line 4360 "parser.tab.c"
    break;

  case 168: /* $@109: %empty  */
#line 690 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4367 "parser.tab.c"
    break;

  case 169: /* instruction: TOKEN_PUT_X_VOID LPAR NUMBER $@109 RPAR  */
#line 692 "parser.y"
          { Instr_Index(PUT_X_VOID,index1);
          }
#line 4374 "parser.tab.c"
    break;

  case 170: /* $@110: %empty  */
#line 695 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4381 "parser.tab.c"
    break;

  case 171: /* instruction: TOKEN_PUT_Y_VOID LPAR NUMBER $@110 RPAR  */
#line 697 "parser.y"
          { Instr_Index(PUT_Y_VOID,index1);
	  }
#line 4388 "parser.tab.c"
    break;

  case 172: /* $@111: %empty  */
#line 700 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4395 "parser.tab.c"
    break;

  case 173: /* $@112: %empty  */
#line 702 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 4402 "parser.tab.c"
    break;

  case 174: /* instruction: TOKEN_PUT_X_VARIABLE LPAR NUMBER $@111 COMMA NUMBER $@112 RPAR  */
#line 704 "parser.y"
          { Instr_Index_Index(PUT_X_VARIABLE,index1,index2);
          }
#line 4409 "parser.tab.c"
    break;

  case 175: /* $@113: %empty  */
#line 707 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4416 "parser.tab.c"
    break;

  case 176: /* $@114: %empty  */
#line 709 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 4423 "parser.tab.c"
    break;

  case 177: /* instruction: TOKEN_PUT_Y_VARIABLE LPAR NUMBER $@113 COMMA NUMBER $@114 RPAR  */
#line 711 "parser.y"
          { Instr_Index_Index(PUT_Y_VARIABLE,index1,index2);
          }
#line 4430 "parser.tab.c"
    break;

  case 178: /* $@115: %empty  */
#line 714 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4437 "parser.tab.c"
    break;

  case 179: /* $@116: %empty  */
#line 716 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 4444 "parser.tab.c"
    break;

  case 180: /* instruction: TOKEN_PUT_X_VALUE LPAR NUMBER $@115 COMMA NUMBER $@116 RPAR  */
#line 718 "parser.y"
          { Instr_Index_Index(PUT_X_VALUE,index1,index2);
          }
#line 4451 "parser.tab.c"
    break;

  case 181: /* $@117: %empty  */
#line 721 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4458 "parser.tab.c"
    break;

  case 182: /* $@118: %empty  */
#line 723 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 4465 "parser.tab.c"
    break;

  case 183: /* instruction: TOKEN_PUT_Y_VALUE LPAR NUMBER $@117 COMMA NUMBER $@118 RPAR  */
#line 725 "parser.y"
          { Instr_Index_Index(PUT_Y_VALUE,index1,index2);
          }
#line 4472 "parser.tab.c"
    break;

  case 184: /* $@119: %empty  */
#line 728 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4479 "parser.tab.c"
    break;

  case 185: /* $@120: %empty  */
#line 730 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(PUT2_Y_VALUE,index1,index2);
	  }
#line 4487 "parser.tab.c"
    break;

  case 186: /* $@121: %empty  */
#line 733 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4494 "parser.tab.c"
    break;

  case 187: /* $@122: %empty  */
#line 735 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4502 "parser.tab.c"
    break;

  case 189: /* $@123: %empty  */
#line 739 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4509 "parser.tab.c"
    break;

  case 190: /* $@124: %empty  */
#line 741 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(PUT3_Y_VALUE,index1,index2);
	  }
#line 4517 "parser.tab.c"
    break;

  case 191: /* $@125: %empty  */
#line 744 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4524 "parser.tab.c"
    break;

  case 192: /* $@126: %empty  */
#line 746 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4532 "parser.tab.c"
    break;

  case 193: /* $@127: %empty  */
#line 749 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4539 "parser.tab.c"
    break;

  case 194: /* $@128: %empty  */
#line 751 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4547 "parser.tab.c"
    break;

  case 196: /* $@129: %empty  */
#line 755 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4554 "parser.tab.c"
    break;

  case 197: /* $@130: %empty  */
#line 757 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(PUT4_Y_VALUE,index1,index2);
	  }
#line 4562 "parser.tab.c"
    break;

  case 198: /* $@131: %empty  */
#line 760 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4569 "parser.tab.c"
    break;

  case 199: /* $@132: %empty  */
#line 762 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4577 "parser.tab.c"
    break;

  case 200: /* $@133: %empty  */
#line 765 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4584 "parser.tab.c"
    break;

  case 201: /* $@134: %empty  */
#line 767 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4592 "parser.tab.c"
    break;

  case 202: /* $@135: %empty  */
#line 770 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4599 "parser.tab.c"
    break;

  case 203: /* $@136: %empty  */
#line 772 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4607 "parser.tab.c"
    break;

  case 205: /* $@137: %empty  */
#line 776 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4614 "parser.tab.c"
    break;

  case 206: /* $@138: %empty  */
#line 778 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(PUT5_Y_VALUE,index1,index2);
	  }
#line 4622 "parser.tab.c"
    break;

  case 207: /* $@139: %empty  */
#line 781 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4629 "parser.tab.c"
    break;

  case 208: /* $@140: %empty  */
#line 783 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4637 "parser.tab.c"
    break;

  case 209: /* $@141: %empty  */
#line 786 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4644 "parser.tab.c"
    break;

  case 210: /* $@142: %empty  */
#line 788 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4652 "parser.tab.c"
    break;

  case 211: /* $@143: %empty  */
#line 791 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4659 "parser.tab.c"
    break;

  case 212: /* $@144: %empty  */
#line 793 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4667 "parser.tab.c"
    break;

  case 213: /* $@145: %empty  */
#line 796 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4674 "parser.tab.c"
    break;

  case 214: /* $@146: %empty  */
#line 798 "parser.y"
          { index2 = atoi(yytext);
	    Next_Index_Index(index1, index2);
	  }
#line 4682 "parser.tab.c"
    break;

  case 216: /* $@147: %empty  */
#line 802 "parser.y"
          { const_term = ConstInt(yytext);
	  }
#line 4689 "parser.tab.c"
    break;

  case 217: /* $@148: %empty  */
#line 804 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4696 "parser.tab.c"
    break;

  case 218: /* instruction: TOKEN_PUT_CONSTANT LPAR NUMBER $@147 COMMA NUMBER $@148 RPAR  */
#line 806 "parser.y"
          { Instr_Term_Index(PUT_CONSTANT,const_term,index1);
          }
#line 4703 "parser.tab.c"
    break;

  case 219: /* $@149: %empty  */
#line 809 "parser.y"
          { const_term = ConstFlt(yytext);
	  }
#line 4710 "parser.tab.c"
    break;

  case 220: /* $@150: %empty  */
#line 811 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4717 "parser.tab.c"
    break;

  case 221: /* instruction: TOKEN_PUT_CONSTANT LPAR FLOAT $@149 COMMA NUMBER $@150 RPAR  */
#line 813 "parser.y"
          { Instr_Term_Index(PUT_CONSTANT,const_term,index1);
          }
#line 4724 "parser.tab.c"
    break;

  case 222: /* $@151: %empty  */
#line 816 "parser.y"
          { const_term = ConstAtom(yytext);
	  }
#line 4731 "parser.tab.c"
    break;

  case 223: /* $@152: %empty  */
#line 818 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4738 "parser.tab.c"
    break;

  case 224: /* instruction: TOKEN_PUT_CONSTANT LPAR ATOMNAME $@151 COMMA NUMBER $@152 RPAR  */
#line 820 "parser.y"
          { Instr_Term_Index(PUT_CONSTANT,const_term,index1);
          }
#line 4745 "parser.tab.c"
    break;

  case 225: /* $@153: %empty  */
#line 823 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4752 "parser.tab.c"
    break;

  case 226: /* instruction: TOKEN_PUT_NIL LPAR NUMBER $@153 RPAR  */
#line 825 "parser.y"
          { Instr_Index(PUT_NIL,index1);
          }
#line 4759 "parser.tab.c"
    break;

  case 227: /* $@154: %empty  */
#line 828 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4766 "parser.tab.c"
    break;

  case 228: /* $@155: %empty  */
#line 830 "parser.y"
          { arity = atoi(yytext);
	  }
#line 4773 "parser.tab.c"
    break;

  case 229: /* $@156: %empty  */
#line 832 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4780 "parser.tab.c"
    break;

  case 230: /* instruction: TOKEN_PUT_STRUCTURE LPAR ATOMNAME $@154 SLASH NUMBER $@155 COMMA NUMBER $@156 RPAR  */
#line 834 "parser.y"
          { Instr_Functor_Index(PUT_STRUCTURE,ConstFunctor(fname,arity),index1);
          }
#line 4787 "parser.tab.c"
    break;

  case 231: /* $@157: %empty  */
#line 837 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4794 "parser.tab.c"
    break;

  case 232: /* $@158: %empty  */
#line 839 "parser.y"
          { arity = atoi(yytext);	
	    features = 0;
	    hv = 0;
	    tbl = (Term*)(malloc(arity * sizeof(Term)));
	  }
#line 4804 "parser.tab.c"
    break;

  case 233: /* $@159: %empty  */
#line 844 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4811 "parser.tab.c"
    break;

  case 234: /* instruction: TOKEN_PUT_RECORD LPAR ATOMNAME $@157 COMMA NUMBER $@158 COMMA LHPAR feat_const_list RHPAR COMMA NUMBER $@159 RPAR  */
#line 846 "parser.y"
          { if (features == 0)
	      hv = HashNameArity(Atm(fname),arity);
	   else 
	      hv = hv + (arity -features);
	    Instr_Functor_Index(PUT_STRUCTURE,
	      create_functor(hv,fname,features,arity,tbl),index1);
	    free(tbl);
          }
#line 4824 "parser.tab.c"
    break;

  case 235: /* $@160: %empty  */
#line 855 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4831 "parser.tab.c"
    break;

  case 236: /* instruction: TOKEN_PUT_LIST LPAR NUMBER $@160 RPAR  */
#line 857 "parser.y"
          { Instr_Index(PUT_LIST,index1);
          }
#line 4838 "parser.tab.c"
    break;

  case 237: /* $@161: %empty  */
#line 860 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 4845 "parser.tab.c"
    break;

  case 238: /* $@162: %empty  */
#line 862 "parser.y"
          { arity = atoi(yytext);
	  }
#line 4852 "parser.tab.c"
    break;

  case 239: /* $@163: %empty  */
#line 864 "parser.y"
          { index1 = atoi(yytext);
          }
#line 4859 "parser.tab.c"
    break;

  case 240: /* instruction: TOKEN_PUT_ABSTRACTION LPAR ATOMNAME $@161 SLASH NUMBER $@162 COMMA NUMBER $@163 RPAR  */
#line 866 "parser.y"
          { Instr_Pred_Index(PUT_ABSTRACTION,GetPred(fname,arity),index1);
          }
#line 4866 "parser.tab.c"
    break;

  case 241: /* $@164: %empty  */
#line 869 "parser.y"
          { label = atoi(yytext);
	  }
#line 4873 "parser.tab.c"
    break;

  case 242: /* $@165: %empty  */
#line 871 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4880 "parser.tab.c"
    break;

  case 243: /* instruction: TOKEN_PUT_ABSTRACTION LPAR NUMBER $@164 COMMA NUMBER $@165 RPAR  */
#line 873 "parser.y"
          { Instr_Pred_Index(PUT_ABSTRACTION,GetAbstraction(label),index1);
          }
#line 4887 "parser.tab.c"
    break;

  case 244: /* instruction: TOKEN_UNIFY_VOID  */
#line 876 "parser.y"
          { Instr_None(UNIFY_VOID);
	  }
#line 4894 "parser.tab.c"
    break;

  case 245: /* $@166: %empty  */
#line 879 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4901 "parser.tab.c"
    break;

  case 246: /* instruction: TOKEN_UNIFY_X_VARIABLE LPAR NUMBER $@166 RPAR  */
#line 881 "parser.y"
          { Instr_Index(UNIFY_X_VARIABLE,index1);
          }
#line 4908 "parser.tab.c"
    break;

  case 247: /* $@167: %empty  */
#line 884 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4915 "parser.tab.c"
    break;

  case 248: /* instruction: TOKEN_UNIFY_Y_VARIABLE LPAR NUMBER $@167 RPAR  */
#line 886 "parser.y"
          { Instr_Index(UNIFY_Y_VARIABLE,index1);
          }
#line 4922 "parser.tab.c"
    break;

  case 249: /* $@168: %empty  */
#line 889 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4929 "parser.tab.c"
    break;

  case 250: /* instruction: TOKEN_UNIFY_X_VALUE LPAR NUMBER $@168 RPAR  */
#line 891 "parser.y"
          { Instr_Index(UNIFY_X_VALUE,index1);
	  }
#line 4936 "parser.tab.c"
    break;

  case 251: /* $@169: %empty  */
#line 894 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 4943 "parser.tab.c"
    break;

  case 252: /* instruction: TOKEN_UNIFY_Y_VALUE LPAR NUMBER $@169 RPAR  */
#line 896 "parser.y"
          { Instr_Index(UNIFY_Y_VALUE,index1);
	  }
#line 4950 "parser.tab.c"
    break;

  case 253: /* $@170: %empty  */
#line 899 "parser.y"
          { const_term = ConstInt(yytext);
	  }
#line 4957 "parser.tab.c"
    break;

  case 254: /* instruction: TOKEN_UNIFY_CONSTANT LPAR NUMBER $@170 RPAR  */
#line 901 "parser.y"
          { Instr_Term(UNIFY_CONSTANT,const_term);
          }
#line 4964 "parser.tab.c"
    break;

  case 255: /* $@171: %empty  */
#line 904 "parser.y"
          { const_term = ConstFlt(yytext);
	  }
#line 4971 "parser.tab.c"
    break;

  case 256: /* instruction: TOKEN_UNIFY_CONSTANT LPAR FLOAT $@171 RPAR  */
#line 906 "parser.y"
          { Instr_Term(UNIFY_CONSTANT,const_term);
          }
#line 4978 "parser.tab.c"
    break;

  case 257: /* $@172: %empty  */
#line 909 "parser.y"
          { const_term = ConstAtom(yytext);
	  }
#line 4985 "parser.tab.c"
    break;

  case 258: /* instruction: TOKEN_UNIFY_CONSTANT LPAR ATOMNAME $@172 RPAR  */
#line 911 "parser.y"
          { Instr_Term(UNIFY_CONSTANT,const_term);
          }
#line 4992 "parser.tab.c"
    break;

  case 259: /* instruction: TOKEN_UNIFY_NIL  */
#line 914 "parser.y"
          { Instr_None(UNIFY_NIL);
	  }
#line 4999 "parser.tab.c"
    break;

  case 260: /* instruction: TOKEN_UNIFY_LIST  */
#line 917 "parser.y"
          { Instr_None(UNIFY_LIST);
	  }
#line 5006 "parser.tab.c"
    break;

  case 261: /* $@173: %empty  */
#line 920 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 5013 "parser.tab.c"
    break;

  case 262: /* $@174: %empty  */
#line 922 "parser.y"
          { arity = atoi(yytext);
	  }
#line 5020 "parser.tab.c"
    break;

  case 263: /* instruction: TOKEN_UNIFY_STRUCTURE LPAR ATOMNAME $@173 SLASH NUMBER $@174 RPAR  */
#line 924 "parser.y"
          { Instr_Functor(UNIFY_STRUCTURE,ConstFunctor(fname,arity));
          }
#line 5027 "parser.tab.c"
    break;

  case 264: /* $@175: %empty  */
#line 927 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Size(SWITCH_ON_CONSTANT,tablesize);
	  }
#line 5035 "parser.tab.c"
    break;

  case 265: /* $@176: %empty  */
#line 930 "parser.y"
          { label = atoi(yytext);
	    sort_switch_table(tablesize);
	    Next_Label(label);
	  }
#line 5044 "parser.tab.c"
    break;

  case 267: /* $@177: %empty  */
#line 935 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Size(SWITCH_ON_STRUCTURE,tablesize);
	  }
#line 5052 "parser.tab.c"
    break;

  case 268: /* $@178: %empty  */
#line 938 "parser.y"
          { label = atoi(yytext);
	    sort_switch_table(tablesize);
	    Next_Label(label);
	  }
#line 5061 "parser.tab.c"
    break;

  case 270: /* instruction: TOKEN_SEND3  */
#line 943 "parser.y"
          { Instr_None(SEND3)
	  }
#line 5068 "parser.tab.c"
    break;

  case 271: /* $@179: %empty  */
#line 946 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5075 "parser.tab.c"
    break;

  case 272: /* $@180: %empty  */
#line 948 "parser.y"
          { label = atoi(yytext);
	  }
#line 5082 "parser.tab.c"
    break;

  case 273: /* $@181: %empty  */
#line 950 "parser.y"
          { size = atoi(yytext);
	    Instr_Index_Label_Size(SUSPEND_FLAT,index1,label,size);
	  }
#line 5090 "parser.tab.c"
    break;

  case 275: /* $@182: %empty  */
#line 954 "parser.y"
          { index1 = atoi(yytext);
	    number = fd_new_label();
	    Instr_Term_Index(PUT_CONSTANT,MakeSmallNum(number),index1);
	  }
#line 5099 "parser.tab.c"
    break;

  case 277: /* $@183: %empty  */
#line 959 "parser.y"
          { label = atoi(yytext);
	  }
#line 5106 "parser.tab.c"
    break;

  case 278: /* instruction: LABEL LPAR NUMBER $@183 RPAR  */
#line 962 "parser.y"
          { Label(label);
#ifdef BAM
	  }
#line 5114 "parser.tab.c"
    break;

  case 279: /* instruction: TOKEN_FAIL_DEC  */
#line 965 "parser.y"
                         { Instr_None(FAIL_DEC); }
#line 5120 "parser.tab.c"
    break;

  case 280: /* $@184: %empty  */
#line 967 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(LOAD_NIL,Bindex(index1));
	  }
#line 5128 "parser.tab.c"
    break;

  case 282: /* $@185: %empty  */
#line 971 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5135 "parser.tab.c"
    break;

  case 283: /* $@186: %empty  */
#line 973 "parser.y"
          { Instr_Index_Term(LOAD_ATOM,Bindex(index1),const_term);
	  }
#line 5142 "parser.tab.c"
    break;

  case 285: /* $@187: %empty  */
#line 976 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5149 "parser.tab.c"
    break;

  case 286: /* $@188: %empty  */
#line 978 "parser.y"
          { Instr_Index_Term(LOAD_INTEGER,Bindex(index1),const_term);
	  }
#line 5156 "parser.tab.c"
    break;

  case 288: /* $@189: %empty  */
#line 981 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5163 "parser.tab.c"
    break;

  case 289: /* $@190: %empty  */
#line 983 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(READ_CAR,Bindex(index1),Bindex(index2));
	  }
#line 5171 "parser.tab.c"
    break;

  case 291: /* $@191: %empty  */
#line 987 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5178 "parser.tab.c"
    break;

  case 292: /* $@192: %empty  */
#line 989 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(READ_CDR,Bindex(index1),Bindex(index2));
	  }
#line 5186 "parser.tab.c"
    break;

  case 294: /* $@193: %empty  */
#line 993 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5193 "parser.tab.c"
    break;

  case 295: /* $@194: %empty  */
#line 995 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 5200 "parser.tab.c"
    break;

  case 296: /* $@195: %empty  */
#line 997 "parser.y"
          { index3 = atoi(yytext);
	    Instr_Indices_3(READ_ARG,Bindex(index1),Bindex(index2),Bindex(index3));
	  }
#line 5208 "parser.tab.c"
    break;

  case 298: /* $@196: %empty  */
#line 1001 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5215 "parser.tab.c"
    break;

  case 299: /* $@197: %empty  */
#line 1003 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(SET_REG_REG,Bindex(index1),Bindex(index2));
	  }
#line 5223 "parser.tab.c"
    break;

  case 301: /* $@198: %empty  */
#line 1007 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5230 "parser.tab.c"
    break;

  case 302: /* $@199: %empty  */
#line 1009 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(SET_REG_YVAR,Bindex(index1),Bindex(index2));
	  }
#line 5238 "parser.tab.c"
    break;

  case 304: /* $@200: %empty  */
#line 1013 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5245 "parser.tab.c"
    break;

  case 305: /* $@201: %empty  */
#line 1015 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(SET_YVAR_REG,Bindex(index1),Bindex(index2));
	  }
#line 5253 "parser.tab.c"
    break;

  case 307: /* $@202: %empty  */
#line 1019 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5260 "parser.tab.c"
    break;

  case 308: /* $@203: %empty  */
#line 1021 "parser.y"
          { label1 = label;
          }
#line 5267 "parser.tab.c"
    break;

  case 309: /* $@204: %empty  */
#line 1023 "parser.y"
          { label2 = label;
	  }
#line 5274 "parser.tab.c"
    break;

  case 310: /* $@205: %empty  */
#line 1025 "parser.y"
          { label3 = label;
	  }
#line 5281 "parser.tab.c"
    break;

  case 311: /* $@206: %empty  */
#line 1027 "parser.y"
          { label4 = label;
	  }
#line 5288 "parser.tab.c"
    break;

  case 312: /* $@207: %empty  */
#line 1029 "parser.y"
          { label5 = label;
	    Instr_Index_Labels_5(TERM_SWITCH,Bindex(index1),
		label1,label2,label3,label4,label5);
	  }
#line 5297 "parser.tab.c"
    break;

  case 314: /* $@208: %empty  */
#line 1034 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5304 "parser.tab.c"
    break;

  case 315: /* $@209: %empty  */
#line 1036 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(ATOM_SWITCH,Bindex(index1),tablesize);
	  }
#line 5312 "parser.tab.c"
    break;

  case 316: /* $@210: %empty  */
#line 1039 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5319 "parser.tab.c"
    break;

  case 318: /* $@211: %empty  */
#line 1042 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5326 "parser.tab.c"
    break;

  case 319: /* $@212: %empty  */
#line 1044 "parser.y"
          { label1 = label;
	  }
#line 5333 "parser.tab.c"
    break;

  case 320: /* $@213: %empty  */
#line 1046 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Label_Size(ATOM_WITH_ELSE_SWITCH,Bindex(index1),label1,tablesize);
	  }
#line 5341 "parser.tab.c"
    break;

  case 321: /* $@214: %empty  */
#line 1049 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5348 "parser.tab.c"
    break;

  case 323: /* $@215: %empty  */
#line 1052 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5355 "parser.tab.c"
    break;

  case 324: /* $@216: %empty  */
#line 1054 "parser.y"
          { label1 = label;
	  }
#line 5362 "parser.tab.c"
    break;

  case 325: /* $@217: %empty  */
#line 1056 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Label_Size(ATOM_WITH_ATOM_ELSE_SWITCH,Bindex(index1),label1,tablesize);
	  }
#line 5370 "parser.tab.c"
    break;

  case 326: /* $@218: %empty  */
#line 1059 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5377 "parser.tab.c"
    break;

  case 328: /* $@219: %empty  */
#line 1062 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5384 "parser.tab.c"
    break;

  case 329: /* $@220: %empty  */
#line 1064 "parser.y"
          { label1 = label;
	    Instr_Index_Label(ATOM_TYPE,Bindex(index1),label1);
	  }
#line 5392 "parser.tab.c"
    break;

  case 331: /* $@221: %empty  */
#line 1068 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5399 "parser.tab.c"
    break;

  case 332: /* $@222: %empty  */
#line 1070 "parser.y"
          { label1 = label;
	  }
#line 5406 "parser.tab.c"
    break;

  case 333: /* $@223: %empty  */
#line 1072 "parser.y"
          { label2 = label;
	    Instr_Index_Label_Label(ATOM_TYPE_WITH_ELSE,Bindex(index1),label1,label2);
	  }
#line 5414 "parser.tab.c"
    break;

  case 335: /* $@224: %empty  */
#line 1076 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5421 "parser.tab.c"
    break;

  case 336: /* $@225: %empty  */
#line 1078 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(KNOWN_ATOM_SWITCH,Bindex(index1),tablesize);
	  }
#line 5429 "parser.tab.c"
    break;

  case 337: /* $@226: %empty  */
#line 1081 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5436 "parser.tab.c"
    break;

  case 339: /* $@227: %empty  */
#line 1084 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5443 "parser.tab.c"
    break;

  case 340: /* $@228: %empty  */
#line 1086 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH,Bindex(index1),tablesize);
	  }
#line 5451 "parser.tab.c"
    break;

  case 341: /* $@229: %empty  */
#line 1089 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5458 "parser.tab.c"
    break;

  case 343: /* $@230: %empty  */
#line 1092 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5465 "parser.tab.c"
    break;

  case 344: /* $@231: %empty  */
#line 1094 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(INTEGER_SWITCH,Bindex(index1),tablesize);
	  }
#line 5473 "parser.tab.c"
    break;

  case 345: /* $@232: %empty  */
#line 1097 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5480 "parser.tab.c"
    break;

  case 347: /* $@233: %empty  */
#line 1100 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5487 "parser.tab.c"
    break;

  case 348: /* $@234: %empty  */
#line 1102 "parser.y"
          { label1 = label;
	  }
#line 5494 "parser.tab.c"
    break;

  case 349: /* $@235: %empty  */
#line 1104 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Label_Size(INTEGER_WITH_ELSE_SWITCH,Bindex(index1),label1,tablesize);
	  }
#line 5502 "parser.tab.c"
    break;

  case 350: /* $@236: %empty  */
#line 1107 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5509 "parser.tab.c"
    break;

  case 352: /* $@237: %empty  */
#line 1110 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5516 "parser.tab.c"
    break;

  case 353: /* $@238: %empty  */
#line 1112 "parser.y"
          { label1 = label;
	  }
#line 5523 "parser.tab.c"
    break;

  case 354: /* $@239: %empty  */
#line 1114 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Label_Size(INTEGER_WITH_INTEGER_ELSE_SWITCH,Bindex(index1),label1,tablesize);
	  }
#line 5531 "parser.tab.c"
    break;

  case 355: /* $@240: %empty  */
#line 1117 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5538 "parser.tab.c"
    break;

  case 357: /* $@241: %empty  */
#line 1120 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5545 "parser.tab.c"
    break;

  case 358: /* $@242: %empty  */
#line 1122 "parser.y"
          { label1 = label;
	    Instr_Index_Label(INTEGER_TYPE,Bindex(index1),label1);
	  }
#line 5553 "parser.tab.c"
    break;

  case 360: /* $@243: %empty  */
#line 1126 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5560 "parser.tab.c"
    break;

  case 361: /* $@244: %empty  */
#line 1128 "parser.y"
          { label1 = label;
	  }
#line 5567 "parser.tab.c"
    break;

  case 362: /* $@245: %empty  */
#line 1130 "parser.y"
          { label2 = label;
	    Instr_Index_Label_Label(INTEGER_TYPE_WITH_ELSE,Bindex(index1),label1,label2);
	  }
#line 5575 "parser.tab.c"
    break;

  case 364: /* $@246: %empty  */
#line 1134 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5582 "parser.tab.c"
    break;

  case 365: /* $@247: %empty  */
#line 1136 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(KNOWN_INTEGER_SWITCH,Bindex(index1),tablesize);
	  }
#line 5590 "parser.tab.c"
    break;

  case 366: /* $@248: %empty  */
#line 1139 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5597 "parser.tab.c"
    break;

  case 368: /* $@249: %empty  */
#line 1142 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5604 "parser.tab.c"
    break;

  case 369: /* $@250: %empty  */
#line 1144 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH,Bindex(index1),tablesize);
	  }
#line 5612 "parser.tab.c"
    break;

  case 370: /* $@251: %empty  */
#line 1147 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5619 "parser.tab.c"
    break;

  case 372: /* $@252: %empty  */
#line 1150 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5626 "parser.tab.c"
    break;

  case 373: /* $@253: %empty  */
#line 1152 "parser.y"
          { label1 = label;
	  }
#line 5633 "parser.tab.c"
    break;

  case 374: /* $@254: %empty  */
#line 1154 "parser.y"
          { label2 = label;
	    Instr_Index_Label_Label(LIST_SWITCH,Bindex(index1),label1,label2);
	  }
#line 5641 "parser.tab.c"
    break;

  case 376: /* $@255: %empty  */
#line 1158 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5648 "parser.tab.c"
    break;

  case 377: /* $@256: %empty  */
#line 1160 "parser.y"
          { label1 = label;
	  }
#line 5655 "parser.tab.c"
    break;

  case 378: /* $@257: %empty  */
#line 1162 "parser.y"
          { label2 = label;
	    Instr_Index_Label_Label(LIST_TYPE_WITH_ELSE,Bindex(index1),label1,label2);
	  }
#line 5663 "parser.tab.c"
    break;

  case 380: /* $@258: %empty  */
#line 1166 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5670 "parser.tab.c"
    break;

  case 381: /* $@259: %empty  */
#line 1168 "parser.y"
          { label1 = label;
	    Instr_Index_Label(STRUCT_TYPE,Bindex(index1),label1);
	  }
#line 5678 "parser.tab.c"
    break;

  case 383: /* $@260: %empty  */
#line 1172 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5685 "parser.tab.c"
    break;

  case 384: /* $@261: %empty  */
#line 1174 "parser.y"
          { label1 = label;
	  }
#line 5692 "parser.tab.c"
    break;

  case 385: /* $@262: %empty  */
#line 1176 "parser.y"
          { label2 = label;
	    Instr_Index_Label_Label(STRUCT_TYPE_WITH_ELSE,Bindex(index1),label1,label2);
	  }
#line 5700 "parser.tab.c"
    break;

  case 387: /* $@263: %empty  */
#line 1180 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5707 "parser.tab.c"
    break;

  case 388: /* $@264: %empty  */
#line 1182 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(KNOWN_FUNCTOR_SWITCH,Bindex(index1),tablesize);
	  }
#line 5715 "parser.tab.c"
    break;

  case 389: /* $@265: %empty  */
#line 1185 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5722 "parser.tab.c"
    break;

  case 391: /* $@266: %empty  */
#line 1188 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5729 "parser.tab.c"
    break;

  case 392: /* $@267: %empty  */
#line 1190 "parser.y"
          { tablesize = atoi(yytext);
	    Instr_Index_Size(KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH,Bindex(index1),tablesize);
	  }
#line 5737 "parser.tab.c"
    break;

  case 393: /* $@268: %empty  */
#line 1193 "parser.y"
          { sort_switch_table(tablesize);
	  }
#line 5744 "parser.tab.c"
    break;

  case 395: /* $@269: %empty  */
#line 1196 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5751 "parser.tab.c"
    break;

  case 396: /* $@270: %empty  */
#line 1198 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 5758 "parser.tab.c"
    break;

  case 397: /* $@271: %empty  */
#line 1200 "parser.y"
          { label1 = label;
	  }
#line 5765 "parser.tab.c"
    break;

  case 398: /* instruction: TOKEN_INT_COMPARE_REG_REG LPAR NUMBER $@269 COMMA NUMBER $@270 COMMA label_dest $@271 COMMA label_dest RPAR  */
#line 1202 "parser.y"
          { label2 = label;
	    Instr_Index_Index_Label_Label(INT_COMPARE_REG_REG,
		Bindex(index1),Bindex(index2),label1,label2);
	  }
#line 5774 "parser.tab.c"
    break;

  case 399: /* $@272: %empty  */
#line 1207 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5781 "parser.tab.c"
    break;

  case 400: /* $@273: %empty  */
#line 1209 "parser.y"
          { label1 = label;
	  }
#line 5788 "parser.tab.c"
    break;

  case 401: /* instruction: TOKEN_INT_COMPARE_REG_INT LPAR NUMBER $@272 COMMA bam_const COMMA label_dest $@273 COMMA label_dest RPAR  */
#line 1211 "parser.y"
          { label2 = label;
	    Instr_Index_Term_Label_Label(INT_COMPARE_REG_INT,
		Bindex(index1),const_term,label1,label2);
	  }
#line 5797 "parser.tab.c"
    break;

  case 402: /* $@274: %empty  */
#line 1216 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5804 "parser.tab.c"
    break;

  case 403: /* $@275: %empty  */
#line 1218 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 5811 "parser.tab.c"
    break;

  case 404: /* $@276: %empty  */
#line 1220 "parser.y"
          { label1 = label;
	  }
#line 5818 "parser.tab.c"
    break;

  case 405: /* instruction: TOKEN_EQ_REG_REG LPAR NUMBER $@274 COMMA NUMBER $@275 COMMA label_dest $@276 COMMA label_dest RPAR  */
#line 1222 "parser.y"
          { label2 = label;
	    Instr_Index_Index_Label_Label(EQ_REG_REG,
		Bindex(index1),Bindex(index2),label1,label2);
	  }
#line 5827 "parser.tab.c"
    break;

  case 406: /* $@277: %empty  */
#line 1227 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5834 "parser.tab.c"
    break;

  case 407: /* $@278: %empty  */
#line 1229 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(MATCH_REG_REG,Bindex(index1),Bindex(index2));
	  }
#line 5842 "parser.tab.c"
    break;

  case 409: /* $@279: %empty  */
#line 1233 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5849 "parser.tab.c"
    break;

  case 410: /* $@280: %empty  */
#line 1235 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 5856 "parser.tab.c"
    break;

  case 411: /* $@281: %empty  */
#line 1237 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Index_Offset(MATCH_REG_REG_OFF,Bindex(index1),Bindex(index2),offset1);
	  }
#line 5864 "parser.tab.c"
    break;

  case 413: /* $@282: %empty  */
#line 1241 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5871 "parser.tab.c"
    break;

  case 414: /* $@283: %empty  */
#line 1243 "parser.y"
          { Instr_Index_Term(MATCH_REG_ATOM,Bindex(index1),const_term);
	  }
#line 5878 "parser.tab.c"
    break;

  case 416: /* $@284: %empty  */
#line 1246 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5885 "parser.tab.c"
    break;

  case 417: /* $@285: %empty  */
#line 1248 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 5892 "parser.tab.c"
    break;

  case 418: /* instruction: TOKEN_MATCH_REG_OFF_ATOM LPAR NUMBER $@284 COMMA NUMBER $@285 COMMA bam_const RPAR  */
#line 1250 "parser.y"
          { Instr_Index_Offset_Term(MATCH_REG_OFF_ATOM,Bindex(index1),offset1,const_term);
	  }
#line 5899 "parser.tab.c"
    break;

  case 419: /* $@286: %empty  */
#line 1253 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5906 "parser.tab.c"
    break;

  case 420: /* $@287: %empty  */
#line 1255 "parser.y"
          { Instr_Index_Term(MATCH_REG_INTEGER,Bindex(index1),const_term);
	  }
#line 5913 "parser.tab.c"
    break;

  case 422: /* $@288: %empty  */
#line 1258 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5920 "parser.tab.c"
    break;

  case 423: /* $@289: %empty  */
#line 1260 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 5927 "parser.tab.c"
    break;

  case 424: /* instruction: TOKEN_MATCH_REG_OFF_INTEGER LPAR NUMBER $@288 COMMA NUMBER $@289 COMMA bam_const RPAR  */
#line 1262 "parser.y"
          { Instr_Index_Offset_Term(MATCH_REG_OFF_INTEGER,Bindex(index1),offset1,const_term);
	  }
#line 5934 "parser.tab.c"
    break;

  case 425: /* $@290: %empty  */
#line 1265 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(MATCH_REG_NIL,Bindex(index1));
	  }
#line 5942 "parser.tab.c"
    break;

  case 427: /* $@291: %empty  */
#line 1269 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5949 "parser.tab.c"
    break;

  case 428: /* $@292: %empty  */
#line 1271 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Offset(MATCH_REG_OFF_NIL,Bindex(index1),offset1);
	  }
#line 5957 "parser.tab.c"
    break;

  case 430: /* $@293: %empty  */
#line 1275 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5964 "parser.tab.c"
    break;

  case 431: /* $@294: %empty  */
#line 1277 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(MATCH_REG_YVAR,Bindex(index1),Bindex(index2));
	  }
#line 5972 "parser.tab.c"
    break;

  case 433: /* $@295: %empty  */
#line 1281 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5979 "parser.tab.c"
    break;

  case 434: /* $@296: %empty  */
#line 1283 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(MATCH_YVAR_YVAR,Bindex(index1),Bindex(index2));
	  }
#line 5987 "parser.tab.c"
    break;

  case 436: /* $@297: %empty  */
#line 1287 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 5994 "parser.tab.c"
    break;

  case 437: /* $@298: %empty  */
#line 1289 "parser.y"
          { index2 = atoi(yytext);
	  }
#line 6001 "parser.tab.c"
    break;

  case 438: /* $@299: %empty  */
#line 1291 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Index_Offset(MATCH_YVAR_REG_OFF,Bindex(index1),Bindex(index2),offset1);
	  }
#line 6009 "parser.tab.c"
    break;

  case 440: /* $@300: %empty  */
#line 1295 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6016 "parser.tab.c"
    break;

  case 441: /* $@301: %empty  */
#line 1297 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Offset(MATCH_REG_H_LIST,Bindex(index1),offset1);
	  }
#line 6024 "parser.tab.c"
    break;

  case 443: /* $@302: %empty  */
#line 1301 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6031 "parser.tab.c"
    break;

  case 444: /* $@303: %empty  */
#line 1303 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Offset(MATCH_YVAR_H_LIST,Bindex(index1),offset1);
	  }
#line 6039 "parser.tab.c"
    break;

  case 446: /* $@304: %empty  */
#line 1307 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6046 "parser.tab.c"
    break;

  case 447: /* $@305: %empty  */
#line 1309 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Offset(MATCH_REG_H_STRUCT,Bindex(index1),offset1);
	  }
#line 6054 "parser.tab.c"
    break;

  case 449: /* $@306: %empty  */
#line 1313 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6061 "parser.tab.c"
    break;

  case 450: /* $@307: %empty  */
#line 1315 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Index_Offset(MATCH_YVAR_H_STRUCT,Bindex(index1),offset1);
	  }
#line 6069 "parser.tab.c"
    break;

  case 452: /* $@308: %empty  */
#line 1319 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6076 "parser.tab.c"
    break;

  case 453: /* $@309: %empty  */
#line 1321 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6083 "parser.tab.c"
    break;

  case 454: /* $@310: %empty  */
#line 1323 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Functor_Label_Offset_Index(MATCH_REG_STRUCT,
		Bindex(index1),ConstFunctor(name,arity),
		label,offset1,Bindex(index2));
	  }
#line 6093 "parser.tab.c"
    break;

  case 456: /* $@311: %empty  */
#line 1329 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6100 "parser.tab.c"
    break;

  case 457: /* $@312: %empty  */
#line 1331 "parser.y"
          { number = atoi(yytext);
	  }
#line 6107 "parser.tab.c"
    break;

  case 458: /* $@313: %empty  */
#line 1333 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6114 "parser.tab.c"
    break;

  case 459: /* $@314: %empty  */
#line 1335 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Functor_Label_Level_Offset_Index(MATCH_REG_STRUCT_LEVEL,
		Bindex(index1),ConstFunctor(name,arity),label,number,
		offset1,Bindex(index2));
	  }
#line 6124 "parser.tab.c"
    break;

  case 461: /* $@315: %empty  */
#line 1341 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6131 "parser.tab.c"
    break;

  case 462: /* $@316: %empty  */
#line 1343 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6138 "parser.tab.c"
    break;

  case 463: /* $@317: %empty  */
#line 1345 "parser.y"
          { offset2 = atoi(yytext);
	  }
#line 6145 "parser.tab.c"
    break;

  case 464: /* $@318: %empty  */
#line 1347 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Offset_Functor_Label_Offset_Index(MATCH_REG_OFF_STRUCT,
		Bindex(index1),offset1,ConstFunctor(name,arity),
		label,offset2,Bindex(index2));
	  }
#line 6155 "parser.tab.c"
    break;

  case 466: /* $@319: %empty  */
#line 1353 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6162 "parser.tab.c"
    break;

  case 467: /* $@320: %empty  */
#line 1355 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6169 "parser.tab.c"
    break;

  case 468: /* $@321: %empty  */
#line 1357 "parser.y"
          { number = atoi(yytext);
	  }
#line 6176 "parser.tab.c"
    break;

  case 469: /* $@322: %empty  */
#line 1359 "parser.y"
          { offset2 = atoi(yytext);
	  }
#line 6183 "parser.tab.c"
    break;

  case 470: /* $@323: %empty  */
#line 1361 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Offset_Functor_Label_Level_Offset_Index(MATCH_REG_OFF_STRUCT_LEVEL,
		Bindex(index1),offset1,ConstFunctor(name,arity),
		label,number,offset2,Bindex(index2));
	  }
#line 6193 "parser.tab.c"
    break;

  case 472: /* $@324: %empty  */
#line 1367 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6200 "parser.tab.c"
    break;

  case 473: /* $@325: %empty  */
#line 1369 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6207 "parser.tab.c"
    break;

  case 474: /* $@326: %empty  */
#line 1371 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Functor_Label_Offset_Index(MATCH_YVAR_STRUCT,
		Bindex(index1),ConstFunctor(name,arity),
		label,offset1,Bindex(index2));
	  }
#line 6217 "parser.tab.c"
    break;

  case 476: /* $@327: %empty  */
#line 1377 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6224 "parser.tab.c"
    break;

  case 477: /* $@328: %empty  */
#line 1379 "parser.y"
          { number = atoi(yytext);
	  }
#line 6231 "parser.tab.c"
    break;

  case 478: /* $@329: %empty  */
#line 1381 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6238 "parser.tab.c"
    break;

  case 479: /* $@330: %empty  */
#line 1383 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Functor_Label_Level_Offset_Index(MATCH_YVAR_STRUCT_LEVEL,
		Bindex(index1),ConstFunctor(name,arity),label,number,
		offset1,Bindex(index2));
	  }
#line 6248 "parser.tab.c"
    break;

  case 481: /* $@331: %empty  */
#line 1389 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6255 "parser.tab.c"
    break;

  case 482: /* $@332: %empty  */
#line 1391 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Index(MATCH_REG_LIST_CAR,Bindex(index1),label,Bindex(index2));
	  }
#line 6263 "parser.tab.c"
    break;

  case 484: /* $@333: %empty  */
#line 1395 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6270 "parser.tab.c"
    break;

  case 485: /* $@334: %empty  */
#line 1397 "parser.y"
          { number = atoi(yytext);
	  }
#line 6277 "parser.tab.c"
    break;

  case 486: /* $@335: %empty  */
#line 1399 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Level_Index(MATCH_REG_LIST_CAR_LEVEL,
		Bindex(index1),label,number,Bindex(index2));
	  }
#line 6286 "parser.tab.c"
    break;

  case 488: /* $@336: %empty  */
#line 1404 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6293 "parser.tab.c"
    break;

  case 489: /* $@337: %empty  */
#line 1406 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6300 "parser.tab.c"
    break;

  case 490: /* $@338: %empty  */
#line 1408 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Offset_Label_Index(MATCH_REG_OFF_LIST_CAR,
		Bindex(index1),offset1,label,Bindex(index2));
	  }
#line 6309 "parser.tab.c"
    break;

  case 492: /* $@339: %empty  */
#line 1413 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6316 "parser.tab.c"
    break;

  case 493: /* $@340: %empty  */
#line 1415 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6323 "parser.tab.c"
    break;

  case 494: /* $@341: %empty  */
#line 1417 "parser.y"
          { number = atoi(yytext);
	  }
#line 6330 "parser.tab.c"
    break;

  case 495: /* $@342: %empty  */
#line 1419 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Offset_Label_Level_Index(MATCH_REG_OFF_LIST_CAR_LEVEL,
		Bindex(index1),offset1,label,number,Bindex(index2));
	  }
#line 6339 "parser.tab.c"
    break;

  case 497: /* $@343: %empty  */
#line 1424 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6346 "parser.tab.c"
    break;

  case 498: /* $@344: %empty  */
#line 1426 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Index(MATCH_YVAR_LIST_CAR,Bindex(index1),label,Bindex(index2));
	  }
#line 6354 "parser.tab.c"
    break;

  case 500: /* $@345: %empty  */
#line 1430 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6361 "parser.tab.c"
    break;

  case 501: /* $@346: %empty  */
#line 1432 "parser.y"
          { number = atoi(yytext);
	  }
#line 6368 "parser.tab.c"
    break;

  case 502: /* $@347: %empty  */
#line 1434 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Level_Index(MATCH_YVAR_LIST_CAR_LEVEL,
		Bindex(index1),label,number,Bindex(index2));
	  }
#line 6377 "parser.tab.c"
    break;

  case 504: /* $@348: %empty  */
#line 1439 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6384 "parser.tab.c"
    break;

  case 505: /* $@349: %empty  */
#line 1441 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Index(MATCH_REG_LIST_CDR,Bindex(index1),label,Bindex(index2));
	  }
#line 6392 "parser.tab.c"
    break;

  case 507: /* $@350: %empty  */
#line 1445 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6399 "parser.tab.c"
    break;

  case 508: /* $@351: %empty  */
#line 1447 "parser.y"
          { number = atoi(yytext);
	  }
#line 6406 "parser.tab.c"
    break;

  case 509: /* $@352: %empty  */
#line 1449 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Level_Index(MATCH_REG_LIST_CDR_LEVEL,
		Bindex(index1),label,number,Bindex(index2));
	  }
#line 6415 "parser.tab.c"
    break;

  case 511: /* $@353: %empty  */
#line 1454 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6422 "parser.tab.c"
    break;

  case 512: /* $@354: %empty  */
#line 1456 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6429 "parser.tab.c"
    break;

  case 513: /* $@355: %empty  */
#line 1458 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Offset_Label_Index(MATCH_REG_OFF_LIST_CDR,
		Bindex(index1),offset1,label,Bindex(index2));
	  }
#line 6438 "parser.tab.c"
    break;

  case 515: /* $@356: %empty  */
#line 1463 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6445 "parser.tab.c"
    break;

  case 516: /* $@357: %empty  */
#line 1465 "parser.y"
          { offset1 = atoi(yytext);
	  }
#line 6452 "parser.tab.c"
    break;

  case 517: /* $@358: %empty  */
#line 1467 "parser.y"
          { number = atoi(yytext);
	  }
#line 6459 "parser.tab.c"
    break;

  case 518: /* $@359: %empty  */
#line 1469 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Offset_Label_Level_Index(MATCH_REG_OFF_LIST_CDR_LEVEL,
		Bindex(index1),offset1,label,number,Bindex(index2));
	  }
#line 6468 "parser.tab.c"
    break;

  case 520: /* $@360: %empty  */
#line 1474 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6475 "parser.tab.c"
    break;

  case 521: /* $@361: %empty  */
#line 1476 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Index(MATCH_YVAR_LIST_CDR,Bindex(index1),label,Bindex(index2));
	  }
#line 6483 "parser.tab.c"
    break;

  case 523: /* $@362: %empty  */
#line 1480 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6490 "parser.tab.c"
    break;

  case 524: /* $@363: %empty  */
#line 1482 "parser.y"
          { number = atoi(yytext);
	  }
#line 6497 "parser.tab.c"
    break;

  case 525: /* $@364: %empty  */
#line 1484 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Label_Level_Index(MATCH_YVAR_LIST_CDR_LEVEL,
		Bindex(index1),label,number,Bindex(index2));
	  }
#line 6506 "parser.tab.c"
    break;

  case 527: /* instruction: TOKEN_PUSH_LIST_IMM  */
#line 1488 "parser.y"
                              { Instr_None(PUSH_LIST_IMM); }
#line 6512 "parser.tab.c"
    break;

  case 528: /* $@365: %empty  */
#line 1490 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Offset(PUSH_LIST,offset1);
	  }
#line 6520 "parser.tab.c"
    break;

  case 530: /* instruction: TOKEN_PUSH_STRUCTURE_IMM  */
#line 1493 "parser.y"
                                   { Instr_None(PUSH_STRUCTURE_IMM); }
#line 6526 "parser.tab.c"
    break;

  case 531: /* $@366: %empty  */
#line 1495 "parser.y"
          { offset1 = atoi(yytext);
	    Instr_Offset(PUSH_STRUCTURE,offset1);
	  }
#line 6534 "parser.tab.c"
    break;

  case 533: /* instruction: TOKEN_PUSH_NIL  */
#line 1498 "parser.y"
                         { Instr_None(PUSH_NIL); }
#line 6540 "parser.tab.c"
    break;

  case 534: /* $@367: %empty  */
#line 1500 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(PUSH_REG,Bindex(index1));
	  }
#line 6548 "parser.tab.c"
    break;

  case 536: /* $@368: %empty  */
#line 1504 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(PUSH_YVAR,Bindex(index1));
	  }
#line 6556 "parser.tab.c"
    break;

  case 538: /* instruction: TOKEN_PUSH_VOID  */
#line 1507 "parser.y"
                          { Instr_None(PUSH_VOID); }
#line 6562 "parser.tab.c"
    break;

  case 539: /* $@369: %empty  */
#line 1509 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(SET_REG_H_AND_PUSH_VOID,Bindex(index1));
	  }
#line 6570 "parser.tab.c"
    break;

  case 541: /* $@370: %empty  */
#line 1513 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(SET_YVAR_H_AND_PUSH_VOID,Bindex(index1));
	  }
#line 6578 "parser.tab.c"
    break;

  case 543: /* $@371: %empty  */
#line 1517 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6585 "parser.tab.c"
    break;

  case 544: /* $@372: %empty  */
#line 1519 "parser.y"
          { index2 = atoi(yytext);
	    Instr_Index_Index(SET_REG_AND_YVAR_H_AND_PUSH_VOID,Bindex(index1),Bindex(index2));
	  }
#line 6593 "parser.tab.c"
    break;

  case 546: /* instruction: TOKEN_PUSH_ATOM LPAR bam_const RPAR  */
#line 1523 "parser.y"
          { Instr_Term(PUSH_ATOM,const_term);
	  }
#line 6600 "parser.tab.c"
    break;

  case 547: /* instruction: TOKEN_PUSH_INTEGER LPAR bam_const RPAR  */
#line 1526 "parser.y"
          { Instr_Term(PUSH_INTEGER,const_term);
	  }
#line 6607 "parser.tab.c"
    break;

  case 548: /* instruction: TOKEN_PUSH_FUNCTOR LPAR bam_functor RPAR  */
#line 1529 "parser.y"
          { Instr_Functor(PUSH_FUNCTOR,ConstFunctor(name,arity));
	  }
#line 6614 "parser.tab.c"
    break;

  case 549: /* $@373: %empty  */
#line 1532 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(SET_REG_H,Bindex(index1));
	  }
#line 6622 "parser.tab.c"
    break;

  case 551: /* $@374: %empty  */
#line 1536 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(SET_YVAR_H,Bindex(index1));
	  }
#line 6630 "parser.tab.c"
    break;

  case 553: /* $@375: %empty  */
#line 1540 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Label_Index(BTRY,label,Bindex(index1));
	  }
#line 6638 "parser.tab.c"
    break;

  case 555: /* $@376: %empty  */
#line 1544 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Label_Index(BTRUST,label,Bindex(index1));
	  }
#line 6646 "parser.tab.c"
    break;

  case 557: /* $@377: %empty  */
#line 1548 "parser.y"
          { label1 = label;
	  }
#line 6653 "parser.tab.c"
    break;

  case 558: /* instruction: TOKEN_TEST_TRY LPAR label_dest $@377 COMMA label_dest RPAR  */
#line 1550 "parser.y"
          { label2 = label;
	    Instr_Label_Label(TEST_TRY,label1,label2);
	  }
#line 6661 "parser.tab.c"
    break;

  case 559: /* $@378: %empty  */
#line 1554 "parser.y"
          { label1 = label;
	  }
#line 6668 "parser.tab.c"
    break;

  case 560: /* instruction: TOKEN_TEST_TRUST LPAR label_dest $@378 COMMA label_dest RPAR  */
#line 1556 "parser.y"
          { label2 = label;
	    Instr_Label_Label(TEST_TRUST,label1,label2);
	  }
#line 6676 "parser.tab.c"
    break;

  case 561: /* instruction: TOKEN_S_TRY LPAR label_dest RPAR  */
#line 1560 "parser.y"
          { Instr_Label(S_TRY,label);
	  }
#line 6683 "parser.tab.c"
    break;

  case 562: /* instruction: TOKEN_S_TRUST LPAR label_dest RPAR  */
#line 1563 "parser.y"
          { Instr_Label(S_TRUST,label);
	  }
#line 6690 "parser.tab.c"
    break;

  case 563: /* instruction: TOKEN_BACK LPAR label_dest RPAR  */
#line 1566 "parser.y"
          { Instr_Label(BACK,label);
	  }
#line 6697 "parser.tab.c"
    break;

  case 564: /* $@379: %empty  */
#line 1569 "parser.y"
          { bitmax = atoi(yytext);
	    Instr_Label(GEN_TRY,label);
	    bitmask = 0;
	    bitmask_offset = 0;
	  }
#line 6707 "parser.tab.c"
    break;

  case 566: /* instruction: TOKEN_GEN_TRUST  */
#line 1574 "parser.y"
                          { Instr_None(GEN_TRUST); }
#line 6713 "parser.tab.c"
    break;

  case 567: /* instruction: TOKEN_BEXECUTE_SIMPLE LPAR bam_functor RPAR  */
#line 1576 "parser.y"
          { Instr_Pred(BEXECUTE_SIMPLE,GetPred(fname,arity));
	  }
#line 6720 "parser.tab.c"
    break;

  case 568: /* instruction: TOKEN_BEXECUTE LPAR bam_functor RPAR  */
#line 1579 "parser.y"
          { Instr_Pred(BEXECUTE,GetPred(fname,arity));
	  }
#line 6727 "parser.tab.c"
    break;

  case 569: /* $@380: %empty  */
#line 1582 "parser.y"
          { bitmax = atoi(yytext);
	    Instr_Pred(BCALL,GetPred(fname,arity));
	    bitmask = 0;
	    bitmask_offset = 0;
	  }
#line 6737 "parser.tab.c"
    break;

  case 571: /* instruction: TOKEN_JUMP_GC_AND_DUMP_CONT LPAR label_dest RPAR  */
#line 1588 "parser.y"
          { Instr_Label(JUMP_GC_AND_DUMP_CONT,label);
	  }
#line 6744 "parser.tab.c"
    break;

  case 572: /* instruction: TOKEN_JUMP_GC LPAR label_dest RPAR  */
#line 1591 "parser.y"
          { Instr_Label(JUMP_GC,label);
	  }
#line 6751 "parser.tab.c"
    break;

  case 573: /* instruction: TOKEN_JUMP LPAR label_dest RPAR  */
#line 1594 "parser.y"
          { Instr_Label(JUMP,label);
	  }
#line 6758 "parser.tab.c"
    break;

  case 574: /* instruction: TOKEN_BPROCEED_SIMPLE  */
#line 1596 "parser.y"
                                { Instr_None(BPROCEED_SIMPLE); }
#line 6764 "parser.tab.c"
    break;

  case 575: /* instruction: TOKEN_BPROCEED  */
#line 1597 "parser.y"
                         { Instr_None(BPROCEED); }
#line 6770 "parser.tab.c"
    break;

  case 576: /* $@381: %empty  */
#line 1599 "parser.y"
          { size = atoi(yytext);
	    Instr_Size(CREATE_CONT,size);
	  }
#line 6778 "parser.tab.c"
    break;

  case 578: /* $@382: %empty  */
#line 1603 "parser.y"
          { number = atoi(yytext);
	  }
#line 6785 "parser.tab.c"
    break;

  case 579: /* instruction: TOKEN_TEST_LEVEL LPAR NUMBER $@382 COMMA label_dest RPAR  */
#line 1605 "parser.y"
          { Instr_Level_Label(TEST_LEVEL,number,label);
	  }
#line 6792 "parser.tab.c"
    break;

  case 580: /* $@383: %empty  */
#line 1608 "parser.y"
          { bitmax = atoi(yytext);
	    Instr_Label(CONSTRAINT_STACK_TEST,label);
	    bitmask = 0;
	    bitmask_offset = 0;
	  }
#line 6802 "parser.tab.c"
    break;

  case 582: /* instruction: TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK LPAR label_dest RPAR  */
#line 1614 "parser.y"
          { Instr_Label(JUMP_ON_NONEMPTY_CONSTRAINT_STACK,label);
	  }
#line 6809 "parser.tab.c"
    break;

  case 583: /* instruction: TOKEN_ALIAS_CHECK  */
#line 1616 "parser.y"
                            { Instr_None(ALIAS_CHECK); }
#line 6815 "parser.tab.c"
    break;

  case 584: /* $@384: %empty  */
#line 1618 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(ALIAS_CHECK_REG,Bindex(index1));
	  }
#line 6823 "parser.tab.c"
    break;

  case 586: /* $@385: %empty  */
#line 1622 "parser.y"
          { index1 = atoi(yytext);
	    Instr_Index(FAIL_ON_ALIAS,Bindex(index1));
	  }
#line 6831 "parser.tab.c"
    break;

  case 588: /* $@386: %empty  */
#line 1626 "parser.y"
          { size = atoi(yytext);
	    Instr_Size(CHOICE_QUIET_CUT,size);
	  }
#line 6839 "parser.tab.c"
    break;

  case 590: /* $@387: %empty  */
#line 1630 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6846 "parser.tab.c"
    break;

  case 591: /* $@388: %empty  */
#line 1632 "parser.y"
          { arity = atoi(yytext);
	    Instr_Index_Label_Size(SUSPEND_LONE_VAR,Bindex(index1),label,arity);
	  }
#line 6854 "parser.tab.c"
    break;

  case 593: /* $@389: %empty  */
#line 1636 "parser.y"
          { index1 = atoi(yytext);
	  }
#line 6861 "parser.tab.c"
    break;

  case 594: /* $@390: %empty  */
#line 1638 "parser.y"
          { arity = atoi(yytext);
	    Instr_Index_Label_Size(BSUSPEND,Bindex(index1),label,arity);
	  }
#line 6869 "parser.tab.c"
    break;

  case 596: /* $@391: %empty  */
#line 1642 "parser.y"
          { arity = atoi(yytext);
	    Instr_Label_Size(SUSPEND_WITHOUT_REG,label,arity);
	  }
#line 6877 "parser.tab.c"
    break;

  case 598: /* $@392: %empty  */
#line 1646 "parser.y"
          { size = atoi(yytext);
	    Instr_Size(CREATE_GUARD,size);
	  }
#line 6885 "parser.tab.c"
    break;

  case 600: /* $@393: %empty  */
#line 1650 "parser.y"
          { arity = atoi(yytext);
	    Instr_Label_Size(FLAT_QUIET_CUT,label,arity);
	  }
#line 6893 "parser.tab.c"
    break;

  case 602: /* $@394: %empty  */
#line 1654 "parser.y"
          { arity = atoi(yytext);
	  }
#line 6900 "parser.tab.c"
    break;

  case 603: /* $@395: %empty  */
#line 1656 "parser.y"
          { bitmax = atoi(yytext);
	    Instr_Label_Size(FLAT_QUIET_COMMIT,label,arity);
	    bitmask = 0;
	    bitmask_offset = 0;
	  }
#line 6910 "parser.tab.c"
    break;

  case 605: /* $@396: %empty  */
#line 1662 "parser.y"
          { arity = atoi(yytext);
	  }
#line 6917 "parser.tab.c"
    break;

  case 606: /* $@397: %empty  */
#line 1664 "parser.y"
          { bitmax = atoi(yytext);
	    Instr_Label_Size(FLAT_NOISY_WAIT,label,arity);
	    bitmask = 0;
	    bitmask_offset = 0;
	  }
#line 6927 "parser.tab.c"
    break;

  case 608: /* instruction: TOKEN_SHORTCUT_QUIET_CUT LPAR label_dest RPAR  */
#line 1670 "parser.y"
          { Instr_Label(SHORTCUT_QUIET_CUT,label);
	  }
#line 6934 "parser.tab.c"
    break;

  case 609: /* instruction: TOKEN_SHORTCUT_QUIET_COMMIT LPAR label_dest RPAR  */
#line 1673 "parser.y"
          { Instr_Label(SHORTCUT_QUIET_COMMIT,label);
	  }
#line 6941 "parser.tab.c"
    break;

  case 610: /* instruction: TOKEN_SHORTCUT_NOISY_WAIT LPAR label_dest RPAR  */
#line 1676 "parser.y"
          { Instr_Label(SHORTCUT_NOISY_WAIT,label);
	  }
#line 6948 "parser.tab.c"
    break;

  case 611: /* instruction: TOKEN_DEEP_QUIET_CUT  */
#line 1678 "parser.y"
                               { Instr_None(DEEP_QUIET_CUT); }
#line 6954 "parser.tab.c"
    break;

  case 612: /* instruction: TOKEN_DEEP_QUIET_COMMIT  */
#line 1679 "parser.y"
                                  { Instr_None(DEEP_QUIET_COMMIT); }
#line 6960 "parser.tab.c"
    break;

  case 613: /* instruction: TOKEN_DEEP_NOISY_WAIT  */
#line 1680 "parser.y"
                                { Instr_None(DEEP_NOISY_WAIT); }
#line 6966 "parser.tab.c"
    break;

  case 614: /* $@398: %empty  */
#line 1682 "parser.y"
          { label = atoi(yytext);
	  }
#line 6973 "parser.tab.c"
    break;

  case 615: /* $@399: %empty  */
#line 1684 "parser.y"
          { number = atoi(yytext);	/* What to do with this? */
	    pc = (code *) WordAlign((uword)pc);
	    Label(label);
	  }
#line 6982 "parser.tab.c"
    break;

  case 617: /* instruction: pseudo_term  */
#line 1689 "parser.y"
          { /* Maybe we should inline the pseudo_terms here? */
	  }
#line 6989 "parser.tab.c"
    break;

  case 618: /* $@400: %empty  */
#line 1695 "parser.y"
          { label = atoi(yytext);
	  }
#line 6996 "parser.tab.c"
    break;

  case 620: /* label_dest: TOKEN_FAIL_DEC  */
#line 1698 "parser.y"
          { label = 0;
	  }
#line 7003 "parser.tab.c"
    break;

  case 621: /* bam_const: NUMBER  */
#line 1704 "parser.y"
          { const_term = ConstInt(yytext);
	  }
#line 7010 "parser.tab.c"
    break;

  case 622: /* bam_const: ATOMNAME  */
#line 1707 "parser.y"
          { const_term = ConstAtom(yytext);
	  }
#line 7017 "parser.tab.c"
    break;

  case 623: /* bam_const_table_entry: bam_const COMMA label_dest  */
#line 1713 "parser.y"
          { BamConstTableEntry(const_term,(code)label);
	  }
#line 7024 "parser.tab.c"
    break;

  case 626: /* $@401: %empty  */
#line 1725 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 7031 "parser.tab.c"
    break;

  case 627: /* bam_functor: ATOMNAME $@401 COMMA NUMBER  */
#line 1727 "parser.y"
          { arity = atoi(yytext);
	  }
#line 7038 "parser.tab.c"
    break;

  case 628: /* bam_functor_table_entry: bam_functor COMMA label_dest  */
#line 1733 "parser.y"
          { BamFunctorTableEntry(ConstFunctor(fname,arity),(code)label);
	  }
#line 7045 "parser.tab.c"
    break;

  case 631: /* bam_binding: CONSTANT LPAR bam_const RPAR  */
#line 1744 "parser.y"
          { BamBindingConst(const_term);
	  }
#line 7052 "parser.tab.c"
    break;

  case 632: /* bam_binding: LIST LPAR label_dest RPAR  */
#line 1747 "parser.y"
          { BamBindingLabel(label, LABELREF_TAG_LST);
	  }
#line 7059 "parser.tab.c"
    break;

  case 633: /* bam_binding: STRUCT LPAR label_dest RPAR  */
#line 1750 "parser.y"
          { BamBindingLabel(label, LABELREF_TAG_STR);
	  }
#line 7066 "parser.tab.c"
    break;

  case 634: /* bam_binding: TYPED LPAR type_id RPAR  */
#line 1753 "parser.y"
          { BamBindingConst(TagPseudoTYPE(type_code));
	  }
#line 7073 "parser.tab.c"
    break;

  case 635: /* type_id: INTEGER  */
#line 1758 "parser.y"
                 { type_code = TYPED_INTEGER; }
#line 7079 "parser.tab.c"
    break;

  case 636: /* type_id: ATOM  */
#line 1759 "parser.y"
                 { type_code = TYPED_ATOM; }
#line 7085 "parser.tab.c"
    break;

  case 637: /* type_id: STRUCT  */
#line 1760 "parser.y"
                 { type_code = TYPED_STRUCT; }
#line 7091 "parser.tab.c"
    break;

  case 638: /* pseudo_term: CONSTANT LPAR bam_const RPAR  */
#line 1765 "parser.y"
          { BamBindingConst(const_term);
	  }
#line 7098 "parser.tab.c"
    break;

  case 639: /* pseudo_term: FUNCTOR LPAR bam_functor RPAR  */
#line 1768 "parser.y"
          { BamBindingFunctor(ConstFunctor(fname,arity));
	  }
#line 7105 "parser.tab.c"
    break;

  case 640: /* pseudo_term: LIST LPAR label_dest RPAR  */
#line 1771 "parser.y"
          { BamBindingLabel(label, LABELREF_TAG_LST);
	  }
#line 7112 "parser.tab.c"
    break;

  case 641: /* pseudo_term: STRUCT LPAR label_dest RPAR  */
#line 1774 "parser.y"
          { BamBindingLabel(label, LABELREF_TAG_STR);
	  }
#line 7119 "parser.tab.c"
    break;

  case 642: /* $@402: %empty  */
#line 1777 "parser.y"
          { index1 = atoi(yytext);
	    BamBindingConst(TagPseudoREG(Bindex(index1)));
	  }
#line 7127 "parser.tab.c"
    break;

  case 644: /* pseudo_term: VOID  */
#line 1781 "parser.y"
          { BamBindingConst(TagPseudoREG(PSEUDO_VOID));
	  }
#line 7134 "parser.tab.c"
    break;

  case 645: /* pseudo_term: LABELED LPAR label_dest RPAR  */
#line 1784 "parser.y"
          { BamBindingLabel(label, LABELREF_TAG_LBL);
#endif
	  }
#line 7142 "parser.tab.c"
    break;

  case 646: /* bam_integer_set: integer_set  */
#line 1791 "parser.y"
        { CodeWord(0,bitmask);
	  EndInstruction(1);
	}
#line 7150 "parser.tab.c"
    break;

  case 647: /* integer_set_item: NUMBER  */
#line 1798 "parser.y"
        { index1 = atoi(yytext);
	  index1 = Bindex(index1) - bitmask_offset;
	  if  (index1 >= BITMASKBITS) {
	    FatalError("No long bitmasks yet!");
	  }
	  while (index1 >= BITMASKBITS) {
	    CodeWord(0,bitmask);
	    EndInstruction(1);
	    bitmask = 0;
	    bitmask_offset += BITMASKBITS;
	    index1 -= BITMASKBITS;
	  }
	  bitmask = (bitmask | (0x1 << index1));
	}
#line 7169 "parser.tab.c"
    break;

  case 650: /* $@403: %empty  */
#line 1822 "parser.y"
          { const_term = ConstAtom(yytext);
	  }
#line 7176 "parser.tab.c"
    break;

  case 651: /* $@404: %empty  */
#line 1824 "parser.y"
          { label = atoi(yytext);
	    ConstantTableEntry(const_term,(code)label);
	  }
#line 7184 "parser.tab.c"
    break;

  case 653: /* $@405: %empty  */
#line 1828 "parser.y"
          { const_term = ConstInt(yytext);
	  }
#line 7191 "parser.tab.c"
    break;

  case 654: /* $@406: %empty  */
#line 1830 "parser.y"
          { label = atoi(yytext);
	    ConstantTableEntry(const_term,(code)label);
	  }
#line 7199 "parser.tab.c"
    break;

  case 658: /* $@407: %empty  */
#line 1842 "parser.y"
          { arity = atoi(yytext);
	  }
#line 7206 "parser.tab.c"
    break;

  case 659: /* $@408: %empty  */
#line 1844 "parser.y"
          { label = atoi(yytext);
	    FunctorTableEntry(ConstFunctor(fname,arity),(code)label);
	  }
#line 7214 "parser.tab.c"
    break;

  case 661: /* $@409: %empty  */
#line 1848 "parser.y"
          { arity = atoi(yytext);	
	    features = 0;
	    hv = 0;
	    tbl = (Term*)(malloc(arity * sizeof(Term)));
	  }
#line 7224 "parser.tab.c"
    break;

  case 662: /* $@410: %empty  */
#line 1853 "parser.y"
          { if (features == 0)
	      hv = HashNameArity(Atm(fname),arity);
	    else 
	      hv = hv + (arity -features);
	  }
#line 7234 "parser.tab.c"
    break;

  case 663: /* $@411: %empty  */
#line 1858 "parser.y"
          { label = atoi(yytext);
	    FunctorTableEntry(
	       create_functor(hv,fname,features,arity,tbl),
		(code)label);		
	    free(tbl);
	  }
#line 7245 "parser.tab.c"
    break;

  case 665: /* $@412: %empty  */
#line 1868 "parser.y"
          { fname = ConstAtom(yytext);
	  }
#line 7252 "parser.tab.c"
    break;

  case 669: /* feat_const: NUMBER  */
#line 1879 "parser.y"
          { const_term = ConstInt(yytext);
	    tbl[features++] = const_term;
	  }
#line 7260 "parser.tab.c"
    break;

  case 670: /* feat_const: ATOMNAME  */
#line 1883 "parser.y"
          { const_term = ConstAtom(yytext);
	    tbl[features++] = const_term;
	  }
#line 7268 "parser.tab.c"
    break;

  case 671: /* feat_const_list_entry: feat_const  */
#line 1891 "parser.y"
          { build_hashvalue(const_term,0,&hv);
	  }
#line 7275 "parser.tab.c"
    break;

  case 674: /* fd_entry: RPAR  */
#line 1903 "parser.y"
          { fd_add_instr(FD_HALT); }
#line 7281 "parser.tab.c"
    break;

  case 676: /* fd_instr: NUMBER  */
#line 1909 "parser.y"
          { fd_add_const(atoi(yytext)); }
#line 7287 "parser.tab.c"
    break;

  case 677: /* fd_instr: FD_const_T  */
#line 1911 "parser.y"
          { fd_add_instr(FD_CONST); }
#line 7293 "parser.tab.c"
    break;

  case 678: /* fd_instr: FD_element_x_T  */
#line 1913 "parser.y"
          { fd_add_instr(FD_ELEMENT_X); }
#line 7299 "parser.tab.c"
    break;

  case 679: /* fd_instr: FD_element_i_T  */
#line 1915 "parser.y"
          { fd_add_instr(FD_ELEMENT_I); }
#line 7305 "parser.tab.c"
    break;

  case 680: /* fd_instr: FD_val_T  */
#line 1917 "parser.y"
          { fd_add_instr(FD_VAL); }
#line 7311 "parser.tab.c"
    break;

  case 681: /* fd_instr: FD_val_0_T  */
#line 1919 "parser.y"
          { fd_add_instr(FD_VAL_0); }
#line 7317 "parser.tab.c"
    break;

  case 682: /* fd_instr: FD_val_1_T  */
#line 1921 "parser.y"
          { fd_add_instr(FD_VAL_1); }
#line 7323 "parser.tab.c"
    break;

  case 683: /* fd_instr: FD_val_2_T  */
#line 1923 "parser.y"
          { fd_add_instr(FD_VAL_2); }
#line 7329 "parser.tab.c"
    break;

  case 684: /* fd_instr: FD_max_elem_T  */
#line 1925 "parser.y"
          { fd_add_instr(FD_MAXELEM); }
#line 7335 "parser.tab.c"
    break;

  case 685: /* fd_instr: FD_add_T  */
#line 1927 "parser.y"
          { fd_add_instr(FD_ADD); }
#line 7341 "parser.tab.c"
    break;

  case 686: /* fd_instr: FD_sub_T  */
#line 1929 "parser.y"
          { fd_add_instr(FD_SUB); }
#line 7347 "parser.tab.c"
    break;

  case 687: /* fd_instr: FD_mult_T  */
#line 1931 "parser.y"
          { fd_add_instr(FD_MULT); }
#line 7353 "parser.tab.c"
    break;

  case 688: /* fd_instr: FD_divd_T  */
#line 1933 "parser.y"
          { fd_add_instr(FD_DIVD); }
#line 7359 "parser.tab.c"
    break;

  case 689: /* fd_instr: FD_divu_T  */
#line 1935 "parser.y"
          { fd_add_instr(FD_DIVU); }
#line 7365 "parser.tab.c"
    break;

  case 690: /* fd_instr: FD_mod_T  */
#line 1937 "parser.y"
          { fd_add_instr(FD_MOD); }
#line 7371 "parser.tab.c"
    break;

  case 691: /* fd_instr: FD_min_T  */
#line 1939 "parser.y"
          { fd_add_instr(FD_MIN); }
#line 7377 "parser.tab.c"
    break;

  case 692: /* fd_instr: FD_max_T  */
#line 1941 "parser.y"
          { fd_add_instr(FD_MAX); }
#line 7383 "parser.tab.c"
    break;

  case 693: /* fd_instr: FD_dom_min_T  */
#line 1943 "parser.y"
          { fd_add_instr(FD_DOMMIN); }
#line 7389 "parser.tab.c"
    break;

  case 694: /* fd_instr: FD_dom_min_0_T  */
#line 1945 "parser.y"
          { fd_add_instr(FD_DOMMIN_0); }
#line 7395 "parser.tab.c"
    break;

  case 695: /* fd_instr: FD_dom_min_1_T  */
#line 1947 "parser.y"
          { fd_add_instr(FD_DOMMIN_1); }
#line 7401 "parser.tab.c"
    break;

  case 696: /* fd_instr: FD_dom_min_2_T  */
#line 1949 "parser.y"
          { fd_add_instr(FD_DOMMIN_2); }
#line 7407 "parser.tab.c"
    break;

  case 697: /* fd_instr: FD_dom_max_T  */
#line 1951 "parser.y"
          { fd_add_instr(FD_DOMMAX); }
#line 7413 "parser.tab.c"
    break;

  case 698: /* fd_instr: FD_dom_max_0_T  */
#line 1953 "parser.y"
          { fd_add_instr(FD_DOMMAX_0); }
#line 7419 "parser.tab.c"
    break;

  case 699: /* fd_instr: FD_dom_max_1_T  */
#line 1955 "parser.y"
          { fd_add_instr(FD_DOMMAX_1); }
#line 7425 "parser.tab.c"
    break;

  case 700: /* fd_instr: FD_dom_max_2_T  */
#line 1957 "parser.y"
          { fd_add_instr(FD_DOMMAX_2); }
#line 7431 "parser.tab.c"
    break;

  case 701: /* fd_instr: FD_dom_T  */
#line 1959 "parser.y"
          { fd_add_instr(FD_DOM); }
#line 7437 "parser.tab.c"
    break;

  case 702: /* fd_instr: FD_dom_0_T  */
#line 1961 "parser.y"
          { fd_add_instr(FD_DOM_0); }
#line 7443 "parser.tab.c"
    break;

  case 703: /* fd_instr: FD_dom_1_T  */
#line 1963 "parser.y"
          { fd_add_instr(FD_DOM_1); }
#line 7449 "parser.tab.c"
    break;

  case 704: /* fd_instr: FD_dom_2_T  */
#line 1965 "parser.y"
          { fd_add_instr(FD_DOM_2); }
#line 7455 "parser.tab.c"
    break;

  case 705: /* fd_instr: FD_range_T  */
#line 1967 "parser.y"
          { fd_add_instr(FD_RANGE); }
#line 7461 "parser.tab.c"
    break;

  case 706: /* fd_instr: FD_outer_range_T  */
#line 1969 "parser.y"
          { fd_add_instr(FD_OUTER_RANGE); }
#line 7467 "parser.tab.c"
    break;

  case 707: /* fd_instr: FD_setadd_T  */
#line 1971 "parser.y"
          { fd_add_instr(FD_SETADD); }
#line 7473 "parser.tab.c"
    break;

  case 708: /* fd_instr: FD_setsub_T  */
#line 1973 "parser.y"
          { fd_add_instr(FD_SETSUB); }
#line 7479 "parser.tab.c"
    break;

  case 709: /* fd_instr: FD_setmod_T  */
#line 1975 "parser.y"
          { fd_add_instr(FD_SETMOD); }
#line 7485 "parser.tab.c"
    break;

  case 710: /* fd_instr: FD_compl_T  */
#line 1977 "parser.y"
          { fd_add_instr(FD_COMPL); }
#line 7491 "parser.tab.c"
    break;

  case 711: /* fd_instr: FD_union_T  */
#line 1979 "parser.y"
          { fd_add_instr(FD_UNION); }
#line 7497 "parser.tab.c"
    break;

  case 712: /* fd_instr: FD_inter_T  */
#line 1981 "parser.y"
          { fd_add_instr(FD_INTER); }
#line 7503 "parser.tab.c"
    break;

  case 713: /* fd_instr: FD_check_T  */
#line 1983 "parser.y"
          { 
            fd_add_instr(FD_CHECK);
	    fd_push_check_label();
	  }
#line 7512 "parser.tab.c"
    break;

  case 714: /* fd_instr: FD_label_T  */
#line 1988 "parser.y"
          {
	    fd_pop_check_label();
	  }
#line 7520 "parser.tab.c"
    break;


#line 7524 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1993 "parser.y"

static int yyerror(s)
	char *s	;
{
    fprintf(stderr, "%s\n", s);
    return 1;
}
