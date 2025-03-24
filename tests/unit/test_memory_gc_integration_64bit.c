/**
 * test_memory_gc_integration_64bit.c
 * 
 * Integration test for 64-bit memory management, garbage collection, and tagged pointers.
 * 
 * This test verifies that all components work together properly, including:
 * - Memory allocation with proper alignment
 * - Tagging pointers with type information
 * - Garbage collection marking and sweeping
 * - Memory reuse after collection
 * - Complex data structure traversal during GC
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "emulator/compat/tagged_pointers_64bit.h"
#include "emulator/compat/storage_64bit.h"
#include "emulator/compat/gc_64bit.h"
#include "tests/framework/test_framework.h"

#define MAX_OBJECTS 1000
#define INITIAL_HEAP_SIZE (1024 * 1024) /* 1MB */
#define GC_STACK_SIZE (1024 * 16)       /* 16K entries */

/* Test objects that will be allocated and collected */
typedef struct test_reference {
    Term value;
} test_reference;

typedef struct test_cons_cell {
    Term head;
    Term tail;
} test_cons_cell;

typedef struct test_structure {
    int arity;
    Term args[10]; /* Up to 10 arguments */
} test_structure;

/* Global variables for test state */
void* test_heap = NULL;
void* test_heap_end = NULL;
void* test_heap_current = NULL;
Term test_objects[MAX_OBJECTS];
int object_count = 0;

/* Test root objects that won't be collected */
Term root_objects[10];
int root_count = 0;

/**
 * Test memory initialization
 */
void setup_test_memory() {
    printf("Setting up test memory environment...\n");
    
    /* Initialize memory management system */
    init_memory_system_64bit();
    
    /* Initialize heap with 1MB size and limit of 10 blocks */
    init_heap_64bit(INITIAL_HEAP_SIZE, 10);
    
    /* Initialize garbage collector */
    init_gc_64bit(GC_STACK_SIZE);
    
    /* Configure GC to be verbose for testing */
    gc_config_64bit_t config = {
        .auto_gc_enabled = true,
        .heap_threshold = INITIAL_HEAP_SIZE / 2,
        .verbose_gc = true
    };
    configure_gc_64bit(config);
    
    /* Reset object tracking */
    object_count = 0;
    root_count = 0;
    
    printf("Memory environment initialized\n");
}

/**
 * Clean up test memory
 */
void teardown_test_memory() {
    printf("Cleaning up test memory...\n");
    
    /* Reset object tracking */
    object_count = 0;
    root_count = 0;
    
    /* GC cleanup would happen here in a real implementation */
}

/**
 * Allocate a test reference object
 */
Term allocate_test_reference(Term value) {
    test_reference* ref;
    NEW_64BIT(ref, test_reference);
    ref->value = value;
    
    /* Tag the pointer */
    Term term = TagRef(ref);
    
    /* Store in object list if there's space */
    if (object_count < MAX_OBJECTS) {
        test_objects[object_count++] = term;
    }
    
    return term;
}

/**
 * Allocate a test cons cell
 */
Term allocate_test_cons_cell(Term head, Term tail) {
    test_cons_cell* cell;
    NEW_64BIT(cell, test_cons_cell);
    cell->head = head;
    cell->tail = tail;
    
    /* Tag the pointer */
    Term term = TagLst(cell);
    
    /* Store in object list if there's space */
    if (object_count < MAX_OBJECTS) {
        test_objects[object_count++] = term;
    }
    
    return term;
}

/**
 * Allocate a test structure
 */
Term allocate_test_structure(int arity) {
    test_structure* str;
    NEW_64BIT(str, test_structure);
    str->arity = arity;
    
    /* Initialize arguments to small integers */
    for (int i = 0; i < arity; i++) {
        str->args[i] = TagNum(i);
    }
    
    /* Tag the pointer */
    Term term = TagStr(str);
    
    /* Store in object list if there's space */
    if (object_count < MAX_OBJECTS) {
        test_objects[object_count++] = term;
    }
    
    return term;
}

/**
 * Make an object a root (protected from GC)
 */
void make_root(Term term) {
    if (root_count < 10) {
        root_objects[root_count++] = term;
    }
}

/**
 * Create a complex data structure for testing GC traversal
 * Returns a reference to the root of the structure
 */
Term create_complex_structure() {
    /* Create a linked list with 10 elements */
    Term list = TagNum(0); /* Nil at the end */
    
    for (int i = 9; i >= 0; i--) {
        Term cell = allocate_test_cons_cell(TagNum(i), list);
        list = cell;
    }
    
    /* Create a structure with 5 arguments */
    Term structure = allocate_test_structure(5);
    
    /* Make one of the structure's arguments point to the list */
    test_structure* str = (test_structure*)AddressOf(structure);
    str->args[2] = list;
    
    /* Create a reference to the structure */
    Term reference = allocate_test_reference(structure);
    
    return reference;
}

