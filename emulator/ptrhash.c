/* $Id: ptrhash.c,v 1.1 1993/08/26 15:08:08 bd Exp $ */

#include "include.h"
#include "term.h"
#include "debug.h"
#include "ptrhash.h"
#include "error.h"

void clear_hash_table(table)
     hash_table *table;
{
  long n = hash_table_length(table);

  table->limit = limitof_hash_table(n);	/* Set limit at 75% full */

  do {
    n -= 1;
    table->entry[n].key = VACANT_HASH_ENTRY;
  } while (n != 0);
}

hash_table *new_hash_table(n)
     long n;
{
  hash_table *table;
  uword s = 0x4;
  if (n & ~((uword)0xff)) s = 0x100;
  while (s < n) s = (s << 1);

  /* s is now guaranteed to be a large enough power of 2 */

  table = (hash_table *) malloc(sizeof_hash_table(s));
  if (table == NULL) return NULL;
  table->mask = s-1;			/* For calculation of modulus s */

  clear_hash_table(table);

  return table;
}

hash_table *grow_hash_table(table)
     hash_table *table;
{
  hash_table *newtbl;
  hash_entry *e = &table->entry[0];
  long newlimit, n = hash_table_length(table);

  /* Make a new table, twice as big as the old */

  newtbl = new_hash_table(2*n);
  if (newtbl == NULL) {
    FatalError("Couldn't grow hash table");
  }

  newlimit = newtbl->limit;

  /* Traverse the old table to copy all entries */

  do {
    uword key = e->key;
    if (key != VACANT_HASH_ENTRY) {
      hash_entry *enew;
      unsigned long hval = HashValue(key);
      HashLookup(newtbl, key, hval, enew, break);
      enew->key = key;
      enew->value = e->value;
      newlimit -= 1;
    }
    e += 1;
    n -= 1;
  } while (n != 0);

  newtbl->limit = newlimit;
  free(table);
  return newtbl;
}
