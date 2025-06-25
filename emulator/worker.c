/* $Id: worker.c,v 1.2 1991/11/13 12:55:51 jm Exp $ */

#include "include.h"
#ifndef APOLLO
#include <malloc.h>
#endif

void		update_context_trail();
void		update_context_task();


void init_trail(wrk,size)
     worker *wrk;
     int size;
{
  variable **seg;
  seg = (variable**)malloc(sizeof(variable*)*size);
  if(seg == NULL) {
    FatalError("could not allocate trail stack");
  }
  wrk->trail.size = size;
  wrk->trail.current = seg;
  wrk->trail.start = seg;
  wrk->trail.end = seg + size;
}

void reinit_trail(wrk)
     worker *wrk;
{
  variable **new;
  int  offset;

  wrk->trail.size += wrk->trail.size;
  new = (variable**)realloc(wrk->trail.start, sizeof(variable*)*wrk->trail.size);
  if(new == NULL)
    FatalError("could not reallocate trail stack");
  if(new != wrk->trail.start) {
    update_context_trail(wrk, wrk->trail.start, new);
    offset = wrk->trail.current - wrk->trail.start;
    wrk->trail.start = new;
    wrk->trail.current = new + offset;
  }
  wrk->trail.end = wrk->trail.start + wrk->trail.size;
}    


void init_suspend(wrk,size)
     worker *wrk;
     int size;
{
  variable **seg;
  seg = (variable**)malloc(sizeof(variable*)*size);
  if(seg == NULL) {
    FatalError("could not allocate suspend stack");
  }
  wrk->suspend.size = size;
  wrk->suspend.current = seg;
  wrk->suspend.start = seg;
  wrk->suspend.end = seg + size;
}

void reinit_suspend(wrk)
     worker *wrk;
{
  variable **new;
  int  offset;

  wrk->suspend.size += wrk->suspend.size;
  new = (variable**)realloc(wrk->suspend.start, sizeof(variable*)*wrk->suspend.size);
  if(new == NULL)
    FatalError("could not allocate suspend stack");
  if(new != wrk->suspend.start) {
    offset = wrk->suspend.current - wrk->suspend.start;
    wrk->suspend.start = new;
    wrk->suspend.current = new + offset;
  }
  wrk->suspend.end = wrk->suspend.start + wrk->suspend.size;
}


void init_task(wrk,size)
     worker *wrk;
{
  task *seg;
  seg = (task*)malloc(sizeof(task)*size);
  if(seg == NULL) {
    FatalError("could not allocate task stack");
  }
  wrk->task.size = size;
  wrk->task.current = seg;
  wrk->task.start = seg;
  wrk->task.end = seg + size;
}

void reinit_task(wrk)
     worker *wrk;
{
  task *new;
  int  offset;

  wrk->task.size += wrk->task.size;
  new = (task *)realloc(wrk->task.start, sizeof(task)*wrk->task.size);
  if(new == NULL)
    FatalError("could not allocate task stack");
  if(new != wrk->task.start) {
    update_context_task(wrk,wrk->task.start,new);
    offset = wrk->task.current - wrk->task.start;
    wrk->task.start = new;
    wrk->task.current = new + offset;
  }
  wrk->task.end = wrk->task.start + wrk->task.size;
}    

task *shift_andb_tasks(wrk,i,n)
     worker *wrk;
     int i, n;
{
  task *current, *end, *top, *insert;
  context *from, *cntxt;

  current = wrk->task.current;
  end = wrk->task.end;
  
  top = current + i;
  while(top > end) {
    reinit_task(wrk);
    current = wrk->task.current;
    end = wrk->task.end;
    top = current + i;
  }

  /* the uppermost contexts should not change */
  from = wrk->context.start + n;

  if(current != from->andb) {
    task *tsk, *newt;
    tsk = current;
    newt = tsk + i;
    do {
      tsk--;
      newt--;
      *newt = *tsk;
    } while( tsk != from->andb);
  }

  insert = from->andb;

  wrk->task.current = current + i;

  /* update the context task field */
  
  for(cntxt = wrk->context.current; cntxt != from; cntxt--) {
    cntxt->chb = cntxt->chb + i;      
    cntxt->andb = cntxt->andb + i;
  }
  return insert;
}


void init_context(wrk,size)
     worker *wrk;
{
  context *seg;
  seg = (context *)malloc(sizeof(context)*size);
  if(seg == NULL) {
    FatalError("could not allocate context stack");
  }
  wrk->context.size = size;
  wrk->context.current = seg;
  wrk->context.start = seg;
  wrk->context.end = seg + size;
}

void reinit_context(wrk)
     worker *wrk;
{
  context *new;
  int  offset;

  wrk->context.size += wrk->context.size;
  new = (context *)realloc(wrk->context.start, sizeof(context)*wrk->context.size);
  if(new == NULL)
    FatalError("could not allocate context stack");
  if(new != wrk->context.start) {
    offset = wrk->context.current - wrk->context.start;
    wrk->context.start = new;
    wrk->context.current = new + offset;
  }
  wrk->context.end = wrk->context.start  + wrk->context.size;
}    


void newcontext(wrk)
     worker *wrk;
{
  wrk->context.current++;
  if(wrk->context.current == wrk->context.end){
    reinit_context(wrk);
  }
  wrk->context.current->chb = wrk->task.current;
  wrk->context.current->andb = wrk->task.current;
  wrk->context.current->trail = wrk->trail.current;
}

void resetcontext(wrk)
     worker *wrk;
{
  wrk->context.current--;
}

void update_context_task(wrk,old,new)
     worker *wrk;
     task *old, *new;
{
  context *cntx;
  int offchb, offandb;
  
  cntx = wrk->context.start;
  for(cntx = wrk->context.current; cntx != wrk->context.start; cntx--){
    offchb = cntx->chb - old;
    offandb = cntx->andb - old;
    cntx->chb = new + offchb;
    cntx->andb = new + offandb;
  }
}


void update_context_trail(wrk, old,new)
     worker *wrk;
     variable **old, **new;
{
  context *cntx;
  int offset;
  cntx = wrk->context.start;
  for(cntx = wrk->context.current; cntx != wrk->context.start; cntx--){
    offset = cntx->trail - old;
    cntx->trail = new + offset;
  }
}

worker *init_worker()
{

  worker *wrk;
  int i;

  wrk = (worker *)malloc(sizeof(worker));
  if(wrk == NULL)
    FatalError("could not allocate worker");

  init_trail(wrk,AKLTRAILSTACKSIZE);
  init_suspend(wrk,AKLSUSPENDSTACKSIZE);
  init_task(wrk,AKLTASKSTACKSIZE);
  init_context(wrk,AKLCONTEXTSTACKSIZE);

  wrk->pc = NULL;
  wrk->yreg = NULL;
  wrk->def = NULL;
  wrk->arity = 0,
  wrk->str = NULL;
  wrk->insert = NULL;
  wrk->root = NULL;
  wrk->andb  = NULL;
  wrk->chb  = NULL;
  for(i = 0; i < MAX_AREGS; i++) {
    wrk->areg[i] = NULL;
  }
  return wrk;
}
