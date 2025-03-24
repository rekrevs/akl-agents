/**
 * storage_64bit.c - Modern 64-bit implementation of AKL's memory management
 *
 * This file provides the implementation of memory management primitives that
 * integrate with the 64-bit tagged pointer system.
 */

#include "emulator/compat/storage_64bit.h"
#include "emulator/compat/debug_checks.h"
#include "emulator/compat/structure_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global memory management variables */
memory_block_t*  global_block_list = NULL;      /* Global list of all memory blocks */
memory_block_t*  current_memory_block = NULL;   /* Current block for allocations */
memory_block_t*  unused_block_list = NULL;      /* List of unused memory blocks */
memory_block_t*  used_block_list = NULL;        /* List of used memory blocks */

void*            global_heap_end = NULL;        /* End of the current heap block */
void*            global_heap_current = NULL;    /* Current allocation point in heap */

bool             gc_flag = false;               /* Flag indicating if GC is needed */
int              block_limit = 0;               /* Maximum number of blocks before GC */
int              active_blocks = 0;             /* Current number of active blocks */
size_t           memory_block_size = 0;         /* Size of each memory block */

/* Constant memory area */
void*            const_space = NULL;            /* Start of the constant area */
void*            const_current = NULL;          /* Current position in constant area */
void*            const_end = NULL;              /* End of the constant area */

/* GC flip-flop bit for marking */
int              current_flip = 0;              /* Current flip-flop state (0 or 1) */

/**
 * Allocate a new memory block of specified size
 */
memory_block_t* allocate_memory_block(size_t size) {
    memory_block_t* new_block;
    void* memory_segment;
    
    /* Allocate the block control structure */
    new_block = (memory_block_t*)malloc(sizeof(memory_block_t));
    if (!new_block) {
        fprintf(stderr, "Failed to allocate memory block control structure\n");
        exit(EXIT_FAILURE);
    }
    
    /* Allocate the actual memory segment */
    memory_segment = malloc(size);
    if (!memory_segment) {
        fprintf(stderr, "Failed to allocate memory segment of size %zu\n", size);
        free(new_block);
        exit(EXIT_FAILURE);
    }
    
    /* Initialize the block */
    new_block->start = memory_segment;
    new_block->end = (void*)((uintptr_t)memory_segment + size);
    new_block->logical_end = new_block->end;
    new_block->next = NULL;
    new_block->size = size;
    
    return new_block;
}

/**
 * Get a new heap block, either from the unused list or allocate a new one
 */
memory_block_t* get_heap_block() {
    memory_block_t* block;
    
    if (unused_block_list) {
        /* Reuse an existing block from the unused list */
        block = unused_block_list;
        unused_block_list = unused_block_list->next;
    } else {
        /* Allocate a new block */
        block = allocate_memory_block(memory_block_size);
    }
    
    block->next = NULL;
    active_blocks++;
    
    return block;
}

/**
 * Initialize the heap with specified size and block limit
 */
void init_heap_64bit(size_t size, int limit) {
    /* Set the block size and limit */
    memory_block_size = size;
    block_limit = limit;
    active_blocks = 0;
    
    /* Initialize global block list */
    global_block_list = get_heap_block();
    current_memory_block = global_block_list;
    
    /* Initialize heap pointers */
    global_heap_current = current_memory_block->start;
    global_heap_end = current_memory_block->end;
    
    /* Initialize lists */
    unused_block_list = NULL;
    used_block_list = NULL;
    
    /* Reset GC flag */
    gc_flag = false;
}

/**
 * Reinitialize the heap when current block is exhausted
 */
void reinit_heap_64bit(void* ptr, size_t size_needed) {
    /* Check if object is too large for a single block */
    if (size_needed > memory_block_size) {
        fprintf(stderr, "Cannot create object larger than %zu bytes\n", memory_block_size);
        exit(EXIT_FAILURE);
    }
    
    /* Check if garbage collection is needed */
    if (active_blocks >= block_limit) {
        gc_flag = true;
    }
    
    active_blocks++;
    
    /* Mark the logical end of the current block */
    current_memory_block->logical_end = ptr;
    
    /* Get a new block and make it current */
    current_memory_block->next = get_heap_block();
    current_memory_block = current_memory_block->next;
    
    /* Update heap pointers */
    global_heap_current = current_memory_block->start;
    global_heap_end = current_memory_block->end;
}

/**
 * Restore a previously saved memory block
 */
