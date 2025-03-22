/* $Id: unify.h,v 1.9 1993/11/18 11:16:23 bd Exp $ */

#ifndef UNIFY_INCLUDED
#define UNIFY_INCLUDED

typedef struct {
  Term *tuple1;
  Term *tuple2;
  int index;
} unify_entry;

#define UNIFY_TABLE_LENGTH		16

#define UNIFY_STACK_LENGTH		50

struct unifystate {
  struct hash_table	*table;
  int			table_used_flag;
  int			stack_length;
  unify_entry		*sp;
  unify_entry		*stack_end;
  unify_entry		*stack;
};

extern bool unify();
extern bool unify_internal();	/* For use by partially open-coded unify */
extern bool unify_more();	/* For callback from generic objects */
extern bool unify_alias();	/* For generics with direct cycles */
extern void init_unifystate();

#endif
