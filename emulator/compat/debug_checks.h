/**
 * debug_checks.h - Debug verification functions for AKL 64-bit implementation
 *
 * This file provides various debug-only verification functions to ensure
 * the correctness of operations in the 64-bit tagged pointer system.
 */

#ifndef DEBUG_CHECKS_H
#define DEBUG_CHECKS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Verify pointer alignment for 64-bit tagging
 * 
 * In debug builds, this function checks that a pointer is properly aligned
 * for the 64-bit tagged pointer system. If not, it prints an error message
 * and terminates the program.
 *
 * @param ptr The pointer to check
 * @param file The source file name where the check is performed
 * @param line The line number where the check is performed
 */
#ifdef DEBUG
#define VERIFY_POINTER_ALIGNMENT(ptr) verify_pointer_alignment_64bit((void*)(ptr), __FILE__, __LINE__)

static inline void verify_pointer_alignment_64bit(void* ptr, const char* file, int line) {
    if (((uintptr_t)ptr & 0x7) != 0) {
        fprintf(stderr, "ERROR: Misaligned pointer 0x%lx at %s:%d\n", 
                (unsigned long)ptr, file, line);
        fprintf(stderr, "64-bit tagged pointers require 8-byte alignment\n");
        abort();
    }
}

#else
#define VERIFY_POINTER_ALIGNMENT(ptr) ((void)0)
#endif /* DEBUG */

/**
 * Verify small integer range
 * 
 * In debug builds, this function checks that an integer is within the valid range
 * for small integers in the 64-bit implementation.
 *
 * @param num The integer to check
 * @param file The source file name where the check is performed
 * @param line The line number where the check is performed
 */
#ifdef DEBUG
#define VERIFY_SMALL_INT_RANGE(num) verify_small_int_range_64bit((num), __FILE__, __LINE__)

static inline void verify_small_int_range_64bit(int64_t num, const char* file, int line) {
    static const int64_t MIN_SMALL_INT = -(1LL << 59);
    static const int64_t MAX_SMALL_INT = (1LL << 59) - 1;
    
    if (num < MIN_SMALL_INT || num > MAX_SMALL_INT) {
        fprintf(stderr, "ERROR: Integer %lld outside small integer range at %s:%d\n", 
                (long long)num, file, line);
        fprintf(stderr, "Valid range: %lld to %lld\n", 
                (long long)MIN_SMALL_INT, (long long)MAX_SMALL_INT);
        abort();
    }
}

#else
#define VERIFY_SMALL_INT_RANGE(num) ((void)0)
#endif /* DEBUG */

/**
 * Verify consistent implementation usage
 * 
 * In debug builds, this function checks that the correct implementation
 * (32-bit or 64-bit) is being used based on the configuration settings.
 */
#ifdef DEBUG
#define VERIFY_IMPLEMENTATION_CONSISTENCY() verify_implementation_consistency(__FILE__, __LINE__)

static inline void verify_implementation_consistency(const char* file, int line) {
#if defined(AKL_64BIT) && !defined(USE_MODERN_TAGGED_POINTERS)
    fprintf(stderr, "ERROR: 64-bit platform detected but modern tagged pointers not enabled at %s:%d\n",
            file, line);
    abort();
#elif defined(AKL_32BIT) && defined(USE_MODERN_TAGGED_POINTERS)
    fprintf(stderr, "ERROR: Modern tagged pointers enabled on 32-bit platform at %s:%d\n",
            file, line);
    fprintf(stderr, "This configuration is not yet supported\n");
    abort();
#endif
}

#else
#define VERIFY_IMPLEMENTATION_CONSISTENCY() ((void)0)
#endif /* DEBUG */

#endif /* DEBUG_CHECKS_H */
