/* $Id: inout.h,v 1.12 1994/04/07 13:40:35 bd Exp $ */


struct ptrdict {
  long			count;
  struct hash_table	*table;
};

/* This must be a power of 2 */
#define DICT_TABLE_SIZE	16

void reset_dict();
bool push_dict();
bool pop_dict();
long lookup_dict();

void initialize_inout();
