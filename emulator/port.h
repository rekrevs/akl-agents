/* $Id: port.h,v 1.10 1994/03/29 13:59:46 bd Exp $ */

extern void initialize_port();

extern bool akl_send_3();

extern predicate *send3_def;

extern constraint	*make_port_constraint();
extern constraint	*copy_referenced_constraint();
extern constraint	*gc_referenced_constraint();
extern void		close_port_stream();
extern int		port_needs_copy_close();
