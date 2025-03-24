/**
 * storage_64bit.h - Modern 64-bit implementation of AKL's memory management
 *
 * This file provides a clean 64-bit implementation of the memory management system used in AKL.
 * It ensures proper alignment and integration with the 64-bit tagged pointer system.
 */

#ifndef STORAGE_64BIT_H
#define STORAGE_64BIT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "emulator/compat/tagged_pointers_64bit.h"
#include "emulator/compat/structure_compat.h"

/**
 * Memory block structure - represents a segment of memory
 */
typedef struct memory_block {
    void*               start;          /* Start address of the memory block */
    void*               end;            /* End address of the memory block */
    void*               logical_end;    /* Current logical end (next allocation point) */
    struct memory_block* next;          /* Pointer to the next block in chain */
    int                 size;           /* Size of the block in bytes */
} memory_block_t;

/* Global memory management variables */
extern memory_block_t*  global_block_list;      /* Global list of all memory blocks */
extern memory_block_t*  current_memory_block;   /* Current block for allocations */
extern memory_block_t*  unused_block_list;      /* List of unused memory blocks */
extern memory_block_t*  used_block_list;        /* List of used memory blocks */

extern void*            global_heap_end;        /* End of the current heap block */
extern void*            global_heap_current;    /* Current allocation point in heap */

extern bool             gc_flag;                /* Flag indicating if GC is needed */
extern int              block_limit;            /* Maximum number of blocks before GC */
extern int              active_blocks;          /* Current number of active blocks */
extern size_t           memory_block_size;      /* Size of each memory block */

/**
 * Special alignment macros for 64-bit architecture
 * 
 * On 64-bit systems, we need to ensure 8-byte alignment for all pointers
 * that will be tagged. This ensures the proper bits are available for tagging.
 */
#define ALIGNMENT_64BIT 8
#define ALIGN_64BIT(size) (((size) + (ALIGNMENT_64BIT-1)) & ~((size_t)(ALIGNMENT_64BIT-1)))

/**
 * Verify that a heap address is properly aligned
 */
#ifdef DEBUG
  #define VERIFY_HEAP_ALIGNMENT(ptr) \
    assert(("Heap pointer not properly aligned for 64-bit tagging", \
           (((uintptr_t)(ptr)) & (ALIGNMENT_64BIT-1)) == 0))
#else
  #define VERIFY_HEAP_ALIGNMENT(ptr) ((void)0)
#endif

/**
 * Memory allocation primitives
 */

/* Allocate a new memory block of specified size */
memory_block_t* allocate_memory_block(size_t size);

/* Initialize the heap with specified size and block limit */
void init_heap_64bit(size_t size, int limit);

/* Reinitialize the heap when current block is exhausted */
void reinit_heap_64bit(void* ptr, size_t size_needed);

/* Restore a previously saved memory block */
void restore_memory_block_64bit(memory_block_t* saved_block);

/* Begin garbage collection process */
void gc_begin_64bit(void);

/* End garbage collection process */
void gc_end_64bit(void);

/* Get the start address of the heap */
void* heap_start_64bit(void);

/* Get the total size of the heap */
size_t heap_size_64bit(void);

/* Initialize the memory allocation system */
void init_memory_system_64bit(void);

/* Initialize the constant space */
void init_const_space_64bit(size_t size);

/**
 * Type-specific allocation macros optimized for 64-bit
 * 
 * These macros replace the original allocation macros but ensure proper 
 * alignment and integration with the 64-bit tagged pointer system.
 */

/* Helper macros for heap allocation control */
#define HEAP_CURRENT    global_heap_current
#define HEAP_END        global_heap_end
#define SAVE_HEAP_REGISTERS()
#define RESTORE_HEAP_REGISTERS()

#define SAVE_HEAP(B,H) {             \
    (H) = HEAP_CURRENT;              \
    (B) = current_memory_block;      \
}

#define RESTORE_HEAP(B,H) {          \
    if (current_memory_block != (B)) {          \
        restore_memory_block_64bit(B);          \
        HEAP_END = current_memory_block->end;   \
    }                                           \
    HEAP_CURRENT = (H);                         \
}

/* Standard memory allocation size */
#define SMALL_SIZE ((size_t)0)

/* Generic allocation macro for any type */
#define NEW_64BIT(ret, type) {                         \
    size_t aligned_size = ALIGN_64BIT(sizeof(type));   \
    (ret) = (type*)HEAP_CURRENT;                       \
    HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size); \
    if (HEAP_CURRENT >= HEAP_END) {                    \
        SAVE_HEAP_REGISTERS();                         \
        reinit_heap_64bit((ret), aligned_size);        \
        RESTORE_HEAP_REGISTERS();                      \
        (ret) = (type*)HEAP_CURRENT;                   \
        HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size); \
    }                                                  \
}

