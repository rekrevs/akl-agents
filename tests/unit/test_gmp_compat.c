#include "test_framework.h"
#include "emulator/compat/gmp_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Test file for the GMP compatibility layer
 * 
 * This tests the compatibility between the old GMP API used in AKL
 * and the modern GMP library, verifying that our adapter layer works correctly.
 */

// Track memory allocations for testing
static size_t total_allocated = 0;
static size_t allocation_count = 0;
static size_t free_count = 0;

// Custom memory functions for testing
void* test_alloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        total_allocated += size;
        allocation_count++;
    }
    return ptr;
}

void* test_realloc(void* ptr, size_t old_size, size_t new_size) {
    void* new_ptr = realloc(ptr, new_size);
    if (new_ptr) {
        total_allocated = total_allocated - old_size + new_size;
    }
    return new_ptr;
}

void test_free(void* ptr, size_t size) {
    if (ptr) {
        free(ptr);
        total_allocated -= size;
        free_count++;
    }
}

// Test basic GMP operations
void test_basic_operations() {
    mpz_t a, b, result;
    
    // Initialize
    mpz_init_set_si(a, 123456789);
    mpz_init_set_si(b, 987654321);
    mpz_init(result);
    
    // Test addition
    mpz_add(result, a, b);
    char* str = mpz_get_str(NULL, 10, result);
    ASSERT_EQUAL_STR("1111111110", str);
    free(str);
    
    // Test subtraction
    mpz_sub(result, b, a);
    str = mpz_get_str(NULL, 10, result);
    ASSERT_EQUAL_STR("864197532", str);
    free(str);
    
    // Test multiplication
    mpz_mul(result, a, b);
    str = mpz_get_str(NULL, 10, result);
    ASSERT_EQUAL_STR("121932631112635269", str);
    free(str);
    
    // Test division
    mpz_tdiv_q(result, b, a);
    str = mpz_get_str(NULL, 10, result);
    ASSERT_EQUAL_STR("8", str);
    free(str);
    
    // Cleanup
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(result);
}

// Test the custom mpz_xor_kludge function
void test_xor_function() {
    mpz_t a, b, result;
    
    // Initialize
    mpz_init_set_si(a, 0x0F0F);  // 0000 1111 0000 1111
    mpz_init_set_si(b, 0x3333);  // 0011 0011 0011 0011
    mpz_init(result);
    
    // Test XOR operation
    mpz_xor_kludge(result, a, b);
    
    // Expected: 0x3C3C = 0011 1100 0011 1100
    ASSERT_EQUAL_INT(0x3C3C, mpz_get_si(result));
    
    // Cleanup
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(result);
}

// Test the custom mpz_mul_2exp_si function
void test_mul_2exp_si() {
    mpz_t a, result;
    
    // Initialize
    mpz_init_set_si(a, 42);
    mpz_init(result);
    
    // Test positive shift (left shift)
    mpz_mul_2exp_si(result, a, 3);  // 42 * 2^3 = 336
    ASSERT_EQUAL_INT(336, mpz_get_si(result));
    
    // Test negative shift (right shift)
    mpz_set_si(a, 336);
    mpz_mul_2exp_si(result, a, -3);  // 336 / 2^3 = 42
    ASSERT_EQUAL_INT(42, mpz_get_si(result));
    
    // Cleanup
    mpz_clear(a);
    mpz_clear(result);
}

// Test memory management
void test_memory_management() {
    // Set custom memory functions
    mp_set_memory_functions(test_alloc, test_realloc, test_free);
    
    // Reset counters
    total_allocated = 0;
    allocation_count = 0;
    free_count = 0;
    
    // Perform some GMP operations
    mpz_t a, b, c;
    mpz_init(a);
    mpz_init(b);
    mpz_init(c);
    
    mpz_set_si(a, 12345);
    mpz_set_si(b, 67890);
    mpz_add(c, a, b);
    
    // Check that memory was allocated
    ASSERT_TRUE(total_allocated > 0);
    ASSERT_TRUE(allocation_count > 0);
    
    // Clean up
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(c);
    
    // Check that memory was freed
    ASSERT_TRUE(free_count > 0);
}

// Test large numbers
void test_large_numbers() {
    mpz_t a, b, result;
    
    // Initialize with large values
    mpz_init(a);
    mpz_init(b);
    mpz_init(result);
    
    // Set a to 2^100
    mpz_set_ui(a, 1);
    mpz_mul_2exp(a, a, 100);
    
    // Set b to 2^50
    mpz_set_ui(b, 1);
    mpz_mul_2exp(b, b, 50);
    
    // Compute a / b = 2^50
    mpz_tdiv_q(result, a, b);
    
    // Convert to string and check
    char* str = mpz_get_str(NULL, 10, result);
    ASSERT_EQUAL_STR("1125899906842624", str);
    free(str);
    
    // Cleanup
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(result);
}

int main(int argc, char** argv) {
    // Create test suite
    test_suite_t* suite = test_suite_create("GMP Compatibility");
    
    // Run tests
    RUN_TEST(suite, test_basic_operations);
    RUN_TEST(suite, test_xor_function);
    RUN_TEST(suite, test_mul_2exp_si);
    RUN_TEST(suite, test_memory_management);
    RUN_TEST(suite, test_large_numbers);
    
    // Print results
    test_suite_print_results(suite);
    
    // Get status
    int status = test_suite_status(suite);
    
    // Cleanup
    test_suite_destroy(suite);
    
    return status;
}