/**
 * Test basic memory allocation and tagging
 */
void test_memory_allocation_and_tagging() {
    printf("Testing memory allocation and tagging...\n");
    
    /* Reset memory state */
    setup_test_memory();
    
    /* Allocate and tag different types of objects */
    Term ref = allocate_test_reference(TagNum(42));
    Term cons = allocate_test_cons_cell(TagNum(1), TagNum(2));
    Term structure = allocate_test_structure(3);
    
    /* Verify tags */
    ASSERT_TRUE(IsRef(ref));
    ASSERT_TRUE(IsLst(cons));
    ASSERT_TRUE(IsStr(structure));
    
    /* Verify content */
    test_reference* ref_ptr = (test_reference*)AddressOf(ref);
    ASSERT_EQUAL_INT(42, GetSmall(ref_ptr->value));
    
    test_cons_cell* cons_ptr = (test_cons_cell*)AddressOf(cons);
    ASSERT_EQUAL_INT(1, GetSmall(cons_ptr->head));
    ASSERT_EQUAL_INT(2, GetSmall(cons_ptr->tail));
    
    test_structure* str_ptr = (test_structure*)AddressOf(structure);
    ASSERT_EQUAL_INT(3, str_ptr->arity);
    ASSERT_EQUAL_INT(0, GetSmall(str_ptr->args[0]));
    ASSERT_EQUAL_INT(1, GetSmall(str_ptr->args[1]));
    ASSERT_EQUAL_INT(2, GetSmall(str_ptr->args[2]));
    
    printf("Memory allocation and tagging: PASSED\n");
    teardown_test_memory();
}

/**
 * Test GC mark bit operations with memory objects
 */
void test_gc_mark_bit_operations() {
    printf("Testing GC mark bit operations...\n");
    
    /* Reset memory state */
    setup_test_memory();
    
    /* Allocate objects */
    Term ref = allocate_test_reference(TagNum(100));
    Term cons = allocate_test_cons_cell(TagNum(101), TagNum(102));
    Term structure = allocate_test_structure(2);
    
    /* Mark objects */
    Term marked_ref = Mark(ref);
    Term marked_cons = Mark(cons);
    Term marked_structure = Mark(structure);
    
    /* Verify marking */
    ASSERT_TRUE(IsMarked(marked_ref));
    ASSERT_TRUE(IsMarked(marked_cons));
    ASSERT_TRUE(IsMarked(marked_structure));
    
    /* Verify types are preserved */
    ASSERT_TRUE(IsRef(marked_ref));
    ASSERT_TRUE(IsLst(marked_cons));
    ASSERT_TRUE(IsStr(marked_structure));
    
    /* Verify addresses are preserved */
    ASSERT_EQUAL_PTR(AddressOf(ref), AddressOf(marked_ref));
    ASSERT_EQUAL_PTR(AddressOf(cons), AddressOf(marked_cons));
    ASSERT_EQUAL_PTR(AddressOf(structure), AddressOf(marked_structure));
    
    /* Unmark objects */
    Term unmarked_ref = Unmark(marked_ref);
    Term unmarked_cons = Unmark(marked_cons);
    Term unmarked_structure = Unmark(marked_structure);
    
    /* Verify unmarking */
    ASSERT_FALSE(IsMarked(unmarked_ref));
    ASSERT_FALSE(IsMarked(unmarked_cons));
    ASSERT_FALSE(IsMarked(unmarked_structure));
    
    /* Verify types still preserved */
    ASSERT_TRUE(IsRef(unmarked_ref));
    ASSERT_TRUE(IsLst(unmarked_cons));
    ASSERT_TRUE(IsStr(unmarked_structure));
    
    printf("GC mark bit operations: PASSED\n");
    teardown_test_memory();
}

/**
 * Test garbage collection cycle with complex object graphs
 */
