/* $Id: debug.c,v 1.17 1994/03/25 14:42:14 jm Exp $ */ 

#include "include.h"
#include "term.h"
#include "initial.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "engine.h"
#include "display.h"
#include "instructions.h"
#include "debug.h"

#include "decodeinstr.h"
#include "instrdefs.h"

int debugflag = FALSE;		/* initially off */


int readstring(x)
char *x;
{
  int i = 0;
  int c;
  while( TRUE ) {
    c = getchar();
    if(c == EOF)
      return 0;
    if(c == '\n') {
      x[i++] = '\n';
      x[i] = 0;
      return 1;
    }
    if(c == 8) {
      if(i>0)
	i--;
    } else {
      x[i] = c;
      i++;
    }  
  }
}


#ifdef DEBUG

static int	debugmode;

static int monitorpoints[MAXSPYPOINTS];
static code *breakpoints[MAXBREAKPOINTS];

static int spyc;
static int breakc;
static code *skipbreak;

#endif


bool akl_pam_debug(Arg)
    Argdecl;
{
#ifdef DEBUG
    debugflag = TRUE;
    
    debugmode = D_CREEP;
    skipbreak = 0;
    spyc = 0;
    breakc = 0;
#else
    printf("{ No PAM-level debugging available.  Recompile with -DDEBUG. }\n");
#endif
    return TRUE;
}


bool akl_pam_nodebug(Arg)
    Argdecl;
{
#ifdef DEBUG
    debugflag = FALSE;
#else
    printf("{ No PAM-level debugging available.  Recompile with -DDEBUG. }\n");
#endif
    return TRUE;
}


#ifdef DEBUG

int in_break(pc)
    code *pc;
{
    int i;
    for(i = 0 ; i != breakc ; i++) {
	if(pc == breakpoints[i]) return TRUE;
    }
    return FALSE;
}

int in_spy(inst)
    code inst;
{
    int i;
    for(i = 0 ; i != spyc ; i++) {
	if(inst == monitorpoints[i]) return TRUE;
    }
    return FALSE;
}

void add_spy(inst)
    code inst;
{
    int i;
    for(i = 0 ; i != spyc ; i++) {
	if(inst == monitorpoints[i]) return;
    }
    if(spyc >= MAXSPYPOINTS){
	printf("To many spy points\n");
	return;
    }
    monitorpoints[spyc++] = inst;
}

void add_break(pc)
    code *pc;
{
    int i;
    for(i = 0 ; i != breakc ; i++) {
	if(pc == breakpoints[i]) return;
    }
    if(breakc >= MAXBREAKPOINTS){
	printf("To many break points\n");
	return;
    }
    breakpoints[breakc++] = pc;
}

void delete_spy(inst)
    code inst;
{
    int i,j;
    for(i = 0 ; i != spyc ; i++) {
	if(inst == monitorpoints[i]) {
	    for(j = i ; j < spyc-1 ; j++)
		monitorpoints[j] = monitorpoints[j+1];
	    spyc--;
	    return;
	}
    }
}

void delete_break(pc)
    code *pc;
{
    int i,j;
    for(i = 0 ; i != breakc ; i++) {
	if(pc == breakpoints[i]) {
	    for(j = i ; j < breakc-1 ; j++)
		breakpoints[j] = breakpoints[j+1];
	    breakc--;
	    return;
	}
    }
}


void display_current_state(andb,chb,pc,xreg,rwmc)
    andbox *andb;
    choicebox *chb;
    code *pc;
    Term *xreg;
    char *rwmc;
{
    int i;
    predicate *pl;
    opcode op;

    fprintf(stdout,"pc:\t%#lx \n", pc);
    fprintf(stdout,"andb:\t%#lx \n", andb);
    if(chb != NULL)
      if(chb->def != NULL)
    	fprintf(stdout,"chb:\t%s/%ld \n",
		AtmPname(chb->def->name), chb->def->arity);
      else
    	fprintf(stdout,"chb:\t%s/%d \n", "ROOT", 0);	
    fprintf(stdout,"Mode:\t%s\t-- ",rwmc);
    
    display_code(pc);

    /* Display arguments to CALL and EXECUTE */

    FetchOpCode(0);
    if (op == EnumToCode(CALL) || op == EnumToCode(EXECUTE)) {
      Instr_Pred(CodeToEnum(op), pl);
#ifdef BAM
    } else if (op == EnumToCode(BEXECUTE)) {
      Instr_Pred(CodeToEnum(op), pl);
    } else if (op == EnumToCode(BCALL)) {
      Instr_Pred_Bitmask(BCALL, pl);
#endif
    } else {
      return;
    }

    for(i = 0 ; i != pl->arity ; i++) {
      fprintf(stdout,"\nareg[%d]:\t",i);
      display_term(xreg[i],-1);
    }
}