/* Allocation with array size */
#define NEWX_64BIT(ret, type, size) {                                    \
    size_t aligned_size = ALIGN_64BIT(sizeof(type) * (size));            \
    /* Verify current heap pointer is aligned */                          \
    VERIFY_HEAP_ALIGNMENT(HEAP_CURRENT);                                 \
    (ret) = (type*)HEAP_CURRENT;                                         \
    HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size);      \
    if (HEAP_CURRENT >= HEAP_END) {                                      \
        SAVE_HEAP_REGISTERS();                                           \
        reinit_heap_64bit((ret), aligned_size);                          \
        RESTORE_HEAP_REGISTERS();                                        \
        /* Verify reallocated heap pointer is aligned */                   \
        VERIFY_HEAP_ALIGNMENT(HEAP_CURRENT);                              \
        (ret) = (type*)HEAP_CURRENT;                                     \
        HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size);  \
    }                                                                    \
    /* Verify result pointer is aligned */                                \
    VERIFY_HEAP_ALIGNMENT(ret);                                          \
}

/* Allocate a new reference (Term) with proper alignment */
#define NewReference_64bit(ret) {                              \
    size_t aligned_size = ALIGN_64BIT(sizeof(Term));           \
    (ret) = (Term*)HEAP_CURRENT;                               \
    HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size); \
    if (HEAP_CURRENT >= HEAP_END) {                            \
        SAVE_HEAP_REGISTERS();                                 \
        reinit_heap_64bit((ret), SMALL_SIZE);                  \
        RESTORE_HEAP_REGISTERS();                              \
        (ret) = (Term*)HEAP_CURRENT;                           \
        HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size); \
    }                                                          \
}

/* Allocate a new structure with proper alignment */
#define NewStructure_64bit(ret, arity) {                                \
    size_t structure_size = sizeof(struct structure) + ((arity) * sizeof(Term)); \
    size_t aligned_size = ALIGN_64BIT(structure_size);                  \
    (ret) = (struct structure*)((uintptr_t)HEAP_CURRENT);               \
    HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size);     \
    if (HEAP_CURRENT >= HEAP_END) {                                     \
        SAVE_HEAP_REGISTERS();                                          \
        reinit_heap_64bit((ret), aligned_size);                         \
        RESTORE_HEAP_REGISTERS();                                       \
        (ret) = (struct structure*)((uintptr_t)HEAP_CURRENT);           \
        HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size); \
    }                                                                   \
}

/* Allocate a new list cell with proper alignment */
#define NewList_64bit(ret) {                                               \
    size_t aligned_size = ALIGN_64BIT(sizeof(struct list));                \
    (ret) = (struct list*)HEAP_CURRENT;                                    \
    HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size);        \
    if (HEAP_CURRENT >= HEAP_END) {                                        \
        SAVE_HEAP_REGISTERS();                                             \
        reinit_heap_64bit((ret), aligned_size);                            \
        RESTORE_HEAP_REGISTERS();                                          \
        (ret) = (struct list*)HEAP_CURRENT;                                \
        HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size);    \
    }                                                                      \
}

/* Allocate a float with proper alignment (doubles need 8-byte alignment anyway) */
#define NewFloat_64bit(ret) {                                           \
    /* Ensure proper alignment for floating point values */             \
    uintptr_t current = (uintptr_t)HEAP_CURRENT;                        \
    if (current % sizeof(double) != 0) {                                \
        current += sizeof(double) - (current % sizeof(double));         \
        HEAP_CURRENT = (void*)current;                                  \
    }                                                                   \
    size_t aligned_size = ALIGN_64BIT(sizeof(double));                  \
    (ret) = (double*)HEAP_CURRENT;                                      \
    HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size);     \
    if (HEAP_CURRENT >= HEAP_END) {                                     \
        SAVE_HEAP_REGISTERS();                                          \
        reinit_heap_64bit((ret), aligned_size);                         \
        RESTORE_HEAP_REGISTERS();                                       \
        /* Re-align if necessary after heap reinitialization */         \
        current = (uintptr_t)HEAP_CURRENT;                              \
        if (current % sizeof(double) != 0) {                            \
            current += sizeof(double) - (current % sizeof(double));     \
            HEAP_CURRENT = (void*)current;                              \
        }                                                               \
        (ret) = (double*)HEAP_CURRENT;                                  \
        HEAP_CURRENT = (void*)((uintptr_t)HEAP_CURRENT + aligned_size); \
    }                                                                   \
}

#endif /* STORAGE_64BIT_H */
