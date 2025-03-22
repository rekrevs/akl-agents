/* $Id: hash.h,v 1.9 1994/04/14 13:43:27 boortz Exp $ */

/* ######################################################################## */
/* 			GENERAL HASH TABLE HANDLING 			    */
/* ######################################################################## */

/* Macros for general hashing by Kent Boortz. The actual hashing code was
 * written by Bjorn Danielsson.
 * You can choose hash function, but not rehash function. You put in
 * routines for comparing keys, allocating memory etc.
 * All routines are (strange) macros purely for efficiency.
 *
 * The table size is always a power of 2 to make modulus calculation cheap.
 * When a table becomes 75% full, it is automatically expanded.
 * The rehashing method used is linear rehashing, with an odd distance that
 * uses some more bits from the key.
 *
 * A deleted entry is only marked as deleted. It may be reused but in the
 * worst case the table could be owerflowed even if there are a lot of
 * deleted entrys.
 */

/* ########################################################################
 *
 *			USING THIS MACRO PACKAGE
 *
 * *First* declare the types 'HashKey' and 'HashValue'.
 * *Then* include this file. An example:
 *
 *	typedef struct {char *name; int arity;} HashKey;
 *	typedef definition *HashValue;
 *
 *	#include "hash.h"
 *
 * You define some routines, preferably as macros. These will be part
 * of the arguments to the macros in this package.
 *
 * ALLOCATE
 * --------
 * The first is for allocating memory. The caller of this routine
 * has defined two varaibles:
 *
 *	unsigned	ht_size;
 *	char		*ht_memory;
 *
 * You should allocate 'ht_size' bytes and let 'ht_memory' point
 * to this block. Example:
 *
 *	{ht_memory = malloc(ht_size)}
 *
 * COMPARE KEYS
 * ------------
 * The second one is used for compare two keys. The hash package know
 * nothing about what the keys are. Variables defined by the caller are:
 *
 *	HashKey		ht_key1, ht_key2;
 *	int 		ht_result;
 *
 * The comparision should set 'ht_result' to 0 if the keys are not equal
 * and > 0 if they are equal. Example:
 *
 *	{ht_result = ((strcmp(ht_key1.name,ht_key2.name) == 0)
 *		   && (ht_key1.arity == ht_key2.arity));}
 *
 * HASH VALUE
 * ----------
 * The third routine calculate the hash value given a key. The
 * predefined variables are:
 *
 *	unsigned long	ht_hash;
 *	HashKey		ht_key;
 *
 * 'ht_hash' is the hash value of 'ht_key'. An example:
 *
 *	{
 *	  int len = strlen(ht_key.name);
 *	  ht_hash = len + ht_key.name[0] + ht_key.name[len - 1] + ht_key.arity;
 *	}
 *
 * DEALLOCATE
 * ----------
 * The deallocate routine has the same predefined variables as the
 * allocation routine:
 *
 *	unsigned	ht_size;
 *	char		*ht_memory;
 *
 * An example:
 *
 *	{free(ht_memory);}
 *
 * CLEAR EACH ENTRY
 * ----------------
 * Deallocate memory you allocated for an elements key and value.
 * The predefined variable is:
 *
 *	HashEntry	*ht_entry;
 *
 * COPY EACH ENTRY
 * ---------------
 * If you use the copy macro you should give a routine for
 * copying each element. The predefined variables are:
 *
 *	HashEntry	*ht_entry;
 *	HashEntry	*ht_newentry;
 *
 * An example:
 *
 *	{*ht_newentry = *ht_entry;}
 *
 * ######################################################################## */

#define VACANT_HASH_ENTRY	0
#define OCCUPIED_HASH_ENTRY	1
#define DELETED_HASH_ENTRY	2

/* ########################################################################
 *
 * NewHashTable(
 *		HashTable *Table,
 *		long Size,
 *		ProcAllocate)
 *
 * Create new hash table. The arguments are:
 *
 *	Table		Return value.
 *	Size		Max number of elements in table.
 *	ProcAllocate	Procedure to allocate memory fo table.
 *			You allocate 'size' bytes and set
 *			'memory' to this memory block. If malloc()
 *			was to be used ProcAllocate could be defined:
 *
 *				{memory = malloc(size);}
 *
 *			Remeber to set 'memory' to NULL if the memory
 *			couldn't be allocated.
 *
 * ------------------------------------------------------------------------ */

