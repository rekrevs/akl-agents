/* $Id: configuration.c,v 1.1 1991/12/19 16:36:24 jm Exp $ */

#include "include.h"

#ifndef APOLLO
#include <malloc.h>
#endif

void		update_context_trail();
void		update_context_task();


configuration *freelist = NULL;

void init_trail(cnf,size)
     configuration *cnf;
     int size;
{
  variable **seg;

  seg = (variable**)malloc(sizeof(variable*)*size);
  if(seg == NULL) {
    FatalError("could not allocate trail stack");
  }
  cnf->trail.size = size;
  cnf->trail.current = seg;
  cnf->trail.start = seg;
  cnf->trail.end = seg + size;
}

void reinit_trail(cnf)
     configuration *cnf;
{
  variable **new;
  int  offset;

  cnf->trail.size += cnf->trail.size;
  new = (variable**)realloc(cnf->trail.start, sizeof(variable*)*cnf->trail.size);
  if(new == NULL)
    FatalError("could not reallocate trail stack");
  if(new != cnf->trail.start) {
    
    update_context_trail(cnf, cnf->trail.start, new);
    offset = cnf->trail.current - cnf->trail.start;
    cnf->trail.start = new;
    cnf->trail.current = new + offset;
  }
  cnf->trail.end = cnf->trail.start + cnf->trail.size;
}    


void init_suspend(cnf,size)
     configuration *cnf;
     int size;
{
  variable **seg;
  seg = (variable**)malloc(sizeof(variable*)*size);
  if(seg == NULL) {
    FatalError("could not allocate suspend stack");
  }
  cnf->suspend.size = size;
  cnf->suspend.current = seg;
  cnf->suspend.start = seg;
  cnf->suspend.end = seg + size;
}

void reinit_suspend(cnf)
     configuration *cnf;
{
  variable **new;
  int  offset;

  cnf->suspend.size += cnf->suspend.size;
  new = (variable**)realloc(cnf->suspend.start, sizeof(variable*)*cnf->suspend.size);
  if(new == NULL)
    FatalError("could not allocate suspend stack");
  if(new != cnf->suspend.start) {
    offset = cnf->suspend.current - cnf->suspend.start;
    cnf->suspend.start = new;
    cnf->suspend.current = new + offset;
  }
  cnf->suspend.end = cnf->suspend.start + cnf->suspend.size;
}


void init_task(cnf,size)
     configuration *cnf;
{
  task *seg;
  seg = (task*)malloc(sizeof(task)*size);
  if(seg == NULL) {
    FatalError("could not allocate task stack");
  }
  cnf->task.size = size;
  cnf->task.current = seg;
  cnf->task.start = seg;
  cnf->task.end = seg + size;
}

void reinit_task(cnf)
     configuration *cnf;
{
  task *new;
  int  offset;

  cnf->task.size += cnf->task.size;
  new = (task *)realloc(cnf->task.start, sizeof(task)*cnf->task.size);
  if(new == NULL)
    FatalError("could not allocate task stack");
  if(new != cnf->task.start) {
    update_context_task(cnf,cnf->task.start,new);
    offset = cnf->task.current - cnf->task.start;
    cnf->task.start = new;
    cnf->task.current = new + offset;
  }
  cnf->task.end = cnf->task.start + cnf->task.size;
}    

task *shift_andb_tasks(cnf,i,n)
     configuration *cnf;
     int i, n;
{
  task *current, *end, *top, *insert;
  context *from, *cntxt;

  current = cnf->task.current;
  end = cnf->task.end;
  
  top = current + i;
  while(top > end) {
    reinit_task(cnf);
    current = cnf->task.current;
    end = cnf->task.end;
    top = current + i;
  }

  /* the uppermost contexts should not change */
  from = cnf->context.start + n;

  if(current != from->andb) {
    task *tsk, *newt;
    tsk = current;
    newt = tsk + i;
    for(;tsk != from->andb; ){
      tsk--;
      newt--;
      *newt = *tsk;
    }
  }

  insert = from->andb + 1;

  cnf->task.current = current + i;

  /* update the context task field */
  
  for(cntxt = cnf->context.current; cntxt != from; cntxt--) {
    cntxt->chb = cntxt->chb + i;      
    cntxt->andb = cntxt->andb + i;
  }
  return insert;
}


