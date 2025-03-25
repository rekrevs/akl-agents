#ifdef __alpha__
#define TADBITS 64
#endif

/* Apple Silicon M1/M2 (ARM64) support */
#if defined(__APPLE__) && defined(__arm64__)
#define TADBITS 64
#define HAS_NO_GCVT
#endif

#ifdef mips
#define HAS_NO_GCVT
#endif

#ifdef sequent
/* For size_t: */
#include <sys/types.h>
extern double atof();
extern char *getenv();
#endif

#ifdef hpux
#define SYS5
# ifdef hppa
#  define PTR_ORG	((uword)0x40000000)
# endif
#endif

#ifdef __svr4__
#define SYS5
#endif

#ifdef macintosh
#define HAS_NO_GCVT
#define OptionalWordAlign(x)	WordAlign((unsigned long)(x))
#endif

#ifdef SYS5
#define HAS_NO_GETRUSAGE
#define HAS_NO_GETWD
#endif

#ifdef _AIX
#define unix
#define PTR_ORG		((uword)0x20000000)
#endif