int debug(andb,chb,pc,xreg,rwmc)
    andbox 	*andb;
    choicebox   *chb;
    code 	*pc;
    Term 	*xreg;
    char 	*rwmc;
{
    char 	debug_str[255];
    int		i,j;
    int 	size;

    switch(debugmode) {
    case D_LEAP:
	if(in_break(pc))
	    break;
	if(in_spy(*pc))
	    break;
	if(pc == skipbreak) {
	    skipbreak = NULL;
	    break;
	}
	return 1;
    case D_CREEP:
	break;
    case D_TRACE:
	if(in_break(pc))
	    break;
	if(in_spy(*pc))
	    break;
	display_current_state(andb,chb,pc,xreg,rwmc);
	putchar('\n');
	return 1;
    default:
	printf("No such debug mode\n");
	return 1;
    }

    display_current_state(andb,chb,pc,xreg,rwmc);
    
 debugstart:
    
    fprintf(stdout,"\n> ");
    readstring(debug_str);
    
    switch(debug_str[0]) {
    case 'b':
	if((GetOpCode(pc) == EnumToCode(EXECUTE)) ||
	   (GetOpCode(pc) == EnumToCode(PROCEED))) {
	    printf("break on: ");
	    display_code(andb->cont->label);
	    add_break(andb->cont->label);
	    goto debugstart;
	} else {
	    printf("breaking on: ");
	    display_code((pc+1));
	    add_break((pc+1));
	    goto debugstart;
	}
    case 'r':
	printf("no break on: ");
	display_code(pc);
	delete_break(pc);
	goto debugstart;
    case 's':
	if((GetOpCode(pc) == EnumToCode(EXECUTE)) ||
	   (GetOpCode(pc) == EnumToCode(PROCEED))) {
	    skipbreak = andb->cont->label;
	    debugmode = D_LEAP;	    
	} else {
	    skipbreak = pc;
	    debugmode = D_LEAP;
	}
	return 1;
    case 'q':
    case 'a':
	return 0;
    case '+':
	printf("spying on %d",CodeToEnum(GetOpCode(pc)));
	add_spy(*pc);
	goto debugstart;
	break;
    case '-':
	printf("no spy on %d",CodeToEnum(GetOpCode(pc)));
	delete_spy(*pc);
	goto debugstart;
	break;
    case 'l':
	debugmode = D_LEAP;
	return 1;
	break;
    case 'd':
	switch(debug_str[1]) {
	case 'b':
	    printf("Current break points are:\n");
	    for(i = 0 ; i != breakc ; i++) {
		printf("pc: %#lx\t",breakpoints[i]);
		display_code(breakpoints[i]);
		putchar('\n');
	    }
	    break;
	case 's':
	    printf("Current spy points are:\n");
	    for(i = 0 ; i != spyc ; i++) {
		printf("Spying on %d",monitorpoints[i]);
		putchar('\n');
	    }
	    break;
	case 'a':
	    i = atoi(&(debug_str[2]));
	    if(chb == NULL) {
		fprintf(stdout, "no choicebox\n");
		break;
	    }
	    if(chb->cont == NULL) {
	      fprintf(stdout, "no choice continuation\n");
	      break;
	    }
	    if((i + 1) > chb->cont->arity) {
		fprintf(stdout, "a[%d] last a registe", (chb->cont->arity -1));
		break;
	    }
	    fprintf(stdout,"a[%d] = ",i);
	    if(chb->cont->arg[i] == NULL)
		fprintf(stdout,"NULL");
	    else
		display_term(chb->cont->arg[i],-1);
	    putchar('\n');
	    break;
	case 'x':
	    i = atoi(&(debug_str[2]));
	    fprintf(stdout,"x[%d] = ",i);
	    if(xreg[i] == NULL)
		fprintf(stdout,"NULL");
	    else
		display_term(xreg[i],-1);
	    putchar('\n');
	    break;
	case 'y':
	    i = atoi(&(debug_str[2]));
	    if(andb == NULL) {
		fprintf(stdout, "no andbox\n");
		break;
	    }
	    if(andb->cont == NULL) {
		fprintf(stdout, "no y registers\n");
		break;
	    }
	    if(andb->cont->yreg == NULL) {
		fprintf(stdout, "no y registers\n");
		break;
	    }
	    if((i + 1) >  andb->cont->ysize) {
		fprintf(stdout, "y[%d] last y register",(andb->cont->ysize - 1));
		break;
	    }
	    fprintf(stdout,"y[%d] = ",i);
	    if(andb->cont->yreg[i] == NULL)
		fprintf(stdout,"NULL");
	    else
		display_term(andb->cont->yreg[i],-1);
	    putchar('\n');
	    break;
	default:
	    fprintf(stdout,"can't print that\n");
	}
	goto debugstart;
    case 'n':
	fprintf(stdout,"Debugger turned off\n");
	debugflag = FALSE;
	return 1;
	break;
    case 'p':
	switch(debug_str[1]) {
	case 'y':
	    if(andb == NULL) {
		fprintf(stdout,"no andbox\n");
		break;
	    }
	    if(andb->cont == NULL) {
		fprintf(stdout,"no y registers\n");
		break;
	    }
	    fprintf(stdout,"The y register contains: \n");
	    size = andb->cont->ysize;
	    for(i = 0; i != size ; i++) {
		fprintf(stdout,"y[%d] = ",i);
		if(andb->cont->yreg[i] != NULL)
		    display_term(andb->cont->yreg[i],-1);
		else
		    fprintf(stdout,"NULL");
		putchar('\n');
	    }
	    break;
	case 'a':
	    if(chb == NULL){
		fprintf(stdout, "no choicebox\n");
		break;
	    }
	    if(chb->cont == NULL) {
		fprintf(stdout, "no choice continuation\n");
		break;
	    }	        
	    fprintf(stdout,"The a register contains: \n");
	    for(i = 0; i != chb->cont->arity ; i++) {
		fprintf(stdout,"a[%d] = ",i);
		if(chb->cont->arg[i] != NULL)
		    display_term(chb->cont->arg[i],-1);
		else
		    fprintf(stdout,"NULL");
		putchar('\n');
	    }
	    break;
	case 'x':
	    j = atoi(&(debug_str[2]));
	    fprintf(stdout,"The x register contains: \n");
	    if(j == 0)
		j =6;
	    for(i = 0; i != j ; i++) {
		fprintf(stdout,"x[%d] = ",i);
		if(xreg[i] != NULL)
		    display_term(xreg[i],-1);
		else
		    fprintf(stdout,"NULL");
		putchar('\n');
	    }
	    break;
	default:
	    fprintf(stdout,"Can't print that\n");
	    break;
	}
	goto debugstart;
    case 't':
	debugmode = D_TRACE;
	break;
    case '?':
	printf("+ \t-- spy on instruction\n");
	printf("- \t-- no spy on instruction\n");
	printf("a \t-- abort (AKL will exit)\n");
	printf("b \t-- breakpoint after this instruction\n");
	printf("c \t-- creep\n");
	printf("db\t-- display break points\n");
	printf("ds\t-- display spy points\n");
	printf("dx <Nr>\t-- display x register nr Nr\n");
	printf("dy <Nr>\t-- display y register nr Nr\n");
	printf("l \t-- leap\n");
	printf("n \t-- turn off debugger\n");
	printf("py \t-- print y register\n");
	printf("pa \t-- print a register\n");
	printf("px <Nr>\t-- print x register to reg.nr Nr\n");
	printf("q \t-- quit (AKL will exit)\n");
	printf("r \t-- remove breakpoint at this instruction\n");
	printf("s \t-- skip (break, leap)\n");
	printf("t \t-- trace (leap and print debug info.)"); putchar('\n');
	
	goto debugstart;
    case 'c':
	debugmode = D_CREEP;
	return 1;
    case '\n':
    case '\000':
	debugmode = D_CREEP;
	break;
    default:
	fprintf(stdout,"unknown command\n");
	goto debugstart;
    }
    return 1;
}

#endif


void initialize_debug() {

  define("pam_debug", akl_pam_debug, 0);
  define("pam_nodebug", akl_pam_nodebug, 0);
}
