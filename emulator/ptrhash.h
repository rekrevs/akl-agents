/* $Id: ptrhash.h,v 1.1 1993/08/26 15:08:11 bd Exp $ */

/* A simple hashing scheme that maps an uword to another uword.
 * The hash function is the key itself, right-shifted 4 bits to get rid
 * of low constant bits in pointers (e.g. tags or alignment zeros).
 * The table size is always a power of 2 to make modulus calculation cheap.
 * When a table becomes 75% full, it is automatically expanded.
 * The rehashing method used is linear rehashing, with an odd distance that
 * uses some more bits from the key.
 */


typedef struct {
  uword key;
  uword value;
} hash_entry;

typedef struct hash_table {
  unsigned long mask;
  long limit;
  hash_entry entry[ANY];
} hash_table;

#define sizeof_hash_table(n) \
	(sizeof(hash_table) + ((n)-ANY)*sizeof(hash_entry))

/* This calculates the 75% limit */
#define limitof_hash_table(n)	(((n) >> 1) + ((n) >> 2))

#define hash_table_length(h)	((h)->mask + 1)

/* Note: Don't use this value as a key! */
#define VACANT_HASH_ENTRY	0

#define HashValue(key) (((uword)(key)) >> 4)

#define HashLookup(Table, Key, Hval, EntryPtr, IfNotFound) \
{\
  register long ZZZi; \
  register unsigned long ZZZm = (Table)->mask; \
  ZZZi = ((Hval) & ZZZm); \
  do { \
    register uword ZZZk; \
    register unsigned long ZZZc; \
    (EntryPtr) = &(Table)->entry[ZZZi]; \
    ZZZk = (EntryPtr)->key; \
    if (ZZZk == (Key)) break; \
    if (ZZZk == VACANT_HASH_ENTRY) { \
      IfNotFound; \
    } \
    ZZZc = (((Hval) >> 3) | 0x1); \
    ZZZi = ((ZZZi + ZZZc) & ZZZm); \
  } while (1); \
}


/* Use HashEnter() when adding new entries.
 * If you want to change an existing value, just zap it.
 * But in both cases you must first do a HashLookup() to get the EntryPtr.
 */

#define HashEnter(Table, EntryPtr, Key, Value) \
{ \
  (EntryPtr)->key = (Key); \
  (EntryPtr)->value = (Value); \
  if (--(Table)->limit == 0) { \
    (Table) = grow_hash_table((Table)); \
  } \
}


/* Use these macros when the keys and values are terms. */

#define TermHashValue(Key)	HashValue(Tad(Key))

#define TermHashLookup(Table, Key, Hval, EntryPtr, IfNotFound) \
	HashLookup(Table, Tad(Key), Hval, EntryPtr, IfNotFound)

#define TermHashEnter(Table, EntryPtr, Key, Value) \
	HashEnter(Table, EntryPtr, Tad(Key), Tad(Value))

#define GetEntryTerm(EntryPtr)	tad_to_term((EntryPtr)->value)


extern hash_table *new_hash_table();
extern hash_table *grow_hash_table();
extern void clear_hash_table();
