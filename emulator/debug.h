/* $Id: debug.h,v 1.10 1994/03/22 13:29:38 jm Exp $ */


#ifdef DEBUG
/*
#define FAIL_DEBUG
#define PROMOTE_DEBUG
#define SUSPEND_DEBUG
#define UNIFY_DEBUG

#define SCHEDULE_DEBUG
#define NONDET_DEBUG
#define COPY_DEBUG

#define GC_DEBUG
*/
#define VAR_DEBUG

#endif 


#ifdef DEBUG

#define MAXSPYPOINTS 100
#define MAXBREAKPOINTS 100

#define DisplayReadInstr \
    if(debugflag) \
    	if(!debug(andb,chb,pc,areg,"read")) return;

#define DisplayWriteInstr \
    if(debugflag) \
    	if(!debug(andb,chb,pc,areg,"write")) return;

#else

#define DisplayReadInstr 
#define DisplayWriteInstr 

#endif


#define DebugPrint(STR) fprintf(stderr,"{ Debug: %s }\n", STR);

#ifdef SUSPEND_DEBUG
#define SuspDebugPrint(STR)  DebugPrint(STR)
#else
#define SuspDebugPrint(STR) 	
#endif

#ifdef UNIFY_DEBUG
#define UnifyDebugPrint(STR)	DebugPrint(STR)
#else
#define UnifyDebugPrint(STR) 	
#endif

#ifdef SCHEDULE_DEBUG
#define ScheduleDebugPrint(STR)	DebugPrint(STR)
#else
#define ScheduleDebugPrint(STR) 
#endif

#ifdef FAIL_DEBUG
#define FailDebugPrint(STR)	DebugPrint(STR)
#else
#define FailDebugPrint(STR) 	
#endif

#ifdef PROMOTE_DEBUG
#define PromoteDebugPrint(STR)	DebugPrint(STR)
#else
#define PromoteDebugPrint(STR) 	
#endif

#ifdef NONDET_DEBUG
#define NondetDebugPrint(STR)	DebugPrint(STR)
#else
#define NondetDebugPrint(STR)	
#endif

#ifdef GC_DEBUG
#define GCDebugPrint(STR)	DebugPrint(STR)
#else
#define GCDebugPrint(STR)	
#endif

enum debugflags {
    D_CREEP,
    D_TRACE,
    D_LEAP
};

extern int debug();

extern int debugflag;

extern void initialize_debug();
