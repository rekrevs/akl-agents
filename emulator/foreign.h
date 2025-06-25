/* $Id: foreign.h,v 1.9 1993/05/19 13:48:04 sverker Exp $ */

extern bool expand_file_name PROTO((char *name, char *target));
extern void compute_cwd();
extern void break_to_gdb();
    
extern void initialize_foreign();
