#ifdef macintosh

#include "macintosh.h"

#ifdef THINK_C
#pragma options(pack_enums)
#include <Mac #includes.c>
#endif

#include <size_t.h>
#include <stdio.h>

#ifdef THINK_C
#include <console.h>
#endif

/* Word aligned malloc, free, and realloc. Tag memory block with size if given
 * block was word-aligned, or -size if it was odd-halfword-aligned.
 */

#define ALIGN4(x) (((x) + 3) & ~((unsigned int) 3))

void *word_aligned_malloc(size)
    size_t size;
{
  unsigned int a;
  size = ALIGN4(size);
  a = (unsigned int) NewPtr(size+sizeof(int)+2);
  if (a == 0) return (void*)0;
  if (a & 2) {
    a += 2;
    *(int*)a = -size;
  } else {
    *(int*)a = size;
  }
  return (void*)(a+sizeof(int));
}

void word_aligned_free(ptr)
    void *ptr;
{
  char *p = ptr;
  int s = *(int*) (p-sizeof(int));
  p -= (s < 0 ? 2 : 0)+sizeof(int);
  DisposPtr(p);
  return;
}

void *word_aligned_realloc(ptr,size)
    void *ptr;
    size_t size;
{
  unsigned int a;
  char *p = ptr;
  int s = *(int*) (p-sizeof(int));

  size = ALIGN4(size);

  p -= sizeof(int);

  if (s < 0) {
    *(int*) p = -size;
    p -= 2;
    SetPtrSize(p, size+sizeof(int)+2);
  } else {
    *(int*) p = size;
    SetPtrSize(p, size+sizeof(int));
  }

  if (MemErr == noErr) {
    return ptr;
  } else {
    /* Couldn't grow/shrink -- gotta try and copy to new block */
    int n, *src, *dest;
    char *new = (char *) word_aligned_malloc(size);
    if (new == 0) {
      return (void*)0;
    }
    if (s < 0) {
      n = (-s) >> 2;
    } else {
      n = s >> 2;
    }
    src = (int*)ptr;
    dest = (int*)new;
    while (n)
    {
      *dest = *src; src += 1; dest += 1; n -= 1;
    }
    DisposPtr(p);
    return (void*)new;
  }
}

/* Simple alloca() that allocates on the AKL heap. */

void *alloca(size)
    size_t size;
{
#define heapcurrent glob_heapcurrent
#define heapend     glob_heapend
  extern char *heapcurrent;
  extern char *heapend;
  char *p = heapcurrent;

  size = ALIGN4(size);
  heapcurrent += size;
  if (heapcurrent >= heapend) {
    reinit_heap(p, size);
    p = heapcurrent;
    heapcurrent += size;
  }
  return p;
}


static int start_tick;

int systime()
{
  int ticks = TickCount() - start_tick;
  return ticks*16 + (ticks*2)/3;		/* Gives milliseconds */
}


static char *sample_argv[8];
static char curapnamebuf[32];

int setup_argv(argv)
    char **argv[];
{
  int argc;
  FILE *fp;

  start_tick = TickCount();

#ifdef THINK_C
  {
    KeyMap keys;
    GetKeys(keys);
    if (keys[1] & 0x4) {	/* The "Option" key in case you wonder... */
      argc = ccommand(argv);
      return argc;
    }
  }
#endif

  fp = fopen("boot.pam", "r");
  if (fp != NULL) {
    fclose(fp);
    sprintf(curapnamebuf, "%#s", CurApName);
    sample_argv[0] = curapnamebuf;
    sample_argv[1] = "-b";
    sample_argv[2] = "boot.pam";
    sample_argv[3] = sample_argv[1];
    sample_argv[4] = "version.pam";
    sample_argv[5] = sample_argv[1];
    sample_argv[6] = "comp.pam";
    sample_argv[7] = NULL;
    argc = 8;
  } else {
    sprintf(curapnamebuf, "%#s", CurApName);
    sample_argv[0] = curapnamebuf;
    sample_argv[1] = "-b";
    sample_argv[2] = sample_argv[0];    
    sample_argv[3] = NULL;
    argc = 4;
  }
  *argv = sample_argv;
  return argc;
}

#endif