#define NewHashTable(Table,Size,ProcAllocate) \
{ \
  unsigned long __real_size = 0x4; \
\
  if (Size & ~((unsigned long)0xff)) /* Always start at 256 or less */ \
    __real_size = 0x100; \
\
  while (__real_size < Size)	/* Find power of 2 that fit */ \
    __real_size = (__real_size << 1); \
\
  AllocateHashTable(Table,__real_size,ProcAllocate) \
}

/* ########################################################################
 *
 * AllocateHashTable(				(INTERNAL: DO NOT USE)
 *		HashTable *Table,
 *		long Size,
 *		ProcAllocate)
 *
 * Assumes that 'Size' is a power of 2. Sets '(Table)->size'.
 * Initiates the elements in the table.
 * ------------------------------------------------------------------------ */

#define AllocateHashTable(Table,Size,ProcAllocate) \
{ \
  {\
    char *ht_memory; \
    unsigned ht_size = SizeOfHashTable(Size); \
    ProcAllocate; \
    (Table) = (HashTable *)ht_memory; \
  } \
\
  if ((Table) != NULL) \
    { \
      int n, i; \
      n = (Table)->size = Size; \
      (Table)->limit = (n >> 1) + (n >> 2); /* Set limit at 75% full */ \
      for (i = 0; i < n; i++) \
        (Table)->entry[i].status = VACANT_HASH_ENTRY; \
    } \
}

/* ########################################################################
 *
 * ClearHashTable(
 *		HashTable *Table,
 *		ProcClearEntry)
 *
 * Clear the hash table. The arguments are:
 *
 *	Table		Table to clear
 *	ProcClearEntry	What to do with entry 'entry' in
 *			table. 'HashEntry' is defined as:
 *
 * The entry key position will be cleared after this call. Sets 'limit'.
 * ------------------------------------------------------------------------ */


#define ClearHashTable(Table,ProcClearEntry) \
{ \
  long i, n = (Table)->size; \
\
  (Table)->limit = (n >> 1) + (n >> 2); /* Set limit at 75% full */ \
\
  for (i = 0; i < n; i++) \
    { \
      HashEntry *ht_entry = &(Table)->entry[i]; \
      if (ht_entry->status == OCCUPIED_HASH_ENTRY) \
        { \
	  ProcClearEntry; \
        } \
      ht_entry->status = VACANT_HASH_ENTRY; \
    }; \
}

/* ########################################################################
 *
 * CopyHashTable(
 *		HashTable NewTable,
 *		HashTable *Table,
 *		ProcAllocate,
 *		ProcCopyEntry)
 *
 * Allocate a new table and copy the elements in the hash table.
 * The arguments are:
 *
 *	NewTable	The new copy of the table.
 *	Table		Table to copy.
 *	ProcAllocate	Procedure to allocate memory fo table.
 *			See 'NewHashTable' above.
 *	ProcCopyEntry	Copy the old entry, 'entry' to the new
 *			entry 'newentry'.
 *			To do a simple element copy you write:
 *
 *				{*newentry = *entry;}
 *
 *			If you don't copy the entry will be cleared.
 * ------------------------------------------------------------------------ */

#define CopyHashTable(NewTable,Table,ProcAllocate,ProcCopyEntry) \
{ \
  int __size = (Table)->size; \
\
  AllocateHashTable((NewTable),__size,ProcAllocate);\
\
  if ((NewTable) != NULL) \
    { \
      int i; \
      (NewTable)->limit = (Table)->limit; \
      for (i = 0; i < __size; i++) \
	{ \
	  HashEntry *ht_entry = &(Table)->entry[i]; \
	  HashEntry *ht_newentry = &(NewTable)->entry[i]; \
	  ht_newentry->status =  ht_entry->status; \
\
	  if (ht_entry->status == OCCUPIED_HASH_ENTRY) \
	    { \
	      ProcCopyEntry; \
	    } \
	} \
    } \
}

/* ########################################################################
 *
 * GrowHashTable(
 *		HashTable NewTable,
 *		HashTable *Table,
 *		ProcHashFunc,
 *		ProcKeyComp,
 *		ProcAllocate,
 *		ProcFree,
 *		ProcCopyEntry)
 *
 * Allocate a new table twice as big as the old one and hash the
 * elements into the new hash table. The arguments are:
 *
 *	NewTable	The new copy of the table.
 *	Table		Table to copy.
 *	ProcHashFunc	Calculate the hash value into 'hash'
 *	ProcKeyComp	See 'HashLookupLoop' below.
 *	ProcAllocate	See 'NewHashTable' above.
 *	ProcFree	Free the memory pointed to by 'memory'.
 *			The block is 'size' bytes.
 * ------------------------------------------------------------------------ */

