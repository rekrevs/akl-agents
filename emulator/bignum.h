/* $Id: bignum.h,v 1.4 1994/03/07 16:02:25 bd Exp $ */


extern Term hard_mul_int(sword x, sword y);
extern Term hard_shift_int(sword x, sword y, int negate);

extern Term bignum_add(Term x, Term y);
extern Term bignum_sub(Term x, Term y);
extern Term bignum_mul(Term x, Term y);
extern Term bignum_div(Term x, Term y);
extern Term bignum_mod(Term x, Term y);
extern Term bignum_and(Term x, Term y);
extern Term bignum_or(Term x, Term y);
extern Term bignum_xor(Term x, Term y);
extern Term bignum_min(Term x, Term y);
extern Term bignum_max(Term x, Term y);

extern Term bignum_shift(Term x, Term y, int negate);

extern Term bignum_identity(Term x);
extern Term bignum_neg(Term x);
extern Term bignum_com(Term x);
extern Term bignum_inc(Term x);
extern Term bignum_dec(Term x);

extern void	initialize_bignum();
