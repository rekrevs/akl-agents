/* $Id: initial.h,v 1.13 1993/06/30 09:35:17 bd Exp $ */

extern code *halt;
extern code *proceed;
extern code *fail_dec;

extern Term term_nil;
extern Term term_true;
extern Term term_fail;

extern void initialise();

extern void define();
