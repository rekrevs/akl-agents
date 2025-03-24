/**
 * gc_64bit.c - Modern garbage collection implementation for 64-bit architecture
 *
 * This file provides the implementation of garbage collection operations
 * that integrate with the 64-bit tagged pointer system.
 */

#include "emulator/compat/gc_64bit.h"
#include "emulator/compat/structure_compat.h"
#include "emulator/compat/debug_checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Avoid time.h conflicts by defining our own time types and functions */
typedef long akl_clock_t;
#define AKL_CLOCKS_PER_SEC 1000000

/* Dummy implementation of clock function to avoid time.h conflicts */
akl_clock_t akl_clock(void) {
    return 0;
}

/* Using structure definitions from structure_compat.h */

/* Global GC state */
gc_state_64bit_t gc_state_64bit = {0};

/* Global GC configuration */
gc_config_64bit_t gc_config_64bit = {
    .auto_gc_enabled = true,
    .heap_threshold = 1024 * 1024, /* 1MB */
    .verbose_gc = false
};

/* Default stack sizes */
#define DEFAULT_REF_STACK_SIZE (1024 * 16)    /* 16K entries */
#define DEFAULT_SUSP_STACK_SIZE (1024 * 8)    /* 8K entries */
#define DEFAULT_GENERIC_STACK_SIZE (1024 * 4) /* 4K entries */

/**
 * Initialize the garbage collection system
 */
void init_gc_64bit(size_t stack_size) {
    /* Use default stack size if none provided */
    if (stack_size == 0) {
        stack_size = DEFAULT_REF_STACK_SIZE;
    }
    
    /* Initialize reference stack */
    gc_state_64bit.ref_stack = (Term**)malloc(stack_size * sizeof(Term*));
    if (!gc_state_64bit.ref_stack) {
        fprintf(stderr, "Failed to allocate GC reference stack\n");
        exit(EXIT_FAILURE);
    }
    gc_state_64bit.ref_stack_start = gc_state_64bit.ref_stack;
    gc_state_64bit.ref_stack_top = gc_state_64bit.ref_stack;
    gc_state_64bit.ref_stack_end = gc_state_64bit.ref_stack + stack_size;
    
    /* Initialize suspension stack */
    gc_state_64bit.susp_stack = (gc_susp_entry_64bit_t*)malloc(DEFAULT_SUSP_STACK_SIZE * sizeof(gc_susp_entry_64bit_t));
    if (!gc_state_64bit.susp_stack) {
        fprintf(stderr, "Failed to allocate GC suspension stack\n");
        exit(EXIT_FAILURE);
    }
    gc_state_64bit.susp_stack_start = gc_state_64bit.susp_stack - 1; /* -1 to allow pre-increment */
    gc_state_64bit.susp_stack_top = gc_state_64bit.susp_stack_start;
    gc_state_64bit.susp_stack_end = gc_state_64bit.susp_stack + DEFAULT_SUSP_STACK_SIZE;
    
    /* Initialize generic object stack */
    gc_state_64bit.generic_stack = (void**)malloc(DEFAULT_GENERIC_STACK_SIZE * sizeof(void*));
    if (!gc_state_64bit.generic_stack) {
        fprintf(stderr, "Failed to allocate GC generic stack\n");
        exit(EXIT_FAILURE);
    }
    gc_state_64bit.generic_stack_top = gc_state_64bit.generic_stack;
    gc_state_64bit.generic_stack_end = gc_state_64bit.generic_stack + DEFAULT_GENERIC_STACK_SIZE;
    
    /* Initialize statistics */
    memset(&gc_state_64bit.stats, 0, sizeof(gc_stats_64bit_t));
    
    /* Initialize internal state */
    gc_state_64bit.current_flip = 0;
    gc_state_64bit.gc_in_progress = false;
}

/**
 * Expand the reference stack if needed
 */
void expand_ref_stack_64bit(void) {
    size_t current_size = gc_state_64bit.ref_stack_end - gc_state_64bit.ref_stack_start;
    size_t new_size = current_size * 2;
    size_t top_offset = gc_state_64bit.ref_stack_top - gc_state_64bit.ref_stack_start;
    
    /* Allocate new stack */
    Term** new_stack = (Term**)malloc(new_size * sizeof(Term*));
    if (!new_stack) {
        fprintf(stderr, "Failed to expand GC reference stack\n");
        exit(EXIT_FAILURE);
    }
    
    /* Copy old stack contents */
    memcpy(new_stack, gc_state_64bit.ref_stack_start, current_size * sizeof(Term*));
    
    /* Free old stack */
    free(gc_state_64bit.ref_stack_start);
    
    /* Update pointers */
    gc_state_64bit.ref_stack_start = new_stack;
    gc_state_64bit.ref_stack = new_stack;
    gc_state_64bit.ref_stack_top = new_stack + top_offset;
    gc_state_64bit.ref_stack_end = new_stack + new_size;
    
    if (gc_config_64bit.verbose_gc) {
        printf("GC: Reference stack expanded to %zu entries\n", new_size);
    }
}

