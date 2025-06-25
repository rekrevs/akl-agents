#ifdef THINK_C
#pragma options(!pack_enums)

#define word_aligned_malloc	malloc
#define word_aligned_free	free
#define word_aligned_realloc	realloc

extern void *malloc();
extern void free();
extern void *realloc();
#endif
