/* $Id: bignum.h,v 1.4 1994/03/07 16:02:25 bd Exp $ */


extern Term	hard_mul_int PROTO((sword x, sword y));
extern Term	hard_shift_int PROTO((sword x, sword y, int negate));

extern Term	bignum_add PROTO((Term x, Term y));
extern Term	bignum_sub PROTO((Term x, Term y));
extern Term	bignum_mul PROTO((Term x, Term y));
extern Term	bignum_div PROTO((Term x, Term y));
extern Term	bignum_mod PROTO((Term x, Term y));
extern Term	bignum_and PROTO((Term x, Term y));
extern Term	bignum_or PROTO((Term x, Term y));
extern Term	bignum_xor PROTO((Term x, Term y));
extern Term	bignum_min PROTO((Term x, Term y));
extern Term	bignum_max PROTO((Term x, Term y));

extern Term	bignum_shift PROTO((Term x, Term y, int negate));

extern Term	bignum_identity PROTO((Term x));
extern Term	bignum_neg PROTO((Term x));
extern Term	bignum_com PROTO((Term x));
extern Term	bignum_inc PROTO((Term x));
extern Term	bignum_dec PROTO((Term x));

extern void	initialize_bignum();