/**
 * Expand the suspension stack if needed
 */
void expand_susp_stack_64bit(void) {
    size_t current_size = gc_state_64bit.susp_stack_end - gc_state_64bit.susp_stack;
    size_t new_size = current_size * 2;
    size_t top_offset = gc_state_64bit.susp_stack_top - gc_state_64bit.susp_stack_start;
    
    /* Allocate new stack */
    gc_susp_entry_64bit_t* new_stack = (gc_susp_entry_64bit_t*)malloc(new_size * sizeof(gc_susp_entry_64bit_t));
    if (!new_stack) {
        fprintf(stderr, "Failed to expand GC suspension stack\n");
        exit(EXIT_FAILURE);
    }
    
    /* Copy old stack contents */
    memcpy(new_stack, gc_state_64bit.susp_stack, current_size * sizeof(gc_susp_entry_64bit_t));
    
    /* Update top offset to account for the new start position */
    top_offset += 1; /* +1 because susp_stack_start is susp_stack-1 */
    
    /* Free old stack */
    free(gc_state_64bit.susp_stack);
    
    /* Update pointers */
    gc_state_64bit.susp_stack = new_stack;
    gc_state_64bit.susp_stack_start = new_stack - 1; /* -1 to allow pre-increment */
    gc_state_64bit.susp_stack_top = new_stack + top_offset - 1; /* -1 to adjust for the offset */
    gc_state_64bit.susp_stack_end = new_stack + new_size;
    
    if (gc_config_64bit.verbose_gc) {
        printf("GC: Suspension stack expanded to %zu entries\n", new_size);
    }
}

/**
 * Push a term's reference onto the reference stack for processing
 */
void push_ref_64bit(Term* ref) {
    /* Check if stack is full */
    if (gc_state_64bit.ref_stack_top >= gc_state_64bit.ref_stack_end) {
        expand_ref_stack_64bit();
    }
    
    /* Push reference */
    *gc_state_64bit.ref_stack_top++ = ref;
}

/**
 * Save a suspension for later processing
 */
void save_suspension_64bit(void* suspension, void** location) {
    /* Check if stack is full */
    if (gc_state_64bit.susp_stack_top + 1 >= gc_state_64bit.susp_stack_end) {
        expand_susp_stack_64bit();
    }
    
    /* Save suspension */
    gc_state_64bit.susp_stack_top++;
    gc_state_64bit.susp_stack_top->suspension = suspension;
    gc_state_64bit.susp_stack_top->location = location;
}

/**
 * Mark a term as live (not garbage)
 */
void mark_term_64bit(Term term, gc_state_64bit_t* gc_state) {
    /* Skip if term is already marked */
    if (IsMarked(term)) {
        return;
    }
    
    /* Mark the term */
    term = Mark(term);
    
    /* Process based on term type */
    if (IsRef(term)) {
        /* For reference terms, mark what they point to */
        Term* ref_ptr = (Term*)AddressOf(term);
        push_ref_64bit(ref_ptr);
    } else if (IsLst(term)) {
        /* For list terms, mark head and tail */
        list_t* lst = (list_t*)AddressOf(term);
        push_ref_64bit(&lst->head);
        push_ref_64bit(&lst->tail);
    } else if (IsStr(term)) {
        /* For structure terms, mark all arguments */
        structure_t* str = (structure_t*)AddressOf(term);
        int arity = str->arity;
        for (int i = 0; i < arity; i++) {
            push_ref_64bit(&str->args[i]);
        }
    } else if (IsGen(term)) {
        /* For generic terms, special handling might be needed */
        /* This depends on the specific generic object implementation */
        /* For now, just mark it */
    } else if (IsNum(term)) {
        /* Small integers are immediate values and don't need marking */
        /* No action needed */
    }
}

/**
 * Mark a variable as live (not garbage)
 */
void mark_variable_64bit(Term* var_ptr, gc_state_64bit_t* gc_state) {
    Term term = *var_ptr;
    
    /* Mark the term */
    mark_term_64bit(term, gc_state);
}

/**
 * Process the reference stack, marking all reachable objects
 */
