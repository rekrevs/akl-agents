/* $Id: constants.h,v 1.27 1993/10/14 18:45:35 bd Exp $ */ 

#define BOOTMAX 42

#define AKLBLOCKSIZE	(200 * 1024)
#define AKLHEAPLIMIT	8
#ifdef macintosh
#define AKLGCRATIO	50
#else
#define AKLGCRATIO	25
#endif

#define AKLCONSTSIZE  	(400 * 1024)

#ifdef macintosh
#define AKLCODESIZE	(250 * 1024)
#else
#define AKLCODESIZE	(1000 * 1024)
#endif

#define AKLTRAILSIZE	(1 * 1024)
#define AKLTASKSIZE	(1 * 1024)
#define AKLCONTEXTSIZE	(64)
#define AKLSUSPENDSIZE	(64)



#define AKLGCREFS	(4 * 1024)

#define AKLGCSUSP	(512)
#ifdef macintosh
#define AKLGCGEN	(1 * 1024)
#else
#define AKLGCGEN	(10 * 1024)
#endif

#define AKLCOPYREFS	(1 * 1024)

#define AKLCOPYTRM	(1 * 1024)
#define AKLCOPYSVA 	(256)
#define AKLCOPYXTR 	(256)
#define AKLCOPYXGN 	(256)
#define AKLCOPYSUS 	(256)
#define AKLCOPYCHK 	(256)

#define ATOMHASHLEN	269

#define MAXATOMLEN	255

#define MAXPATHLEN	1024

#ifdef macintosh
#define MAX_AREGS  	256
#else
#define MAX_AREGS  	8192
#endif

#define MAXNUMLEN	 2048		/* Set to right value later /KB */

#define MAXLABELS 	 1023


#define SP_FLOAT_SIGNIF 17
