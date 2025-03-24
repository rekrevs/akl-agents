#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Simple test framework for AKL modernization project
 * 
 * This provides basic functionality for unit testing individual components
 * as they are modernized.
 */

// Test function type
typedef void (*test_func_t)(void);

// Test result structure
typedef struct {
    const char* name;
    bool passed;
    char* message;
} test_result_t;

// Test suite structure
typedef struct {
    const char* name;
    test_result_t* results;
    int count;
    int capacity;
    int passed;
    int failed;
} test_suite_t;

// Initialize a test suite
test_suite_t* test_suite_create(const char* name);

// Free a test suite
void test_suite_destroy(test_suite_t* suite);

// Run a test and record the result
void test_suite_run(test_suite_t* suite, const char* name, test_func_t test_func);

// Assert functions
void assert_true(test_suite_t* suite, bool condition, const char* message);
void assert_false(test_suite_t* suite, bool condition, const char* message);
void assert_equal_int(test_suite_t* suite, int expected, int actual, const char* message);
void assert_equal_ptr(test_suite_t* suite, const void* expected, const void* actual, const char* message);
void assert_not_equal_ptr(test_suite_t* suite, const void* not_expected, const void* actual, const char* message);
void assert_equal_str(test_suite_t* suite, const char* expected, const char* actual, const char* message);
void assert_null(test_suite_t* suite, const void* ptr, const char* message);
void assert_not_null(test_suite_t* suite, const void* ptr, const char* message);

// Print test results
void test_suite_print_results(test_suite_t* suite);

// Get overall test status (0 = success, 1 = failure)
int test_suite_status(test_suite_t* suite);

// Current test suite (used by assert macros)
extern test_suite_t* current_test_suite;
extern const char* current_test_name;

// Convenience macros for assertions
#define ASSERT_TRUE(condition) \
    assert_true(current_test_suite, (condition), #condition)

#define ASSERT_FALSE(condition) \
    assert_false(current_test_suite, (condition), #condition)

#define ASSERT_EQUAL_INT(expected, actual) \
    assert_equal_int(current_test_suite, (expected), (actual), #actual " == " #expected)

#define ASSERT_EQUAL_PTR(expected, actual) \
    assert_equal_ptr(current_test_suite, (expected), (actual), #actual " == " #expected)

#define ASSERT_NOT_EQUAL_PTR(not_expected, actual) \
    assert_not_equal_ptr(current_test_suite, (not_expected), (actual), #actual " != " #not_expected)

#define ASSERT_EQUAL_STR(expected, actual) \
    assert_equal_str(current_test_suite, (expected), (actual), #actual " == " #expected)

#define ASSERT_NULL(ptr) \
    assert_null(current_test_suite, (ptr), #ptr " == NULL")

#define ASSERT_NOT_NULL(ptr) \
    assert_not_null(current_test_suite, (ptr), #ptr " != NULL")

// Main test runner macro
#define RUN_TEST(suite, test_func) \
    test_suite_run(suite, #test_func, test_func)

#endif /* TEST_FRAMEWORK_H */
