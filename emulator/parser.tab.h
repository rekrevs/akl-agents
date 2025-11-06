/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_SWITCH_ON_TERM = 258,    /* TOKEN_SWITCH_ON_TERM  */
    TOKEN_TRY_SINGLE = 259,        /* TOKEN_TRY_SINGLE  */
    TOKEN_TRY = 260,               /* TOKEN_TRY  */
    TOKEN_RETRY = 261,             /* TOKEN_RETRY  */
    TOKEN_TRUST = 262,             /* TOKEN_TRUST  */
    TOKEN_GUARD_COND = 263,        /* TOKEN_GUARD_COND  */
    TOKEN_GUARD_COMMIT = 264,      /* TOKEN_GUARD_COMMIT  */
    TOKEN_GUARD_WAIT = 265,        /* TOKEN_GUARD_WAIT  */
    TOKEN_GUARD_UNORDER = 266,     /* TOKEN_GUARD_UNORDER  */
    TOKEN_GUARD_ORDER = 267,       /* TOKEN_GUARD_ORDER  */
    TOKEN_GUARD_UNIT = 268,        /* TOKEN_GUARD_UNIT  */
    TOKEN_GUARD_CUT = 269,         /* TOKEN_GUARD_CUT  */
    TOKEN_CALL = 270,              /* TOKEN_CALL  */
    TOKEN_ALLOCATE = 271,          /* TOKEN_ALLOCATE  */
    TOKEN_DEALLOCATE = 272,        /* TOKEN_DEALLOCATE  */
    TOKEN_META_CALL = 273,         /* TOKEN_META_CALL  */
    TOKEN_EXTERN_EXECUTE = 274,    /* TOKEN_EXTERN_EXECUTE  */
    TOKEN_EXECUTE = 275,           /* TOKEN_EXECUTE  */
    TOKEN_DEALLOC_EXECUTE = 276,   /* TOKEN_DEALLOC_EXECUTE  */
    TOKEN_EXTERN_CALL = 277,       /* TOKEN_EXTERN_CALL  */
    TOKEN_CALL_APPLY = 278,        /* TOKEN_CALL_APPLY  */
    TOKEN_EXECUTE_APPLY = 279,     /* TOKEN_EXECUTE_APPLY  */
    TOKEN_DEALLOC_EXECUTE_APPLY = 280, /* TOKEN_DEALLOC_EXECUTE_APPLY  */
    TOKEN_PROCEED = 281,           /* TOKEN_PROCEED  */
    TOKEN_DEALLOC_PROCEED = 282,   /* TOKEN_DEALLOC_PROCEED  */
    TOKEN_FAIL = 283,              /* TOKEN_FAIL  */
    TOKEN_GET_X_VARIABLE = 284,    /* TOKEN_GET_X_VARIABLE  */
    TOKEN_GET_Y_VARIABLE = 285,    /* TOKEN_GET_Y_VARIABLE  */
    TOKEN_GET2_Y_VARIABLE = 286,   /* TOKEN_GET2_Y_VARIABLE  */
    TOKEN_GET3_Y_VARIABLE = 287,   /* TOKEN_GET3_Y_VARIABLE  */
    TOKEN_GET4_Y_VARIABLE = 288,   /* TOKEN_GET4_Y_VARIABLE  */
    TOKEN_GET5_Y_VARIABLE = 289,   /* TOKEN_GET5_Y_VARIABLE  */
    TOKEN_GET_X_VALUE = 290,       /* TOKEN_GET_X_VALUE  */
    TOKEN_GET_Y_VALUE = 291,       /* TOKEN_GET_Y_VALUE  */
    TOKEN_GET_CONSTANT = 292,      /* TOKEN_GET_CONSTANT  */
    TOKEN_GET_NIL = 293,           /* TOKEN_GET_NIL  */
    TOKEN_GET_STRUCTURE = 294,     /* TOKEN_GET_STRUCTURE  */
    TOKEN_GET_LIST = 295,          /* TOKEN_GET_LIST  */
    TOKEN_GET_LIST_X0 = 296,       /* TOKEN_GET_LIST_X0  */
    TOKEN_GET_NIL_X0 = 297,        /* TOKEN_GET_NIL_X0  */
    TOKEN_GET_STRUCTURE_X0 = 298,  /* TOKEN_GET_STRUCTURE_X0  */
    TOKEN_GET_CONSTANT_X0 = 299,   /* TOKEN_GET_CONSTANT_X0  */
    TOKEN_GET_ABSTRACTION = 300,   /* TOKEN_GET_ABSTRACTION  */
    TOKEN_PUT_X_VOID = 301,        /* TOKEN_PUT_X_VOID  */
    TOKEN_PUT_Y_VOID = 302,        /* TOKEN_PUT_Y_VOID  */
    TOKEN_PUT_X_VARIABLE = 303,    /* TOKEN_PUT_X_VARIABLE  */
    TOKEN_PUT_Y_VARIABLE = 304,    /* TOKEN_PUT_Y_VARIABLE  */
    TOKEN_PUT_X_VALUE = 305,       /* TOKEN_PUT_X_VALUE  */
    TOKEN_PUT_Y_VALUE = 306,       /* TOKEN_PUT_Y_VALUE  */
    TOKEN_PUT2_Y_VALUE = 307,      /* TOKEN_PUT2_Y_VALUE  */
    TOKEN_PUT3_Y_VALUE = 308,      /* TOKEN_PUT3_Y_VALUE  */
    TOKEN_PUT4_Y_VALUE = 309,      /* TOKEN_PUT4_Y_VALUE  */
    TOKEN_PUT5_Y_VALUE = 310,      /* TOKEN_PUT5_Y_VALUE  */
    TOKEN_PUT_CONSTANT = 311,      /* TOKEN_PUT_CONSTANT  */
    TOKEN_PUT_NIL = 312,           /* TOKEN_PUT_NIL  */
    TOKEN_PUT_STRUCTURE = 313,     /* TOKEN_PUT_STRUCTURE  */
    TOKEN_PUT_LIST = 314,          /* TOKEN_PUT_LIST  */
    TOKEN_UNIFY_VOID = 315,        /* TOKEN_UNIFY_VOID  */
    TOKEN_PUT_ABSTRACTION = 316,   /* TOKEN_PUT_ABSTRACTION  */
    TOKEN_UNIFY_Y_VARIABLE = 317,  /* TOKEN_UNIFY_Y_VARIABLE  */
    TOKEN_UNIFY_X_VARIABLE = 318,  /* TOKEN_UNIFY_X_VARIABLE  */
    TOKEN_UNIFY_X_VALUE = 319,     /* TOKEN_UNIFY_X_VALUE  */
    TOKEN_UNIFY_Y_VALUE = 320,     /* TOKEN_UNIFY_Y_VALUE  */
    TOKEN_UNIFY_CONSTANT = 321,    /* TOKEN_UNIFY_CONSTANT  */
    TOKEN_UNIFY_NIL = 322,         /* TOKEN_UNIFY_NIL  */
    TOKEN_UNIFY_LIST = 323,        /* TOKEN_UNIFY_LIST  */
    TOKEN_UNIFY_STRUCTURE = 324,   /* TOKEN_UNIFY_STRUCTURE  */
    TOKEN_SWITCH_ON_CONSTANT = 325, /* TOKEN_SWITCH_ON_CONSTANT  */
    TOKEN_SWITCH_ON_STRUCTURE = 326, /* TOKEN_SWITCH_ON_STRUCTURE  */
    TOKEN_SEND3 = 327,             /* TOKEN_SEND3  */
    TOKEN_SUSPEND_FLAT = 328,      /* TOKEN_SUSPEND_FLAT  */
    TOKEN_ATOM_SWITCH = 329,       /* TOKEN_ATOM_SWITCH  */
    TOKEN_FAIL_DEC = 330,          /* TOKEN_FAIL_DEC  */
    TOKEN_LOAD_NIL = 331,          /* TOKEN_LOAD_NIL  */
    TOKEN_LOAD_ATOM = 332,         /* TOKEN_LOAD_ATOM  */
    TOKEN_LOAD_INTEGER = 333,      /* TOKEN_LOAD_INTEGER  */
    TOKEN_READ_CAR = 334,          /* TOKEN_READ_CAR  */
    TOKEN_READ_CDR = 335,          /* TOKEN_READ_CDR  */
    TOKEN_READ_ARG = 336,          /* TOKEN_READ_ARG  */
    TOKEN_SET_REG_REG = 337,       /* TOKEN_SET_REG_REG  */
    TOKEN_SET_REG_YVAR = 338,      /* TOKEN_SET_REG_YVAR  */
    TOKEN_SET_YVAR_REG = 339,      /* TOKEN_SET_YVAR_REG  */
    TOKEN_TERM_SWITCH = 340,       /* TOKEN_TERM_SWITCH  */
    TOKEN_ATOM_WITH_ELSE_SWITCH = 341, /* TOKEN_ATOM_WITH_ELSE_SWITCH  */
    TOKEN_ATOM_WITH_ATOM_ELSE_SWITCH = 342, /* TOKEN_ATOM_WITH_ATOM_ELSE_SWITCH  */
    TOKEN_ATOM_TYPE = 343,         /* TOKEN_ATOM_TYPE  */
    TOKEN_ATOM_TYPE_WITH_ELSE = 344, /* TOKEN_ATOM_TYPE_WITH_ELSE  */
    TOKEN_KNOWN_ATOM_SWITCH = 345, /* TOKEN_KNOWN_ATOM_SWITCH  */
    TOKEN_KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH = 346, /* TOKEN_KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH  */
    TOKEN_INTEGER_SWITCH = 347,    /* TOKEN_INTEGER_SWITCH  */
    TOKEN_INTEGER_WITH_ELSE_SWITCH = 348, /* TOKEN_INTEGER_WITH_ELSE_SWITCH  */
    TOKEN_INTEGER_WITH_INTEGER_ELSE_SWITCH = 349, /* TOKEN_INTEGER_WITH_INTEGER_ELSE_SWITCH  */
    TOKEN_INTEGER_TYPE = 350,      /* TOKEN_INTEGER_TYPE  */
    TOKEN_INTEGER_TYPE_WITH_ELSE = 351, /* TOKEN_INTEGER_TYPE_WITH_ELSE  */
    TOKEN_KNOWN_INTEGER_SWITCH = 352, /* TOKEN_KNOWN_INTEGER_SWITCH  */
    TOKEN_KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH = 353, /* TOKEN_KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH  */
    TOKEN_LIST_SWITCH = 354,       /* TOKEN_LIST_SWITCH  */
    TOKEN_LIST_TYPE_WITH_ELSE = 355, /* TOKEN_LIST_TYPE_WITH_ELSE  */
    TOKEN_STRUCT_TYPE = 356,       /* TOKEN_STRUCT_TYPE  */
    TOKEN_STRUCT_TYPE_WITH_ELSE = 357, /* TOKEN_STRUCT_TYPE_WITH_ELSE  */
    TOKEN_KNOWN_FUNCTOR_SWITCH = 358, /* TOKEN_KNOWN_FUNCTOR_SWITCH  */
    TOKEN_KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH = 359, /* TOKEN_KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH  */
    TOKEN_INT_COMPARE_REG_REG = 360, /* TOKEN_INT_COMPARE_REG_REG  */
    TOKEN_INT_COMPARE_REG_INT = 361, /* TOKEN_INT_COMPARE_REG_INT  */
    TOKEN_EQ_REG_REG = 362,        /* TOKEN_EQ_REG_REG  */
    TOKEN_MATCH_REG_REG = 363,     /* TOKEN_MATCH_REG_REG  */
    TOKEN_MATCH_REG_REG_OFF = 364, /* TOKEN_MATCH_REG_REG_OFF  */
    TOKEN_MATCH_REG_ATOM = 365,    /* TOKEN_MATCH_REG_ATOM  */
    TOKEN_MATCH_REG_OFF_ATOM = 366, /* TOKEN_MATCH_REG_OFF_ATOM  */
    TOKEN_MATCH_REG_INTEGER = 367, /* TOKEN_MATCH_REG_INTEGER  */
    TOKEN_MATCH_REG_OFF_INTEGER = 368, /* TOKEN_MATCH_REG_OFF_INTEGER  */
    TOKEN_MATCH_REG_NIL = 369,     /* TOKEN_MATCH_REG_NIL  */
    TOKEN_MATCH_REG_OFF_NIL = 370, /* TOKEN_MATCH_REG_OFF_NIL  */
    TOKEN_MATCH_REG_YVAR = 371,    /* TOKEN_MATCH_REG_YVAR  */
    TOKEN_MATCH_YVAR_YVAR = 372,   /* TOKEN_MATCH_YVAR_YVAR  */
    TOKEN_MATCH_YVAR_REG_OFF = 373, /* TOKEN_MATCH_YVAR_REG_OFF  */
    TOKEN_MATCH_REG_H_LIST = 374,  /* TOKEN_MATCH_REG_H_LIST  */
    TOKEN_MATCH_YVAR_H_LIST = 375, /* TOKEN_MATCH_YVAR_H_LIST  */
    TOKEN_MATCH_REG_H_STRUCT = 376, /* TOKEN_MATCH_REG_H_STRUCT  */
    TOKEN_MATCH_YVAR_H_STRUCT = 377, /* TOKEN_MATCH_YVAR_H_STRUCT  */
    TOKEN_MATCH_REG_STRUCT = 378,  /* TOKEN_MATCH_REG_STRUCT  */
    TOKEN_MATCH_REG_STRUCT_LEVEL = 379, /* TOKEN_MATCH_REG_STRUCT_LEVEL  */
    TOKEN_MATCH_REG_OFF_STRUCT = 380, /* TOKEN_MATCH_REG_OFF_STRUCT  */
    TOKEN_MATCH_REG_OFF_STRUCT_LEVEL = 381, /* TOKEN_MATCH_REG_OFF_STRUCT_LEVEL  */
    TOKEN_MATCH_YVAR_STRUCT = 382, /* TOKEN_MATCH_YVAR_STRUCT  */
    TOKEN_MATCH_YVAR_STRUCT_LEVEL = 383, /* TOKEN_MATCH_YVAR_STRUCT_LEVEL  */
    TOKEN_MATCH_REG_LIST_CAR = 384, /* TOKEN_MATCH_REG_LIST_CAR  */
    TOKEN_MATCH_REG_LIST_CAR_LEVEL = 385, /* TOKEN_MATCH_REG_LIST_CAR_LEVEL  */
    TOKEN_MATCH_REG_OFF_LIST_CAR = 386, /* TOKEN_MATCH_REG_OFF_LIST_CAR  */
    TOKEN_MATCH_REG_OFF_LIST_CAR_LEVEL = 387, /* TOKEN_MATCH_REG_OFF_LIST_CAR_LEVEL  */
    TOKEN_MATCH_YVAR_LIST_CAR = 388, /* TOKEN_MATCH_YVAR_LIST_CAR  */
    TOKEN_MATCH_YVAR_LIST_CAR_LEVEL = 389, /* TOKEN_MATCH_YVAR_LIST_CAR_LEVEL  */
    TOKEN_MATCH_REG_LIST_CDR = 390, /* TOKEN_MATCH_REG_LIST_CDR  */
    TOKEN_MATCH_REG_LIST_CDR_LEVEL = 391, /* TOKEN_MATCH_REG_LIST_CDR_LEVEL  */
    TOKEN_MATCH_REG_OFF_LIST_CDR = 392, /* TOKEN_MATCH_REG_OFF_LIST_CDR  */
    TOKEN_MATCH_REG_OFF_LIST_CDR_LEVEL = 393, /* TOKEN_MATCH_REG_OFF_LIST_CDR_LEVEL  */
    TOKEN_MATCH_YVAR_LIST_CDR = 394, /* TOKEN_MATCH_YVAR_LIST_CDR  */
    TOKEN_MATCH_YVAR_LIST_CDR_LEVEL = 395, /* TOKEN_MATCH_YVAR_LIST_CDR_LEVEL  */
    TOKEN_PUSH_LIST_IMM = 396,     /* TOKEN_PUSH_LIST_IMM  */
    TOKEN_PUSH_LIST = 397,         /* TOKEN_PUSH_LIST  */
    TOKEN_PUSH_STRUCTURE_IMM = 398, /* TOKEN_PUSH_STRUCTURE_IMM  */
    TOKEN_PUSH_STRUCTURE = 399,    /* TOKEN_PUSH_STRUCTURE  */
    TOKEN_PUSH_NIL = 400,          /* TOKEN_PUSH_NIL  */
    TOKEN_PUSH_REG = 401,          /* TOKEN_PUSH_REG  */
    TOKEN_PUSH_YVAR = 402,         /* TOKEN_PUSH_YVAR  */
    TOKEN_PUSH_VOID = 403,         /* TOKEN_PUSH_VOID  */
    TOKEN_SET_REG_H_AND_PUSH_VOID = 404, /* TOKEN_SET_REG_H_AND_PUSH_VOID  */
    TOKEN_SET_YVAR_H_AND_PUSH_VOID = 405, /* TOKEN_SET_YVAR_H_AND_PUSH_VOID  */
    TOKEN_PUSH_INTEGER = 406,      /* TOKEN_PUSH_INTEGER  */
    TOKEN_PUSH_FUNCTOR = 407,      /* TOKEN_PUSH_FUNCTOR  */
    TOKEN_SET_REG_AND_YVAR_H_AND_PUSH_VOID = 408, /* TOKEN_SET_REG_AND_YVAR_H_AND_PUSH_VOID  */
    TOKEN_PUSH_ATOM = 409,         /* TOKEN_PUSH_ATOM  */
    TOKEN_SET_REG_H = 410,         /* TOKEN_SET_REG_H  */
    TOKEN_SET_YVAR_H = 411,        /* TOKEN_SET_YVAR_H  */
    TOKEN_BTRY = 412,              /* TOKEN_BTRY  */
    TOKEN_BTRUST = 413,            /* TOKEN_BTRUST  */
    TOKEN_TEST_TRY = 414,          /* TOKEN_TEST_TRY  */
    TOKEN_TEST_TRUST = 415,        /* TOKEN_TEST_TRUST  */
    TOKEN_S_TRY = 416,             /* TOKEN_S_TRY  */
    TOKEN_S_TRUST = 417,           /* TOKEN_S_TRUST  */
    TOKEN_BACK = 418,              /* TOKEN_BACK  */
    TOKEN_GEN_TRY = 419,           /* TOKEN_GEN_TRY  */
    TOKEN_GEN_TRUST = 420,         /* TOKEN_GEN_TRUST  */
    TOKEN_BEXECUTE_SIMPLE = 421,   /* TOKEN_BEXECUTE_SIMPLE  */
    TOKEN_BEXECUTE = 422,          /* TOKEN_BEXECUTE  */
    TOKEN_BCALL = 423,             /* TOKEN_BCALL  */
    TOKEN_JUMP_GC_AND_DUMP_CONT = 424, /* TOKEN_JUMP_GC_AND_DUMP_CONT  */
    TOKEN_JUMP_GC = 425,           /* TOKEN_JUMP_GC  */
    TOKEN_JUMP = 426,              /* TOKEN_JUMP  */
    TOKEN_BPROCEED_SIMPLE = 427,   /* TOKEN_BPROCEED_SIMPLE  */
    TOKEN_BPROCEED = 428,          /* TOKEN_BPROCEED  */
    TOKEN_CREATE_CONT = 429,       /* TOKEN_CREATE_CONT  */
    TOKEN_TEST_LEVEL = 430,        /* TOKEN_TEST_LEVEL  */
    TOKEN_CONSTRAINT_STACK_TEST = 431, /* TOKEN_CONSTRAINT_STACK_TEST  */
    TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK = 432, /* TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK  */
    TOKEN_ALIAS_CHECK = 433,       /* TOKEN_ALIAS_CHECK  */
    TOKEN_ALIAS_CHECK_REG = 434,   /* TOKEN_ALIAS_CHECK_REG  */
    TOKEN_FAIL_ON_ALIAS = 435,     /* TOKEN_FAIL_ON_ALIAS  */
    TOKEN_CHOICE_QUIET_CUT = 436,  /* TOKEN_CHOICE_QUIET_CUT  */
    TOKEN_SUSPEND_LONE_VAR = 437,  /* TOKEN_SUSPEND_LONE_VAR  */
    TOKEN_BSUSPEND = 438,          /* TOKEN_BSUSPEND  */
    TOKEN_SUSPEND_WITHOUT_REG = 439, /* TOKEN_SUSPEND_WITHOUT_REG  */
    TOKEN_CREATE_GUARD = 440,      /* TOKEN_CREATE_GUARD  */
    TOKEN_FLAT_QUIET_CUT = 441,    /* TOKEN_FLAT_QUIET_CUT  */
    TOKEN_FLAT_QUIET_COMMIT = 442, /* TOKEN_FLAT_QUIET_COMMIT  */
    TOKEN_FLAT_NOISY_WAIT = 443,   /* TOKEN_FLAT_NOISY_WAIT  */
    TOKEN_SHORTCUT_QUIET_CUT = 444, /* TOKEN_SHORTCUT_QUIET_CUT  */
    TOKEN_SHORTCUT_QUIET_COMMIT = 445, /* TOKEN_SHORTCUT_QUIET_COMMIT  */
    TOKEN_SHORTCUT_NOISY_WAIT = 446, /* TOKEN_SHORTCUT_NOISY_WAIT  */
    TOKEN_DEEP_QUIET_CUT = 447,    /* TOKEN_DEEP_QUIET_CUT  */
    TOKEN_DEEP_QUIET_COMMIT = 448, /* TOKEN_DEEP_QUIET_COMMIT  */
    TOKEN_DEEP_NOISY_WAIT = 449,   /* TOKEN_DEEP_NOISY_WAIT  */
    PREDICATE = 450,               /* PREDICATE  */
    ATOMNAME = 451,                /* ATOMNAME  */
    NEWLINE = 452,                 /* NEWLINE  */
    LPAR = 453,                    /* LPAR  */
    RPAR = 454,                    /* RPAR  */
    LHPAR = 455,                   /* LHPAR  */
    RHPAR = 456,                   /* RHPAR  */
    COMMA = 457,                   /* COMMA  */
    DASH = 458,                    /* DASH  */
    SLASH = 459,                   /* SLASH  */
    LABEL = 460,                   /* LABEL  */
    LABELTAG = 461,                /* LABELTAG  */
    NUMBER = 462,                  /* NUMBER  */
    FLOAT = 463,                   /* FLOAT  */
    END = 464,                     /* END  */
    PLABEL = 465,                  /* PLABEL  */
    CONSTANT = 466,                /* CONSTANT  */
    LIST = 467,                    /* LIST  */
    STRUCT = 468,                  /* STRUCT  */
    TYPED = 469,                   /* TYPED  */
    INTEGER = 470,                 /* INTEGER  */
    ATOM = 471,                    /* ATOM  */
    FUNCTOR = 472,                 /* FUNCTOR  */
    REG = 473,                     /* REG  */
    VOID = 474,                    /* VOID  */
    LABELED = 475,                 /* LABELED  */
    TOKEN_GET_RECORD = 476,        /* TOKEN_GET_RECORD  */
    TOKEN_PUT_RECORD = 477,        /* TOKEN_PUT_RECORD  */
    FD_code_T = 478,               /* FD_code_T  */
    FD_const_T = 479,              /* FD_const_T  */
    FD_max_elem_T = 480,           /* FD_max_elem_T  */
    FD_add_T = 481,                /* FD_add_T  */
    FD_sub_T = 482,                /* FD_sub_T  */
    FD_mult_T = 483,               /* FD_mult_T  */
    FD_divd_T = 484,               /* FD_divd_T  */
    FD_divu_T = 485,               /* FD_divu_T  */
    FD_mod_T = 486,                /* FD_mod_T  */
    FD_val_T = 487,                /* FD_val_T  */
    FD_val_0_T = 488,              /* FD_val_0_T  */
    FD_val_1_T = 489,              /* FD_val_1_T  */
    FD_val_2_T = 490,              /* FD_val_2_T  */
    FD_min_T = 491,                /* FD_min_T  */
    FD_max_T = 492,                /* FD_max_T  */
    FD_dom_min_T = 493,            /* FD_dom_min_T  */
    FD_dom_max_T = 494,            /* FD_dom_max_T  */
    FD_dom_min_0_T = 495,          /* FD_dom_min_0_T  */
    FD_dom_max_0_T = 496,          /* FD_dom_max_0_T  */
    FD_dom_min_1_T = 497,          /* FD_dom_min_1_T  */
    FD_dom_max_1_T = 498,          /* FD_dom_max_1_T  */
    FD_dom_min_2_T = 499,          /* FD_dom_min_2_T  */
    FD_dom_max_2_T = 500,          /* FD_dom_max_2_T  */
    FD_dom_T = 501,                /* FD_dom_T  */
    FD_dom_0_T = 502,              /* FD_dom_0_T  */
    FD_dom_1_T = 503,              /* FD_dom_1_T  */
    FD_dom_2_T = 504,              /* FD_dom_2_T  */
    FD_range_T = 505,              /* FD_range_T  */
    FD_outer_range_T = 506,        /* FD_outer_range_T  */
    FD_setadd_T = 507,             /* FD_setadd_T  */
    FD_setsub_T = 508,             /* FD_setsub_T  */
    FD_setmod_T = 509,             /* FD_setmod_T  */
    FD_compl_T = 510,              /* FD_compl_T  */
    FD_union_T = 511,              /* FD_union_T  */
    FD_inter_T = 512,              /* FD_inter_T  */
    FD_check_T = 513,              /* FD_check_T  */
    FD_label_T = 514,              /* FD_label_T  */
    FD_element_i_T = 515,          /* FD_element_i_T  */
    FD_element_x_T = 516           /* FD_element_x_T  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
