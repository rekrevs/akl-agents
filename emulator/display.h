/* $Id: display.h,v 1.15 1994/04/07 13:40:27 bd Exp $ */

extern void display_term PROTO((Term, int));
extern void fdisplay_term PROTO((FILE *, Term, int));
extern void fdisplay_cyc_term PROTO((FILE *, Term, int, struct ptrdict *));
extern void display_configuration PROTO((andbox *, choicebox *));
extern void display_flat_andbox PROTO((andbox *, choicebox *));
extern void display_choicebox PROTO((choicebox *, choicebox *));
extern int  display_code PROTO((code *));
extern void display_constraint PROTO((constraint *));
extern void display_variable PROTO((FILE *, Term, int, struct ptrdict *));

extern void initialize_varname();
extern void gc_varname();
extern long variable_descriptor();

extern void initialize_display();


