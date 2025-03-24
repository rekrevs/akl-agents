#include "test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables for current test
test_suite_t* current_test_suite = NULL;
const char* current_test_name = NULL;

// Initialize a test suite
test_suite_t* test_suite_create(const char* name) {
    test_suite_t* suite = (test_suite_t*)malloc(sizeof(test_suite_t));
    if (!suite) {
        fprintf(stderr, "Failed to allocate memory for test suite\n");
        exit(EXIT_FAILURE);
    }
    
    suite->name = name;
    suite->capacity = 32;  // Initial capacity
    suite->count = 0;
    suite->passed = 0;
    suite->failed = 0;
    
    suite->results = (test_result_t*)malloc(suite->capacity * sizeof(test_result_t));
    if (!suite->results) {
        fprintf(stderr, "Failed to allocate memory for test results\n");
        free(suite);
        exit(EXIT_FAILURE);
    }
    
    return suite;
}

// Free a test suite
void test_suite_destroy(test_suite_t* suite) {
    if (!suite) return;
    
    // Free messages in results
    for (int i = 0; i < suite->count; i++) {
        free(suite->results[i].message);
    }
    
    free(suite->results);
    free(suite);
}

// Add a test result to the suite
static void add_result(test_suite_t* suite, const char* name, bool passed, const char* message) {
    // Resize results array if needed
    if (suite->count >= suite->capacity) {
        suite->capacity *= 2;
        suite->results = (test_result_t*)realloc(suite->results, 
                                               suite->capacity * sizeof(test_result_t));
        if (!suite->results) {
            fprintf(stderr, "Failed to reallocate memory for test results\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Add the result
    test_result_t* result = &suite->results[suite->count++];
    result->name = name;
    result->passed = passed;
    
    // Copy the message
    if (message) {
        result->message = strdup(message);
    } else {
        result->message = NULL;
    }
    
    // Update counters
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

// Run a test and record the result
void test_suite_run(test_suite_t* suite, const char* name, test_func_t test_func) {
    // Set global variables for assertions
    current_test_suite = suite;
    current_test_name = name;
    
    printf("Running test: %s\n", name);
    
    // Run the test
    test_func();
    
    // Reset global variables
    current_test_suite = NULL;
    current_test_name = NULL;
}

// Assert functions
void assert_true(test_suite_t* suite, bool condition, const char* message) {
    if (!condition) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Assertion failed: %s is not true", message);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_false(test_suite_t* suite, bool condition, const char* message) {
    if (condition) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Assertion failed: %s is not false", message);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_equal_int(test_suite_t* suite, int expected, int actual, const char* message) {
    if (expected != actual) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Assertion failed: %s (expected %d, got %d)", 
                message, expected, actual);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_equal_ptr(test_suite_t* suite, const void* expected, const void* actual, const char* message) {
    if (expected != actual) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Assertion failed: %s (expected %p, got %p)", 
                message, expected, actual);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_not_equal_ptr(test_suite_t* suite, const void* not_expected, const void* actual, const char* message) {
    if (not_expected == actual) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Assertion failed: %s (got %p, which should not equal %p)", 
                message, actual, not_expected);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_equal_str(test_suite_t* suite, const char* expected, const char* actual, const char* message) {
    if (strcmp(expected, actual) != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Assertion failed: %s (expected \"%s\", got \"%s\")", 
                message, expected, actual);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_null(test_suite_t* suite, const void* ptr, const char* message) {
    if (ptr != NULL) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Assertion failed: %s (expected NULL, got %p)", 
                message, ptr);
        add_result(suite, current_test_name, false, error_msg);
    }
}

void assert_not_null(test_suite_t* suite, const void* ptr, const char* message) {
    if (ptr == NULL) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Assertion failed: %s (got NULL)", message);
        add_result(suite, current_test_name, false, error_msg);
    }
}

// Print test results
void test_suite_print_results(test_suite_t* suite) {
    printf("\n=== Test Suite: %s ===\n", suite->name);
    printf("Total tests: %d, Passed: %d, Failed: %d\n", 
           suite->count, suite->passed, suite->failed);
    
    if (suite->failed > 0) {
        printf("\nFailed tests:\n");
        for (int i = 0; i < suite->count; i++) {
            test_result_t* result = &suite->results[i];
            if (!result->passed) {
                printf("  - %s: %s\n", result->name, result->message);
            }
        }
    }
    
    printf("\n");
}

// Get overall test status (0 = success, 1 = failure)
int test_suite_status(test_suite_t* suite) {
    return (suite->failed > 0) ? 1 : 0;
}