void init_context(cnf,size)
     configuration *cnf;
{
  context *seg;
  seg = (context *)malloc(sizeof(context)*size);
  if(seg == NULL) {
    FatalError("could not allocate context stack");
  }
  cnf->context.size = size;
  cnf->context.current = seg;
  cnf->context.start = seg;
  cnf->context.end = seg + size;
}

void reinit_context(cnf)
     configuration *cnf;
{
  context *new;
  int  offset;

  cnf->context.size += cnf->context.size;
  new = (context *)realloc(cnf->context.start, sizeof(context)*cnf->context.size);
  if(new == NULL)
    FatalError("could not allocate context stack");
  if(new != cnf->context.start) {
    offset = cnf->context.current - cnf->context.start;
    cnf->context.start = new;
    cnf->context.current = new + offset;
  }
  cnf->context.end = cnf->context.start  + cnf->context.size;
}    


void newcontext(cnf)
     configuration *cnf;
{
  cnf->context.current++;
  if(cnf->context.current == cnf->context.end){
    reinit_context(cnf);
  }
  cnf->context.current->chb = cnf->task.current;
  cnf->context.current->andb = cnf->task.current;
  cnf->context.current->trail = cnf->trail.current;
}

void resetcontext(cnf)
     configuration *cnf;
{
  cnf->context.current--;
}

void update_context_task(cnf,old,new)
     configuration *cnf;
     task *old, *new;
{
  context *cntx;
  int offchb, offandb;
  
  cntx = cnf->context.start;
  for(cntx = cnf->context.current; cntx != cnf->context.start; cntx--){
    offchb = cntx->chb - old;
    offandb = cntx->andb - old;
    cntx->chb = new + offchb;
    cntx->andb = new + offandb;
  }
}


void update_context_trail(cnf, old,new)
     configuration *cnf;
     variable **old, **new;
{
  context *cntx;
  int offset;
  cntx = cnf->context.start;
  for(cntx = cnf->context.current; cntx != cnf->context.start; cntx--){
    offset = cntx->trail - old;
    cntx->trail = new + offset;
  }
}

configuration *new_configuration()
{
  configuration *cnf;

  if(freelist != NULL) {
    cnf = freelist;
    freelist = freelist->next;
    cnf->trail.current = cnf->trail.start;
    cnf->task.current = cnf->task.start;
    cnf->context.current = cnf->context.start;    
  } else {
    cnf = (configuration *)malloc(sizeof(configuration));
    if(cnf == NULL)
      FatalError("could not allocate configuration");
    init_trail(cnf,AKLTRAILSTACKSIZE);
    init_suspend(cnf,AKLSUSPENDSTACKSIZE);
    init_task(cnf,AKLTASKSTACKSIZE);
    init_context(cnf,AKLCONTEXTSTACKSIZE);
  }
  return cnf;
}


void return_configuration(cnf)
     configuration* cnf;
{
  cnf->next = freelist;
  freelist = cnf;
}

configuration *init_configuration()
{

  configuration *cnf;
  int i;

  cnf = new_configuration();
  cnf->father = NULL;
  cnf->close = NULL;
  cnf->cva = NULL;
  cnf->pc = NULL;
  cnf->arity = 0;
  cnf->def = NULL;
  cnf->insert = NULL;
  cnf->root = NULL;
  cnf->andb  = NULL;

  for(i = 0; i < MAX_AREGS; i++) {
    cnf->areg[i] = NULL;
  }
  return cnf;
}


