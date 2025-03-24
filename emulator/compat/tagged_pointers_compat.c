/**
 * tagged_pointers_compat.c - Implementation of tagged pointer compatibility functions
 *
 * This file implements the functions declared in tagged_pointers_compat.h,
 * providing a compatibility layer between the original and modernized
 * tagged pointer implementations.
 */

#include "tagged_pointers_compat.h"
#include <stdio.h>

#ifdef USE_MODERN_TAGGED_POINTERS
#include "tagged_pointers.h"
#else
#include "../term.h"
#endif

// Forward declarations for functions we need from other modules
extern Term canonize_bignum(void* b);
extern void* make_bignum(akl_int64_t value);
extern akl_int64_t get_bignum_integer(Term i);

/**
 * Create a Term from an integer value
 * 
 * This function handles both small integers and bignums transparently.
 * For small integers, it creates a tagged small integer.
 * For larger integers, it creates a bignum.
 * 
 * @param value The integer value to convert
 * @return A Term representing the integer value
 */
Term akl_int_to_term(akl_int64_t value) {
    // Check if the value can be represented as a small integer
    if (akl_can_be_small_int(value)) {
        return MakeSmallNum((sword)value);
    } else {
        // Create a bignum
        void* bn = make_bignum(value);
        return canonize_bignum(bn);
    }
}

/**
 * Get the integer value from a Term
 * 
 * This function handles both small integers and bignums transparently.
 * 
 * @param term The Term to convert
 * @return The integer value
 */
akl_int64_t akl_term_to_int(Term term) {
    if (IsNum(term)) {
        return (akl_int64_t)GetSmall(term);
    } else if (IsBIG(term)) {
        return get_bignum_integer(term);
    } else {
        // Not an integer term
        fprintf(stderr, "Error: akl_term_to_int called on non-integer term\n");
        return 0;
    }
}

/**
 * Debugging helper to print information about a Term
 * 
 * @param term The Term to inspect
 */
void akl_debug_term(Term term) {
    printf("Term debug info:\n");
    printf("  Raw value: 0x%lx\n", (unsigned long)Tad(term));
    printf("  Primary tag: 0x%lx\n", (unsigned long)PTagOf(term));
    printf("  Secondary tag: 0x%lx\n", (unsigned long)STagOf(term));
    printf("  Address: 0x%lx\n", (unsigned long)AddressOf(term));
    printf("  Is marked: %s\n", IsMarked(term) ? "yes" : "no");
    
    // Type-specific information
    if (IsRef(term)) {
        printf("  Type: Reference\n");
    } else if (IsUva(term)) {
        printf("  Type: Unbound unconstrained variable\n");
    } else if (IsGva(term)) {
        printf("  Type: Unbound constrained variable\n");
    } else if (IsNum(term)) {
        printf("  Type: Small integer\n");
        printf("  Value: %ld\n", (long)GetSmall(term));
    } else if (IsAtm(term)) {
        printf("  Type: Atom\n");
    } else if (IsLst(term)) {
        printf("  Type: List cell\n");
    } else if (IsStr(term)) {
        printf("  Type: Structure\n");
    } else if (IsBIG(term)) {
        printf("  Type: Bignum\n");
    } else if (IsFLT(term)) {
        printf("  Type: Float\n");
    } else if (IsGen(term)) {
        printf("  Type: Generic\n");
    } else {
        printf("  Type: Unknown\n");
    }
}
