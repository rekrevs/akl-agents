/**
 * gmp_compat.h - Compatibility layer for GMP library
 *
 * This file provides compatibility between the old GMP version bundled with AKL
 * and modern GMP versions. It defines macros, type aliases, and functions to
 * bridge API differences.
 */

#ifndef GMP_COMPAT_H
#define GMP_COMPAT_H

#include <gmp.h>
#include <stdlib.h>

/**
 * Type compatibility
 * 
 * The old GMP used MP_INT, while modern GMP uses mpz_t
 */
#ifndef MP_INT_DEFINED
#define MP_INT_DEFINED
typedef mpz_t MP_INT;
#endif

/**
 * Memory management functions
 *
 * The AKL implementation provides custom memory management for GMP.
 * These functions adapt the old interface to the modern GMP memory interface.
 */

/* Function prototypes for AKL's custom memory allocators */
extern void *bignum_alloc(size_t size);
extern void *bignum_realloc(void *ptr, size_t old_size, size_t new_size);
extern void bignum_dealloc(void *ptr, size_t size);

extern void *bignum_const_alloc(size_t size);
extern void *bignum_const_realloc(void *ptr, size_t old_size, size_t new_size);
extern void bignum_const_dealloc(void *ptr, size_t size);

/**
 * Initialize GMP with custom memory functions
 * 
 * This should be called during AKL initialization before any GMP operations.
 */
static inline void gmp_compat_init(void) {
    /* Set custom memory functions for GMP */
    mp_set_memory_functions(
        (void *(*)(size_t))bignum_alloc,
        (void *(*)(void *, size_t, size_t))bignum_realloc,
        (void (*)(void *, size_t))bignum_dealloc
    );
}

/**
 * Custom functions not in standard GMP
 */

/**
 * mpz_xor_kludge - Bitwise XOR operation
 * 
 * This was a custom function in the old GMP implementation.
 * Modern GMP provides mpz_xor, so we just alias it.
 */
#define mpz_xor_kludge mpz_xor

/**
 * mpz_mul_2exp_si - Multiply by a power of 2
 * 
 * This was a custom function in the old GMP implementation.
 * Modern GMP provides mpz_mul_2exp, but it takes unsigned long.
 */
static inline void mpz_mul_2exp_si(mpz_t rop, mpz_t op1, long int op2) {
    if (op2 >= 0) {
        mpz_mul_2exp(rop, op1, (unsigned long)op2);
    } else {
        /* Handle negative shift (right shift) */
        mpz_tdiv_q_2exp(rop, op1, (unsigned long)(-op2));
    }
}

/**
 * Function name compatibility macros
 * 
 * Map old function names to their modern equivalents where needed.
 */

/* Add any function name mappings here if needed */

#endif /* GMP_COMPAT_H */