#define GrowHashTable(NewTable,Table,ProcHashFunc,ProcKeyComp,ProcAlloc,ProcFree) \
{ \
  AllocateHashTable((NewTable),2 * (Table)->size, ProcAlloc); \
\
  if ((NewTable) != NULL) \
    { \
      long __newlim = (NewTable)->limit; /* 75% of new table */ \
      long i, n = (Table)->size; \
\
      HashEntry *__entry = &(Table)->entry[0]; \
\
      for (i = 0; i < n; i++) \
	{ \
          if (__entry->status == OCCUPIED_HASH_ENTRY) \
            { \
	      HashKey ht_key = __entry->key; \
	      unsigned long ht_hash; \
	      HashEntry *__newentry; \
	      ProcHashFunc; \
	      HashLookupLoop((NewTable),ht_key,ht_hash,__newentry,ProcKeyComp,{},{}); \
	      *__newentry = *__entry; \
	      __newlim -= 1;		/* one less before another grow */ \
            } \
          __entry += 1;			/* next entry */ \
        } \
\
      (NewTable)->limit = __newlim; \
      { \
	char *ht_memory = (char *)(Table); \
	unsigned ht_size = SizeOfHashTable((Table)->size); \
        ProcFree; \
      } \
    } \
}

/* ########################################################################
 *
 * HashLookupLoop(					(LOW LEVEL)
 *		HashTable *Table,
 *		HashKey Key,
 *		HashValue Hval,
 *		HashEntry *Entry,
 *		ProcKeyComp,
 *		IfFound,
 *		IfNotFound)
 *
 * Lookup entry in hash table. This is the low level lookup routine that
 * only leave you a pointer to an entry. Read about the arguments below
 * carefully, missuse may damage your hash table. The arguments are:
 *
 *	Table		Table to copy.
 *	Key		'HashKey' key.
 *	Hval		Hash value for this key.
 *	Value		Value for this key.
 *	Entry		Returned pointer to key-value pair,
 *			 see 'CopyHashTable'.
 *	ProcKeyComp	Should compare a variable 'key' with
 *			the supplied 'Key' and set the a variable
 *			'result' to 0 if not equal and greater
 *			than 0 if equal.
 *	IfFound		Executed if entry found.
 *			A pointer to the entry is returned.
 *			You may change the value.
 *	IfNotFound	Executed if entry was not found.
 *			A pointer to a new entry is returned.
 *			You may assign Entry->key to Key and Entry->value
 *			to the value. If the old 'status' was not 
 *			DELETED_HASH_ENTRY you have to Decrement 
 *			Table->limit. If the new limit is zero you have 
 *			to call 'GrowHashTable'.
 * ------------------------------------------------------------------------ */

#define NO_EMPTY -1

#define HashLookupLoop(Table, Key, Hval, Entry, ProcKeyComp, IfFound, IfNotFound) \
{\
  unsigned long __mask = HashMask(Table); \
  long __index = ((Hval) & __mask); \
  long __last_empty = NO_EMPTY; \
\
  do { \
    int ht_result; \
    register unsigned long __c; \
    unsigned char __status; \
\
    (Entry) = &(Table)->entry[__index]; \
    __status = (Entry)->status; \
\
    if (__status == DELETED_HASH_ENTRY) \
      __last_empty = __index; \
    else \
      if (__status == VACANT_HASH_ENTRY) \
	{ \
	  if (__last_empty != NO_EMPTY) \
	    (Entry) = &(Table)->entry[__last_empty]; \
\
          IfNotFound; \
	  break; \
	} \
\
    { \
      HashKey ht_key1 = Key; \
      HashKey ht_key2 = (Entry)->key; \
      ProcKeyComp; \
    } \
\
    if (ht_result) \
      { \
        IfFound; \
	break; \
      } \
\
    __c = (((Hval) >> 3) | 0x1); \
    __index = ((__index + __c) & __mask); \
  } while (1); \
}