void restore_memory_block_64bit(memory_block_t* saved_block) {
    memory_block_t* last_block;
    
    /* Set the current block to the saved one */
    current_memory_block = saved_block;
    
    /* Count blocks to be released and find the last block */
    for (last_block = current_memory_block->next; 
         last_block->next != NULL; 
         last_block = last_block->next) {
        active_blocks--;
    }
    active_blocks--;
    
    /* Add the unused blocks to the unused list */
    last_block->next = unused_block_list;
    unused_block_list = current_memory_block->next;
    
    /* Reset the current block */
    current_memory_block->logical_end = current_memory_block->end;
    current_memory_block->next = NULL;
}

/**
 * Begin garbage collection process
 */
void gc_begin_64bit() {
    /* Save the current used blocks */
    used_block_list = global_block_list;
    
    /* Flip the marking bit for this GC cycle */
    current_flip = (current_flip == 0 ? 1 : 0);
    
    /* Reset the block list for after GC */
    global_block_list = get_heap_block();
    current_memory_block = global_block_list;
    
    /* Reset heap pointers */
    global_heap_current = current_memory_block->start;
    global_heap_end = current_memory_block->end;
    
    /* Reset active block count */
    active_blocks = 1;
}

/**
 * End garbage collection process
 */
void gc_end_64bit() {
    memory_block_t* block;
    memory_block_t* next;
    
    /* Free the used blocks (they are no longer needed) */
    for (block = used_block_list; block != NULL; block = next) {
        next = block->next;
        
        /* Special handling for the first block that contains constants */
        if (block == used_block_list) {
            /* Keep the constant area which is at the start of the first block */
            block->next = unused_block_list;
            unused_block_list = block;
        } else {
            /* Release the memory and control structure */
            free(block->start);
            free(block);
        }
    }
    
    /* Reset the used list */
    used_block_list = NULL;
    
    /* Reset GC flag */
    gc_flag = false;
}

/**
 * Get the start address of the heap
 */
void* heap_start_64bit() {
    return global_block_list->start;
}

/**
 * Get the total size of the heap
 */
size_t heap_size_64bit() {
    memory_block_t* block;
    size_t total_size = 0;
    
    /* Sum up the size of all active blocks */
    for (block = global_block_list; block != NULL; block = block->next) {
        total_size += block->size;
    }
    
    return total_size;
}

/**
 * Initialize the constant space
 */
void init_const_space_64bit(size_t size) {
    /* Allocate memory for constants */
    const_space = malloc(size);
    if (!const_space) {
        fprintf(stderr, "Failed to allocate constant space of size %zu\n", size);
        exit(EXIT_FAILURE);
    }
    
    /* Initialize pointers */
    const_current = const_space;
    const_end = (void*)((uintptr_t)const_space + size);
    
    /* Clear the memory */
    memset(const_space, 0, size);
}

/**
 * Allocate memory in the constant space
 */
void* const_alloc_64bit(size_t size) {
    void* ptr;
    size_t aligned_size = ALIGN_64BIT(size);
    
    /* Check if there's enough space */
    if ((uintptr_t)const_current + aligned_size > (uintptr_t)const_end) {
        fprintf(stderr, "Constant space exhausted\n");
        exit(EXIT_FAILURE);
    }
    
    /* Allocate memory */
    ptr = const_current;
    const_current = (void*)((uintptr_t)const_current + aligned_size);
    
    return ptr;
}

/**
 * Check if a pointer is in the constant space
 */
bool is_constant_64bit(void* ptr) {
    return (ptr >= const_space && ptr < const_end);
}

/**
 * Trigger a garbage collection cycle
 */
bool trigger_garbage_collection_64bit() {
    /* Call the actual GC implementation (would call gc() in the original code) */
    printf("Triggering garbage collection...\n");
    
    /* This should be replaced with the actual GC implementation */
    /* For now, just reset the GC flag */
    gc_flag = false;
    
    return true;
}

/**
 * Initialize the memory management system
 */
void init_memory_system_64bit() {
    /* Set default values if not already set */
    if (memory_block_size == 0) {
        memory_block_size = 1024 * 1024; /* Default to 1MB blocks */
    }
    
    if (block_limit == 0) {
        block_limit = 10; /* Default to 10 blocks before GC */
    }
    
    /* Initialize the heap */
    init_heap_64bit(memory_block_size, block_limit);
    
    /* Initialize constant space */
    init_const_space_64bit(memory_block_size / 4); /* Use 1/4 of block size for constants */
    
    /* Initialize the first flip-flop state */
    current_flip = 0;
}
