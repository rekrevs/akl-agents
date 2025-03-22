/* $Id: copy.h,v 1.13 1994/04/18 15:04:12 bjornc Exp $ */

extern andbox *copy();
extern int	is_copied();
extern void	copy_location();
extern void	copy_locations();
extern void	copy_tuple();
extern void 	copy_forward();
extern void     copy_todo();
extern void     copy_xtodo();
extern void local_suspensions();
extern void external_suspensions();
