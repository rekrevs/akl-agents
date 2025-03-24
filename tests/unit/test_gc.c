#include "test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * Test file for the garbage collection system
 * 
 * This will test the modernized garbage collection implementation,
 * focusing on marking, sweeping, and memory reclamation.
 */

// Include the garbage collection header once it's modernized
// #include "emulator/gc.h"

// For now, we'll define some placeholder functions to demonstrate the testing approach
// These will be replaced with the actual implementation when we modernize the code

// Placeholder for GC-managed object
typedef struct gc_object {
    uintptr_t header;  // Contains GC bit and other metadata
    struct gc_object* next;  // For linked list of objects
    char data[];       // Actual object data
} gc_object_t;

// Placeholder for GC context
typedef struct gc_context {
    gc_object_t* objects;  // List of all objects
    size_t total_objects;
    size_t marked_objects;
    size_t collected_objects;
} gc_context_t;

// Placeholder functions for GC
gc_context_t* gc_create_context() {
    gc_context_t* ctx = (gc_context_t*)malloc(sizeof(gc_context_t));
    if (!ctx) return NULL;
    
    ctx->objects = NULL;
    ctx->total_objects = 0;
    ctx->marked_objects = 0;
    ctx->collected_objects = 0;
    
    return ctx;
}

void gc_destroy_context(gc_context_t* ctx) {
    if (!ctx) return;
    
    // Free all objects
    gc_object_t* obj = ctx->objects;
    while (obj) {
        gc_object_t* next = obj->next;
        free(obj);
        obj = next;
    }
    
    free(ctx);
}

gc_object_t* gc_allocate(gc_context_t* ctx, size_t size) {
    if (!ctx) return NULL;
    
    // Allocate object with header and data
    gc_object_t* obj = (gc_object_t*)malloc(sizeof(gc_object_t) + size);
    if (!obj) return NULL;
    
    // Initialize header (no GC bit set)
    obj->header = 0;
    
    // Add to list of objects
    obj->next = ctx->objects;
    ctx->objects = obj;
    ctx->total_objects++;
    
    return obj;
}

void gc_mark(gc_context_t* ctx, gc_object_t* obj) {
    if (!ctx || !obj) return;
    
    // Check if already marked
    if (obj->header & 0x80000000) return;
    
    // Mark the object
    obj->header |= 0x80000000;
    ctx->marked_objects++;
    
    // Recursively mark any referenced objects
    // (In a real implementation, this would traverse the object graph)
}

void gc_sweep(gc_context_t* ctx) {
    if (!ctx) return;
    
    gc_object_t** obj_ptr = &ctx->objects;
    while (*obj_ptr) {
        gc_object_t* obj = *obj_ptr;
        
        if (obj->header & 0x80000000) {
            // Object is marked, clear mark for next GC cycle
            obj->header &= ~0x80000000;
            obj_ptr = &obj->next;
        } else {
            // Object is not marked, remove it from the list
            *obj_ptr = obj->next;
            free(obj);
            ctx->collected_objects++;
        }
    }
    
    // Reset marked objects count
    ctx->marked_objects = 0;
}

void gc_collect(gc_context_t* ctx) {
    if (!ctx) return;
    
    // Reset counters
    ctx->marked_objects = 0;
    ctx->collected_objects = 0;
    
    // Mark phase would go here
    // (In a real implementation, this would start from root objects)
    
    // Sweep phase
    gc_sweep(ctx);
}

// Test functions

void test_gc_allocation() {
    // Create GC context
    gc_context_t* ctx = gc_create_context();
    ASSERT_NOT_NULL(ctx);
    
    // Allocate some objects
    gc_object_t* obj1 = gc_allocate(ctx, 100);
    ASSERT_NOT_NULL(obj1);
    
    gc_object_t* obj2 = gc_allocate(ctx, 200);
    ASSERT_NOT_NULL(obj2);
    
    // Check that the objects are in the list
    ASSERT_EQUAL_INT(2, ctx->total_objects);
    
    // Check that the objects are linked correctly
    ASSERT_EQUAL_PTR(obj2, ctx->objects);
    ASSERT_EQUAL_PTR(obj1, obj2->next);
    
    // Clean up
    gc_destroy_context(ctx);
}

void test_gc_mark_sweep() {
    // Create GC context
    gc_context_t* ctx = gc_create_context();
    ASSERT_NOT_NULL(ctx);
    
    // Allocate some objects
    gc_object_t* obj1 = gc_allocate(ctx, 100);
    gc_object_t* obj2 = gc_allocate(ctx, 200);
    gc_object_t* obj3 = gc_allocate(ctx, 300);
    
    // Mark some objects
    gc_mark(ctx, obj1);
    gc_mark(ctx, obj3);
    
    // Check that the objects are marked
    ASSERT_EQUAL_INT(2, ctx->marked_objects);
    
    // Sweep
    gc_sweep(ctx);
    
    // Check that unmarked objects were collected
    ASSERT_EQUAL_INT(1, ctx->collected_objects);
    
    // Check that the remaining objects are in the list
    ASSERT_EQUAL_INT(2, ctx->total_objects - ctx->collected_objects);
    
    // Clean up
    gc_destroy_context(ctx);
}

void test_gc_full_cycle() {
    // Create GC context
    gc_context_t* ctx = gc_create_context();
    ASSERT_NOT_NULL(ctx);
    
    // Allocate some objects
    gc_allocate(ctx, 100);
    gc_allocate(ctx, 200);
    gc_allocate(ctx, 300);
    
    // Check that the objects are in the list
    ASSERT_EQUAL_INT(3, ctx->total_objects);
    
    // Run a GC cycle without marking anything
    // (All objects should be collected)
    gc_collect(ctx);
    
    // Check that all objects were collected
    ASSERT_EQUAL_INT(3, ctx->collected_objects);
    
    // Check that the list is empty
    ASSERT_NULL(ctx->objects);
    
    // Clean up
    gc_destroy_context(ctx);
}

int main(int argc, char** argv) {
    // Create test suite
    test_suite_t* suite = test_suite_create("Garbage Collection");
    
    // Run tests
    RUN_TEST(suite, test_gc_allocation);
    RUN_TEST(suite, test_gc_mark_sweep);
    RUN_TEST(suite, test_gc_full_cycle);
    
    // Print results
    test_suite_print_results(suite);
    
    // Get status
    int status = test_suite_status(suite);
    
    // Cleanup
    test_suite_destroy(suite);
    
    return status;
}
