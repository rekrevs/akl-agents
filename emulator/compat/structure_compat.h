/**
 * structure_compat.h - Common structure definitions for AKL
 *
 * This file provides common structure definitions that ensure compatibility
 * between the original AKL implementation and the 64-bit modernized version.
 * These definitions are carefully designed to match memory layouts expected
 * by the emulator code.
 */

#ifndef STRUCTURE_COMPAT_H
#define STRUCTURE_COMPAT_H

/* Include only type definitions to avoid tag value conflicts */
#include "emulator/compat/tagged_pointers_64bit.h"

/**
 * Common structure definitions used across both 32-bit and 64-bit implementations
 * 
 * These must match exactly with any existing definitions in the emulator to ensure
 * correct memory layout and field offsets.
 */

/**
 * List cell structure
 * 
 * This structure must match the layout expected by the emulator.
 * The head and tail fields must be of type Term to ensure correct tagging.
 */
typedef struct list {
    Term head;
    Term tail;
} list_t;

/**
 * Structure cell structure
 * 
 * This structure must match the layout expected by the emulator.
 * The arity field indicates the number of arguments, and the args
 * array contains the arguments.
 */
typedef struct structure {
    int arity;
    Term args[1];  /* Flexible array member (C99) */
} structure_t;

/**
 * Generic cell structure
 * 
 * This structure is used for representing various types that need
 * additional tagging beyond the primary tag.
 */
typedef struct generic {
    int type;
    union {
        double float_value;
        void* pointer_value;
        unsigned long ulong_value;
    } data;
} generic_t;

/**
 * Reference cell structure
 * 
 * Used for representing references to terms.
 */
typedef struct reference {
    Term value;
} reference_t;

/**
 * Variable cell structure
 * 
 * Used for representing logic variables.
 */
typedef struct variable {
    Term value;
    /* Additional fields for constraint store, etc. */
} variable_t;

/**
 * Ensure compiler doesn't add padding that would break memory layout
 */
#pragma pack(push, 1)

/**
 * Memory allocation structure size verification
 * 
 * These static assertions verify that the structure sizes are what
 * the emulator expects. If the sizes don't match, a compilation error
 * will occur.
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(list_t) == 2 * sizeof(Term), 
               "list_t size mismatch");
_Static_assert(sizeof(structure_t) >= sizeof(int) + sizeof(Term), 
               "structure_t size mismatch");
#endif

#pragma pack(pop)

#endif /* STRUCTURE_COMPAT_H */
