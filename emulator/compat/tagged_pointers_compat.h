/**
 * tagged_pointers_compat.h - Compatibility layer for tagged pointers
 *
 * This file provides a compatibility layer between the original AKL tagged pointer
 * implementation and the modernized version. It allows for a gradual transition
 * by providing macros and functions that work with both implementations.
 */

#ifndef TAGGED_POINTERS_COMPAT_H
#define TAGGED_POINTERS_COMPAT_H

// Determine which implementation to use
#ifdef USE_MODERN_TAGGED_POINTERS
    #include "tagged_pointers.h"
#else
    #include "../term.h"
#endif

/**
 * Platform-agnostic type definitions
 * 
 * These ensure consistent types regardless of which implementation is used
 */
#include <stdint.h>
#include <stdbool.h>

#ifndef AKL_PLATFORM_TYPES_DEFINED
#define AKL_PLATFORM_TYPES_DEFINED

// Standard integer types that should be used for new code
typedef int32_t  akl_int32_t;
typedef uint32_t akl_uint32_t;
typedef int64_t  akl_int64_t;
typedef uint64_t akl_uint64_t;
typedef intptr_t akl_intptr_t;
typedef uintptr_t akl_uintptr_t;

// Platform detection
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(__powerpc64__) || defined(__ppc64__) || defined(__ia64__)
    #define AKL_64BIT 1
#else
    #define AKL_32BIT 1
#endif

#endif /* AKL_PLATFORM_TYPES_DEFINED */

/**
 * Helper functions for platform-agnostic code
 */

/**
 * Check if a value can be represented as a small integer
 * 
 * @param value The integer value to check
 * @return true if the value can be represented as a small integer, false otherwise
 */
static inline bool akl_can_be_small_int(akl_int64_t value) {
    return (value >= -MaxSmallNum && value < MaxSmallNum);
}

/**
 * Create a Term from an integer value
 * 
 * This function handles both small integers and bignums transparently.
 * For small integers, it creates a tagged small integer.
 * For larger integers, it creates a bignum (implementation depends on context).
 * 
 * @param value The integer value to convert
 * @return A Term representing the integer value
 */
Term akl_int_to_term(akl_int64_t value);

/**
 * Get the integer value from a Term
 * 
 * This function handles both small integers and bignums transparently.
 * 
 * @param term The Term to convert
 * @return The integer value
 */
akl_int64_t akl_term_to_int(Term term);

/**
 * Check if a Term is an integer (either small or bignum)
 * 
 * @param term The Term to check
 * @return true if the Term is an integer, false otherwise
 */
static inline bool akl_is_integer(Term term) {
    return IsNum(term) || IsBIG(term);
}

/**
 * Debugging helper to print information about a Term
 * 
 * @param term The Term to inspect
 */
void akl_debug_term(Term term);

#endif /* TAGGED_POINTERS_COMPAT_H */
