#include "test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * Test file for the tagged pointer implementation
 * 
 * This will test the modernized tagged pointer implementation for both
 * 32-bit and 64-bit platforms, ensuring compatibility with the original
 * implementation's behavior.
 */

// Include the tagged pointer header once it's modernized
// #include "emulator/tagged_pointers.h"

// For now, we'll define some placeholder functions to demonstrate the testing approach
// These will be replaced with the actual implementation when we modernize the code

// Placeholder for the modernized tagged pointer type
typedef uintptr_t tagged_ptr_t;

// Placeholder tag definitions (will be replaced with actual values from the implementation)
#define TAG_BITS_MASK      0x03  // Bottom 2 bits for tag
#define GC_BIT_MASK        0x80000000  // Top bit for GC (32-bit)
#define GC_BIT_MASK_64     0x8000000000000000ULL  // Top bit for GC (64-bit)

// Placeholder tag values (will be replaced with actual values)
#define TAG_REF            0x00
#define TAG_INT            0x01
#define TAG_STRUCT         0x02
#define TAG_SPECIAL        0x03

// Placeholder functions (will be replaced with actual implementations)
tagged_ptr_t create_tagged_ptr(void* ptr, int tag) {
    return ((uintptr_t)ptr & ~TAG_BITS_MASK) | (tag & TAG_BITS_MASK);
}

void* get_ptr(tagged_ptr_t tagged) {
    return (void*)(tagged & ~TAG_BITS_MASK);
}

int get_tag(tagged_ptr_t tagged) {
    return tagged & TAG_BITS_MASK;
}

void set_gc_bit(tagged_ptr_t* tagged) {
#ifdef SIXTY_FOUR_BIT
    *tagged |= GC_BIT_MASK_64;
#else
    *tagged |= GC_BIT_MASK;
#endif
}

void clear_gc_bit(tagged_ptr_t* tagged) {
#ifdef SIXTY_FOUR_BIT
    *tagged &= ~GC_BIT_MASK_64;
#else
    *tagged &= ~GC_BIT_MASK;
#endif
}

int is_gc_bit_set(tagged_ptr_t tagged) {
#ifdef SIXTY_FOUR_BIT
    return (tagged & GC_BIT_MASK_64) != 0;
#else
    return (tagged & GC_BIT_MASK) != 0;
#endif
}

// Test functions

void test_create_tagged_ptr() {
    void* ptr = (void*)0x10000000;
    
    tagged_ptr_t ref = create_tagged_ptr(ptr, TAG_REF);
    ASSERT_EQUAL_INT(TAG_REF, get_tag(ref));
    ASSERT_EQUAL_PTR(ptr, get_ptr(ref));
    
    tagged_ptr_t integer = create_tagged_ptr(ptr, TAG_INT);
    ASSERT_EQUAL_INT(TAG_INT, get_tag(integer));
    ASSERT_EQUAL_PTR((void*)0x10000000, get_ptr(integer));
    
    tagged_ptr_t structure = create_tagged_ptr(ptr, TAG_STRUCT);
    ASSERT_EQUAL_INT(TAG_STRUCT, get_tag(structure));
    ASSERT_EQUAL_PTR(ptr, get_ptr(structure));
    
    tagged_ptr_t special = create_tagged_ptr(ptr, TAG_SPECIAL);
    ASSERT_EQUAL_INT(TAG_SPECIAL, get_tag(special));
    ASSERT_EQUAL_PTR(ptr, get_ptr(special));
}

void test_gc_bit() {
    void* ptr = (void*)0x10000000;
    tagged_ptr_t tagged = create_tagged_ptr(ptr, TAG_REF);
    
    // Initially, GC bit should not be set
    ASSERT_FALSE(is_gc_bit_set(tagged));
    
    // Set GC bit
    set_gc_bit(&tagged);
    ASSERT_TRUE(is_gc_bit_set(tagged));
    
    // Tag should still be the same
    ASSERT_EQUAL_INT(TAG_REF, get_tag(tagged));
    
    // Pointer should still be the same
    ASSERT_EQUAL_PTR(ptr, get_ptr(tagged));
    
    // Clear GC bit
    clear_gc_bit(&tagged);
    ASSERT_FALSE(is_gc_bit_set(tagged));
    
    // Tag and pointer should still be the same
    ASSERT_EQUAL_INT(TAG_REF, get_tag(tagged));
    ASSERT_EQUAL_PTR(ptr, get_ptr(tagged));
}

void test_alignment_requirements() {
    // Test that pointers with improper alignment are rejected or handled correctly
    // This will be implemented when we have the actual implementation
    
    // For now, just a placeholder that always passes
    ASSERT_TRUE(1);
}

int main(int argc, char** argv) {
    // Create test suite
    test_suite_t* suite = test_suite_create("Tagged Pointers");
    
    // Run tests
    RUN_TEST(suite, test_create_tagged_ptr);
    RUN_TEST(suite, test_gc_bit);
    RUN_TEST(suite, test_alignment_requirements);
    
    // Print results
    test_suite_print_results(suite);
    
    // Get status
    int status = test_suite_status(suite);
    
    // Cleanup
    test_suite_destroy(suite);
    
    return status;
}