void process_ref_stack_64bit(gc_state_64bit_t* gc_state) {
    /* Process until stack is empty */
    while (gc_state->ref_stack_top > gc_state->ref_stack_start) {
        /* Pop reference */
        Term* ref_ptr = *--gc_state->ref_stack_top;
        
        /* Mark the referenced term */
        Term term = *ref_ptr;
        mark_term_64bit(term, gc_state);
    }
}

/**
 * Process the suspension stack, copying all suspensions
 */
void process_suspension_stack_64bit(gc_state_64bit_t* gc_state) {
    /* Process all suspensions */
    while (gc_state->susp_stack_top > gc_state->susp_stack_start) {
        /* Process suspension */
        gc_susp_entry_64bit_t* entry = gc_state->susp_stack_top--;
        
        /* Update suspension pointer in original location */
        /* (implementation depends on suspension structure) */
        /* This typically involves copying the suspension to the new heap */
        /* and updating the pointer to point to the new location */
        
        /* Silence unused variable warning */
        (void)entry;
    }
}

/**
 * Run a full garbage collection cycle
 */
bool run_gc_64bit(void* exec_state) {
    akl_clock_t start_time, end_time;
    
    /* Silence unused parameter warning */
    (void)exec_state;
    
    /* Return if GC is already in progress */
    if (gc_state_64bit.gc_in_progress) {
        return false;
    }
    
    /* Start timing */
    start_time = akl_clock();
    
    /* Set GC in progress flag */
    gc_state_64bit.gc_in_progress = true;
    
    /* Begin GC cycle */
    if (gc_config_64bit.verbose_gc) {
        printf("Starting garbage collection cycle...\n");
    }
    
    /* Flip the marking bit for this cycle */
    gc_state_64bit.current_flip = (gc_state_64bit.current_flip == 0 ? 1 : 0);
    
    /* Call the memory system's GC begin function */
    gc_begin_64bit();
    
    /* Reset stacks */
    gc_state_64bit.ref_stack_top = gc_state_64bit.ref_stack_start;
    gc_state_64bit.susp_stack_top = gc_state_64bit.susp_stack_start;
    gc_state_64bit.generic_stack_top = gc_state_64bit.generic_stack;
    
    /* Mark root objects (this depends on the specific execution state structure) */
    /* For each root object (registers, global variables, etc.): */
    /*   mark_term_64bit(root_term, &gc_state_64bit); */
    
    /* Process all references */
    process_ref_stack_64bit(&gc_state_64bit);
    
    /* Process all suspensions */
    process_suspension_stack_64bit(&gc_state_64bit);
    
    /* Call the memory system's GC end function */
    gc_end_64bit();
    
    /* Update statistics */
    gc_state_64bit.stats.gc_cycles++;
    /* Update other statistics based on what was freed */
    
    /* Clear GC in progress flag */
    gc_state_64bit.gc_in_progress = false;
    
    /* End timing */
    end_time = akl_clock();
    gc_state_64bit.stats.total_gc_time_ms += (uint64_t)((double)(end_time - start_time) * 1000 / AKL_CLOCKS_PER_SEC);
    gc_state_64bit.stats.avg_gc_time_ms = gc_state_64bit.stats.total_gc_time_ms / gc_state_64bit.stats.gc_cycles;
    
    if (gc_config_64bit.verbose_gc) {
        printf("Garbage collection completed in %lu ms\n", 
               (uint64_t)((double)(end_time - start_time) * 1000 / AKL_CLOCKS_PER_SEC));
    }
    
    return true;
}

/**
 * Reset the GC system after a collection
 */
void reset_gc_64bit(void) {
    /* Reset stacks */
    gc_state_64bit.ref_stack_top = gc_state_64bit.ref_stack_start;
    gc_state_64bit.susp_stack_top = gc_state_64bit.susp_stack_start;
    gc_state_64bit.generic_stack_top = gc_state_64bit.generic_stack;
    
    /* Reset in-progress flag */
    gc_state_64bit.gc_in_progress = false;
}

/**
 * Configure the GC system
 */
void configure_gc_64bit(gc_config_64bit_t config) {
    gc_config_64bit = config;
}

/**
 * Get current GC statistics
 */
gc_stats_64bit_t get_gc_stats_64bit(void) {
    return gc_state_64bit.stats;
}

/**
 * Check if an object is marked in the current GC cycle
 */
bool is_marked_64bit(Term term) {
    return IsMarked(term);
}

/**
 * Clean up and free GC resources
 */
void cleanup_gc_64bit(void) {
    /* Free stacks */
    free(gc_state_64bit.ref_stack);
    free(gc_state_64bit.susp_stack);
    free(gc_state_64bit.generic_stack);
    
    /* Reset state */
    memset(&gc_state_64bit, 0, sizeof(gc_state_64bit_t));
}
