/* $Id: c.h,v 1.7 1993/03/08 12:17:04 bd Exp $ */

/* This file contains definitions that could possibly vary with
 * different C implementations, and also some other C and C-library
 * related definitions.
 */

#ifndef sequent
#include <stdlib.h>
#endif

#define ANY 1			/* Number of elts in dummy arrays */

typedef unsigned char u8;	/* unsigned  8 bit item */
typedef unsigned short int u16;	/* unsigned 16 bit item */
typedef short s16;		/*   signed 16 bit item */
typedef unsigned long u32;	/* unsigned 32 bit item */
typedef long s32;		/*   signed 32 bit item */

#define NO_FUNNY_ANSI_STUFF

#ifdef NO_FUNNY_ANSI_STUFF
#define VOLATILE
#define PROTO(ignore) ()
#else
#define VOLATILE volatile
#define PROTO(argl) argl
#endif

#define SAME 0			/* strcmp => same */
