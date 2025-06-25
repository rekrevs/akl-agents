/* $Id: code.c,v 1.20 1994/03/22 13:29:28 jm Exp $ */

#include "include.h"
#include "code.h"
#include "instructions.h"
#include "term.h"
#include "tree.h"
#include "predicate.h"
#include "exstate.h"
#include "initial.h"
#include "engine.h"
#include "error.h"
#include "bam.h"

int	codesize;

code	*codespace;
code	*codecurrent;
code	*codeend;
code	*codebase;

/* labelt[i] contains the definition of the label i. The address is stored
 * as an offset from codebase.
 */
code	labelt[MAXLABELS];

/* labeltag[i] contains the tag type for references to label i (all references
 * must have the same tag). This is used for pseudo terms in BAM code.
 */
char	labeltag[MAXLABELS];

/* labelrefs[i] contains a chain of pointers to all the places where label i
 * is referenced. The chain is terminated with NOLABEL. The pointers are
 * stored as offsets from codebase, which makes it fairly easy to relocate
 * a code block, if one would want to.
 */
code	labelrefs[MAXLABELS];

/* abst[i] is a table of abstraction definitions. The definition of an
 * abstraction must preceed the use. This allows the compiler to start
 * the coding of each predicate definition with a abstraction label set
 * to 0. No error checking is performed.
 */

predicate *abst[MAXLABELS];

void store_label(label)
     int label;
{
  if(labelt[label] != NOLABEL) {
    Error("multiple defined labels, using first");
    return;
  }
  labelt[label] = (char*)codecurrent - (char*)codebase;
}


void reset_label()
{
  int i;
  codebase = codecurrent;
  for (i=0; i<MAXLABELS; i+=1) {
    labelt[i] = NOLABEL;
    labeltag[i] = LABELREF_NULLTAG;
    labelrefs[i] = NOLABEL;
  }
}


void fix_labelrefs()
{
  int i;
  code ref, def;

  for (i=0; i<MAXLABELS; i+=1) {
    ref = labelrefs[i];
    if (ref != NOLABEL) {
      def = labelt[i];
      if (def == NOLABEL) {
	if (i == 0) {
	  /* [BD] Kludge for code that references global label "fail_dec" */
	  def = (code)fail_dec;
	} else {
	  Error("undefined label");
	}
      } else {
        def = (code)(def + (char*)codebase);
      }
      do {
	code *p = (code*)(ref + (char*)codebase);
	ref = *p;
	switch (labeltag[i]) {
	case LABELREF_NULLTAG:
	  *p = def;
	  break;
#ifdef BAM
	case LABELREF_TAG_LST:
	  *p = term_to_code(TagPseudoLST(def));
	  break;
	case LABELREF_TAG_STR:
	  *p = term_to_code(TagPseudoSTR(def));
	  break;
	case LABELREF_TAG_LBL:
	  *p = term_to_code(TagPseudoLABELED(def));
	  break;
#endif
	}
      } while (ref != NOLABEL);
    }
  }
}



/* Sort a switch table consisting of key-label pairs.
 * The pc (actually codecurrent) is expected to be positioned immediately
 * after the last pair.
 */

void sort_switch_table(tablesize)
     int tablesize;
{
  int i;
  u32 *tableptr = (u32*)codecurrent - 2*tablesize;
  int done = 0;
  int passes = 0;
  int end = 2*(tablesize-1);

  /* Bubble sort is not really satisfactory. Do something better! */

  while (!done) {
    done = 1;
    for (i=0; i<end; i+=2) {
      Term key1 = tad_to_term(tableptr[i]);
      Term key2 = tad_to_term(tableptr[i+2]);
      if (CompareConstants(key1, key2) > 0) {
	u32 temp1 = tableptr[i];
	u32 temp2 = tableptr[i+1];
	tableptr[i] = tableptr[i+2];
	tableptr[i+1] = tableptr[i+3];
	tableptr[i+2] = temp1;
	tableptr[i+3] = temp2;
	done = 0;
      }
    }
    passes += 1;
  }

  for (i=end+1; i>0; i-=2) {
    u32 temp = tableptr[i];
    if (temp >= MAXLABELS) {
      label_overflow();
    }
    tableptr[i] = labelrefs[temp];
    labelrefs[temp] = (char*)&tableptr[i] - (char*)codebase;
  }
}


void init_codespace(size)
    int size;
{
  codesize = size;
  codespace = (code *) malloc(sizeof(code)*size);
  if(codespace == NULL)
    FatalError("couldn't allocate code area");
  codeend = codespace + size;
  codecurrent = codespace;
}



void init_code()
{
  char *cp;
  int cdsize;
  init_engine();
  if ((cp = getenv("AKLCODESIZE"))) cdsize = atoi(cp); else cdsize = AKLCODESIZE;
  init_codespace(cdsize);
}


int index_overflow()
{
  char buf[256];
  sprintf(buf, "Index overflow -- can't have more than %d X-registers",
	  MAX_AREGS);
  FatalError(buf);
  return 0;
}

int label_overflow()
{
  char buf[256];
  sprintf(buf, "Label overflow -- can't have more than %d labels",
	  MAXLABELS);
  FatalError(buf);
  return 0;
}


void store_abstraction(number,def)
    int number;
    predicate *def;
{
    abst[number] = def;
}


predicate *get_abstraction(number)
    int number;
{
  return abst[number];
}
