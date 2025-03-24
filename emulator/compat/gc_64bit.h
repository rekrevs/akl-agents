/**
 * gc_64bit.h - Modern garbage collection implementation for 64-bit architecture
 *
 * This file provides the interface for garbage collection operations
 * that integrate with the 64-bit tagged pointer system.
 */

#ifndef GC_64BIT_H
#define GC_64BIT_H

#include <stdint.h>
#include <stdbool.h>
#include "emulator/compat/tagged_pointers_64bit.h"
#include "emulator/compat/storage_64bit.h"

/**
 * GC statistics structure for tracking performance metrics
 */
typedef struct gc_stats_64bit {
    uint64_t gc_cycles;           /* Total number of GC cycles run */
    uint64_t total_objects_marked; /* Total objects marked in last cycle */
    uint64_t total_bytes_freed;   /* Total bytes freed in last cycle */
    uint64_t avg_bytes_freed;     /* Average bytes freed per cycle */
    uint64_t total_gc_time_ms;    /* Total time spent in GC (milliseconds) */
    uint64_t avg_gc_time_ms;      /* Average time per GC cycle (milliseconds) */
} gc_stats_64bit_t;

/**
 * GC configuration structure
 */
typedef struct gc_config_64bit {
    bool     auto_gc_enabled;     /* Whether automatic GC is enabled */
    uint32_t heap_threshold;      /* Memory threshold to trigger GC (bytes) */
    bool     verbose_gc;          /* Whether to print detailed GC info */
} gc_config_64bit_t;

/**
 * Suspension entry for GC processing
 * These must be copied during garbage collection
 */
typedef struct gc_susp_entry_64bit {
    void*    suspension;          /* Pointer to suspension structure */
    void**   location;            /* Pointer to location containing the suspension */
} gc_susp_entry_64bit_t;

/**
 * Main GC state structure
 */
typedef struct gc_state_64bit {
    /* Suspension stack */
    gc_susp_entry_64bit_t* susp_stack;      /* Stack for suspension entries */
    gc_susp_entry_64bit_t* susp_stack_top;   /* Current top of suspension stack */
    gc_susp_entry_64bit_t* susp_stack_end;   /* End of suspension stack */
    gc_susp_entry_64bit_t* susp_stack_start; /* Start of suspension stack */
    
    /* Reference stack */
    Term**  ref_stack;            /* Stack for reference pointers */
    Term**  ref_stack_top;        /* Current top of reference stack */
    Term**  ref_stack_end;        /* End of reference stack */
    Term**  ref_stack_start;      /* Start of reference stack */
    
    /* Generic object stack */
    void**  generic_stack;        /* Stack for generic objects */
    void**  generic_stack_top;    /* Current top of generic stack */
    void**  generic_stack_end;    /* End of generic stack */
    
    /* Statistics */
    gc_stats_64bit_t stats;       /* GC statistics */
    
    /* Internal state */
    int     current_flip;         /* Current flip-flop marker */
    bool    gc_in_progress;       /* Whether GC is currently running */
} gc_state_64bit_t;

/* Global GC state */
extern gc_state_64bit_t gc_state_64bit;

/* Global GC configuration */
extern gc_config_64bit_t gc_config_64bit;

/**
 * Initialize the garbage collection system
 * 
 * @param stack_size Size of the reference and suspension stacks
 */
void init_gc_64bit(size_t stack_size);

/**
 * Run a full garbage collection cycle
 * 
 * @param exec_state Execution state to be garbage collected
 * @return true if GC was successful
 */
bool run_gc_64bit(void* exec_state);

/**
 * Mark a term as live (not garbage)
 * 
 * This recursively marks all objects referenced by this term.
 * 
 * @param term The term to mark
 * @param gc_state GC state structure
 */
void mark_term_64bit(Term term, gc_state_64bit_t* gc_state);

/**
 * Mark a variable as live (not garbage)
 * 
 * @param var_ptr Pointer to the variable to mark
 * @param gc_state GC state structure
 */
void mark_variable_64bit(Term* var_ptr, gc_state_64bit_t* gc_state);

/**
 * Reset the GC system after a collection
 */
void reset_gc_64bit(void);

/**
 * Configure the GC system
 * 
 * @param config New configuration
 */
void configure_gc_64bit(gc_config_64bit_t config);

/**
 * Get current GC statistics
 * 
 * @return Current GC statistics
 */
gc_stats_64bit_t get_gc_stats_64bit(void);

/**
 * Check if an object is marked in the current GC cycle
 * 
 * @param term The term to check
 * @return true if the term is marked
 */
bool is_marked_64bit(Term term);

/**
 * Expand the reference stack if needed
 */
void expand_ref_stack_64bit(void);

/**
 * Expand the suspension stack if needed
 */
void expand_susp_stack_64bit(void);

#endif /* GC_64BIT_H */
