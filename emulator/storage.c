/* $Id: storage.c,v 1.27 1993/10/06 11:39:41 bd Exp $ */ 

#include "include.h"
#include "term.h"
#include "initial.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "storage.h"
#include "error.h"


#if !defined(APOLLO) && !defined(macintosh) && !defined(__APPLE__)
#include <malloc.h>
#endif


block 		*global;
block 		*current_block;
block 		*unused;
block 		*used;

block 		*consth;

heap 		*glob_heapend;
heap 		*glob_heapcurrent;

heap 		*constspace;
heap 		*constcurrent;
heap 		*constend;

gcvalue		gcflag;

int		limit;
int		active;

int 		blocksize;
int 		constsize;



/* Heap allocation stuff */


block *balloc(size)
     int size;
{
  block *new;
  char *seg;
  new = (block *)malloc(sizeof(block));
  seg = (char *)malloc(size);
  if(new != NULL && seg != NULL) {
    new->next = NULL;
    new->start = seg;
    new->end = seg + size;
    return new;
  } else {
    return NULL;
  }
}
  
flipstate currentflip = FLIP;

block *heap_block()
{
  block *free;
  if(unused == NULL) {
    free = balloc(blocksize);
    if(free == NULL) {
      FatalError("couldn't allocate heap space");
    }
  } else {
    free = unused;
    unused = unused->next;
    free->next = NULL;
  }
  free->logical_end = free->end;	/* [BD] Trust me. */
  free->flipflop = currentflip;
  return free;
}


long gcratio;

void init_heap(size,lmt)
    unsigned int size, lmt;
{
  gcratio = lmt;
  limit = AKLHEAPLIMIT;
  blocksize = size;
  
  unused = NULL;
  used = NULL;

  global = heap_block();
  
  active = 1;
  
  current_block = global;
  heapcurrent = current_block->start;  
  heapend = current_block->end;

  gcflag = OFF;
}

void reinit_heap(ret, size_needed)
   heap *ret;
   long size_needed;
{
  if (size_needed > blocksize) {
    char buf[256];
    sprintf(buf, "Cannot create object larger than %ld bytes", blocksize);
    FatalError(buf);
  }
  if(active >= limit) {
    gcflag = ON;
  }
  active++;
  current_block->logical_end = ret;
  current_block->next = heap_block();
  current_block = current_block->next;
  heapcurrent = current_block->start;
  heapend = current_block->end;
}


void restore_heap_block(saved_current)
    block *saved_current;
{
  block *last;

  current_block = saved_current;

  for (last = current_block->next; last->next != NULL; last = last->next) {
    active -= 1;
  }
  active -= 1;

  last->next = unused;
  unused = current_block->next;
  current_block->logical_end = current_block->end;	/* [BD] See heap_block(). */
  current_block->next = NULL;
}


void gc_begin()
{
  used = global;

  currentflip = (currentflip == FLIP ? FLOP : FLIP);

  global = heap_block();
  
  active = 1;
  
  current_block = global;

  heapcurrent = current_block->start;  
  heapend = current_block->end;
  gcflag = OFF;
}

void gc_end()
{
  block *last = used;
  for(;;) {
#ifdef DEBUG
    heap *ptr = last->start;
    /* Clear out old heap block, so we possibly may detect more errors */
    while (ptr != last->end) {
      *ptr++ = 0;
    }
#endif
    if (last->next == NULL)
      break;
    last = last->next;
  }
  last->next = unused;
  unused = used;
  used = NULL;
  /* change the limit depending on the number of blocks used */
  limit = (100 * active) / gcratio;
}

heap* heap_start()
{
  return current_block->start;
}

int heap_size()
{
  return  (blocksize * (active -1)) + (heapcurrent - current_block->start);
}



void init_constspace(size)
    unsigned int size;
{
  constsize = size;
  consth = balloc(size);
  if(consth == NULL)
    FatalError("couldn't allocate constant area");
  constspace = consth->start;
  constend = consth->end;
  constcurrent = constspace;
}


  
void init_alloc()
{
  char *cp;
  int bsize, lmt, asize;
  
  bsize = (cp = getenv("AKLBLOCKSIZE")) ? atoi(cp) : AKLBLOCKSIZE;
  lmt =   (cp = getenv("AKLGCRATIO"))   ? atoi(cp) : AKLGCRATIO;
  asize = (cp = getenv("AKLCONSTSIZE")) ? atoi(cp) : AKLCONSTSIZE;

  init_heap(bsize,lmt);
  init_constspace(asize);
}



char *salloc(size)
    unsigned size;
{
  char *ret;
  ret = malloc(size);
  if(ret == NULL) {
    FatalError("could not allocate memory");
  }
  return ret;
}

char *qalloc(size)
    unsigned size;
{
  char *ret;
  unsigned int asize = WordAlign(size);
  ret = (char *) constcurrent;
  constcurrent += asize/sizeof(*constcurrent);
  if(constcurrent > constend) {
    FatalError("out of constant space");
  }
  return ret;
}


bool akl_garbage_collect(Arg)
     Argdecl;
{
  gcflag = ON;
  return TRUE;
}


void initialize_storage() {

  define("garbage_collect",akl_garbage_collect,0);
}