/* ########################################################################
 *
 * HashFind(
 *		HashTable *Table,
 *		HashKey Key,
 *		HashValue Value,
 *		ProcKeyComp,
 *		IfNotFound)
 *
 * Lookup entry in hash table. If not found, don't insert.
 * The arguments are:
 *
 *	Table		Table to copy.
 *	Key		Hash key. See note at 'HashLookupLoop'.
 *	Value		Value for this key.
 *	ProcHashFunc	Calculate hash value from Key and store in 'hash'
 *	ProcKeyComp	Should compare a variable 'key' with
 *			the supplied 'Key' and set the a variable
 *			'result' to 0 or 1.
 *	IfNotFound	Executed if entry wasn't found.
 *			'Value' will contain garbage.
 * ------------------------------------------------------------------------ */

#define HashFind(Table, Key, Val, ProcHashFunc, ProcKeyComp, IfNotFound) \
{ \
  HashEntry * __entry; \
  unsigned long ht_hash; \
  { \
    HashKey ht_key = (Key); \
    ProcHashFunc; \
  } \
  HashLookupLoop(Table, Key, ht_hash, __entry, ProcKeyComp, {}, IfNotFound); \
  (Val) = __entry->value; \
}

/* ########################################################################
 *
 * HashEnter(
 *		HashTable *Table,
 *		HashKey Key,
 *		ProcHashFunc,
 *		ProcKeyComp,
 *		ProcAllocate,
 *		ProcFree)
 *
 * Will try to find the Key in Table. If found it will overwrite
 * overwrite the value with the new value. If not found it
 * will insert the new (Key,Value) pair into the table.
 * If the table become more than 75% full it will automaticly
 * expand.
 *
 * The arguments are:
 *
 *	Table		Table to copy.
 *	Key		Hash key. See note at 'HashLookupLoop'.
 *	Value		Value for this key.
 *	ProcHashFunc	Calculate hash value from Key and store in 'hash'
 *	ProcKeyComp	Should compare a variable 'key' with
 *			the supplied 'Key' and set the a variable
 *			'result' to 0 or 1.
 *	ProcAllocate	See 'NewHashTable' above.
 *	ProcFree	Free the memory pointed to by 'memory'.
 *			The block is 'size' bytes.
 * ------------------------------------------------------------------------ */

#define HashEnter(Table,Key,Val,ProcHashFunc,ProcKeyComp,ProcAllocate,ProcFree) \
{ \
  HashEntry * __entry; \
  unsigned long ht_hash; \
  { \
    HashKey ht_key = (Key); \
    ProcHashFunc; \
  } \
  HashLookupLoop((Table), (Key), ht_hash, __entry, ProcKeyComp, \
    { \
      __entry->value = (Val); /* Overwrite old value */ \
    }, \
    { \
      __entry->key = (Key); /* If not found, enter key */ \
      __entry->value = (Val); /* Overwrite old value */ \
      __entry->status = OCCUPIED_HASH_ENTRY; \
\
      if (--((Table)->limit) == 0) \
	{ \
	  HashTable *new; \
	  GrowHashTable(new,(Table),ProcHashFunc,ProcKeyComp,ProcAllocate,ProcFree); \
	  (Table) = new; \
	} \
    }) \
}


/* ########################################################################
 *
 * HashEntryLoop(
 *		HashTable *Table,
 *		ProcEntry)
 *
 * Will iterate over all entrys, i.e. (Key,Value) pairs, currently 
 * in the table.
 *
 * The arguments are:
 *
 *	Table		Table to copy.
 *	ProcEntry	Do whatever you like with 'entry'.
 * ------------------------------------------------------------------------ */

#define HashEntryLoop(Table,ProcEntry) \
{ \
  long __n = (Table)->size; \
  do { \
    HashEntry *ht_entry = &(Table)->entry[--__n]; \
\
    if (ht_entry->status == OCCUPIED_HASH_ENTRY) \
      { \
	ProcEntry; \
      } \
  } while (__n != 0); \
}

/* ######################################################################## */

/* Information about the hash table */

#define SizeOfHashTable(n) \
	(sizeof(HashTable) + ((n)-ANY)*sizeof(HashEntry))

#define HashMask(Table) ((Table)->size - 1)


#ifndef ANY
 #define ANY 1
#endif


/* ######################################################################## */

/* Data unique to each table */

typedef struct {
  unsigned char status;
  HashKey key;
  HashValue value;
} HashEntry;

typedef struct {
  unsigned long size;
  unsigned long limit;
  HashEntry entry[ANY];
} HashTable;
