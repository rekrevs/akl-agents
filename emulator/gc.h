/* $Id: gc.h,v 1.15 1994/04/18 15:04:24 bjornc Exp $ */

extern void	gc();
extern void 	gc_configuration();
extern void	gc_location();
extern void	gc_locations();
extern void	gc_tuple();
extern void     gc_susps();
extern void	gc_exstate();
extern void     gc_forward();
extern void     gc_todo();
extern void gc_save_susp();

typedef struct gcstate *gcstatep;
