/* $Id: predicate.h,v 1.6 1994/03/22 13:30:26 jm Exp $ */

typedef enum {
    ENTER_EMULATED,
    ENTER_C,
    ENTER_UNDEFINED
} enter_instruction;


typedef enum {
    EMULATED_PREDICATE,
    C_PREDICATE,
    UNDEFINED_PREDICATE
} predicate_type;

union definfo {
  code 	*incoreinfo;
  bool 	(*cinfo)();
};


typedef struct predicate {
    enter_instruction	enter_instruction;
    Atom 		name;
    uword 		arity;
    uword 		ext;    
    predicate_type 	predicate_type;
    union definfo 	code;
    bool 		protected;
#ifdef TRACE
    struct predicate 	*spypoint;    
#endif
} predicate;

/*
typedef struct module {
  struct module		*next;
  Atom			name;
  predicate		*members;
  bool			protected;
} module;
*/

extern predicate *make_e_predicate();
extern predicate *make_c_predicate();
extern predicate *make_u_predicate();

extern predicate *make_abstraction();

extern void 	define_e_predicate();
extern void	define_c_predicate();

extern void	define_abstraction();

extern bool 	pred_protection;
extern bool 	pred_protect;

extern void initialize_predicate();