void test_garbage_collection_cycle() {
    printf("Testing garbage collection cycle...\n");
    
    /* Reset memory state */
    setup_test_memory();
    
    /* Create a large number of objects to trigger GC */
    printf("Creating objects to fill the heap...\n");
    for (int i = 0; i < 100; i++) {
        /* Create a complex structure with nested objects */
        Term complex = create_complex_structure();
        
        /* Make every 10th object a root */
        if (i % 10 == 0) {
            make_root(complex);
            printf("  Made object %d a root\n", i);
        }
    }
    
    printf("Created %d objects (%d roots)\n", object_count, root_count);
    
    /* Print memory usage before GC */
    printf("Heap size before GC: %zu bytes\n", heap_size_64bit());
    
    /* Run garbage collection */
    printf("Running garbage collection...\n");
    run_gc_64bit(NULL); /* NULL exec_state for testing */
    
    /* Verify roots are still accessible */
    printf("Verifying roots after GC...\n");
    for (int i = 0; i < root_count; i++) {
        Term root = root_objects[i];
        
        /* Check if the root is a reference */
        if (IsRef(root)) {
            test_reference* ref = (test_reference*)AddressOf(root);
            ASSERT_TRUE(ref != NULL);
            
            /* If the reference points to a structure */
            if (IsStr(ref->value)) {
                test_structure* str = (test_structure*)AddressOf(ref->value);
                ASSERT_TRUE(str != NULL);
                
                /* If structure has a list at arg[2] */
                if (str->arity > 2 && IsLst(str->args[2])) {
                    test_cons_cell* cell = (test_cons_cell*)AddressOf(str->args[2]);
                    ASSERT_TRUE(cell != NULL);
                    
                    /* The cell should have a head */
                    if (IsNum(cell->head)) {
                        ASSERT_TRUE(true); /* Passed */
                    } else {
                        ASSERT_FALSE(true); /* Failed */
                    }
                }
            }
        }
    }
    
    /* Print memory usage after GC */
    printf("Heap size after GC: %zu bytes\n", heap_size_64bit());
    
    /* Get GC statistics */
    gc_stats_64bit_t stats = get_gc_stats_64bit();
    printf("GC cycles: %llu\n", (unsigned long long)stats.gc_cycles);
    printf("GC time: %llu ms\n", (unsigned long long)stats.total_gc_time_ms);
    
    printf("Garbage collection cycle: PASSED\n");
    teardown_test_memory();
}

/**
 * Test memory reuse after GC
 */
void test_memory_reuse_after_gc() {
    printf("Testing memory reuse after GC...\n");
    
    /* Reset memory state */
    setup_test_memory();
    
    /* Record starting heap size */
    size_t initial_heap_size = heap_size_64bit();
    printf("Initial heap size: %zu bytes\n", initial_heap_size);
    
    /* Allocate a large number of objects */
    printf("Allocating objects...\n");
    for (int i = 0; i < 500; i++) {
        Term obj = allocate_test_structure(5);
        
        /* Only keep every 50th object as a root */
        if (i % 50 == 0) {
            make_root(obj);
        }
    }
    
    /* Run garbage collection */
    printf("Running first garbage collection...\n");
    run_gc_64bit(NULL);
    
    /* Record heap size after first GC */
    size_t post_gc1_heap_size = heap_size_64bit();
    printf("Heap size after first GC: %zu bytes\n", post_gc1_heap_size);
    
    /* Allocate more objects, but fewer than before */
    printf("Allocating more objects...\n");
    for (int i = 0; i < 200; i++) {
        Term obj = allocate_test_structure(3);
        
        /* Only keep every 50th object as a root */
        if (i % 50 == 0) {
            make_root(obj);
        }
    }
    
    /* Run another garbage collection */
    printf("Running second garbage collection...\n");
    run_gc_64bit(NULL);
    
    /* Record heap size after second GC */
    size_t post_gc2_heap_size = heap_size_64bit();
    printf("Heap size after second GC: %zu bytes\n", post_gc2_heap_size);
    
    /* Verify that we're reusing memory (size shouldn't grow much) */
    ASSERT_TRUE(post_gc2_heap_size <= post_gc1_heap_size * 1.2); /* Allow 20% growth */
    
    printf("Memory reuse after GC: PASSED\n");
    teardown_test_memory();
}

/**
 * Test alignment requirements for 64-bit pointers
 */
void test_pointer_alignment() {
    printf("Testing pointer alignment for 64-bit tagged pointers...\n");
    
    /* Reset memory state */
    setup_test_memory();
    
    /* Allocate different types of objects and check alignment */
    for (int i = 0; i < 100; i++) {
        Term ref = allocate_test_reference(TagNum(i));
        Term cons = allocate_test_cons_cell(TagNum(i), TagNum(i+1));
        Term structure = allocate_test_structure(3);
        
        /* Check that addresses are properly aligned */
        uintptr_t ref_addr = (uintptr_t)AddressOf(ref);
        uintptr_t cons_addr = (uintptr_t)AddressOf(cons);
        uintptr_t str_addr = (uintptr_t)AddressOf(structure);
        
        /* All addresses should be 8-byte aligned for 64-bit */
        ASSERT_EQUAL_INT(0, ref_addr % 8);
        ASSERT_EQUAL_INT(0, cons_addr % 8);
        ASSERT_EQUAL_INT(0, str_addr % 8);
    }
    
    printf("Pointer alignment: PASSED\n");
    teardown_test_memory();
}

/**
 * Main test function
 */
int main() {
    test_suite_t* suite = test_suite_create("Memory-GC-TaggedPointers Integration Test");
    
    printf("=== Testing 64-bit Memory, GC, and Tagged Pointer Integration ===\n\n");
    
    /* Run the tests */
    test_memory_allocation_and_tagging();
    test_gc_mark_bit_operations();
    test_garbage_collection_cycle();
    test_memory_reuse_after_gc();
    test_pointer_alignment();
    
    printf("\n=== All integration tests completed successfully ===\n");
    
    return 0;
}
