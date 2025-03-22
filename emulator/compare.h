/* $Id: compare.h,v 1.2 1994/04/14 14:53:59 boortz Exp $ */

typedef enum {
  CMP_LESS,
  CMP_EQUAL,
  CMP_GREATER
} compres_val;

extern void initialize_compare();
extern bool akl_compare_aux(); /* (Term, Term, exstate, compres_val *); */
