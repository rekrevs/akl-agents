/* $Id: names.h,v 1.8 1994/05/09 09:48:54 bd Exp $ */

#define HashNameArity(N,A) ((AtmHash(N) + (A)) % ATOMHASHLEN)

extern void	      	init_atomtable();
extern void		init_functortable();
extern void		init_predtable();

extern Atom	store_atom();
extern Term	store_integer_term();
extern Functor	store_functor();
extern Functor  create_functor();
extern Fp	store_float();

extern void     store_predicate();

extern struct predicate *find_predicate();
extern struct predicate *get_predicate();


