/**
 * tagged_pointers_64bit.h - Modern 64-bit implementation of AKL's tagged pointer system
 *
 * This file provides a clean 64-bit implementation of the tagged pointer system used in AKL.
 * Following the strategy outlined in INSIGHTS.md, this implementation:
 * 
 * 1. Assumes 64-bit architecture exclusively
 * 2. Uses the lowest bit (bit 0) for garbage collection marking
 * 3. Uses bits 1-3 for type tags (shifted up by 1 from original spec)
 * 4. Assumes 8-byte alignment for all pointers
 * 5. Provides a simpler, more consistent API for tag manipulation
 */

#ifndef TAGGED_POINTERS_64BIT_H
#define TAGGED_POINTERS_64BIT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Basic type definitions for 64-bit architecture
 */
typedef uintptr_t tagged_address;  /* 64-bit tagged address */
typedef int64_t sword;             /* Signed word (64-bit) */
typedef uint64_t uword;            /* Unsigned word (64-bit) */

#define TADBITS 64                 /* Number of bits in a tagged address */
#define SMALLNUMBITS 60            /* Bits available for small integers */

/**
 * Tag masks and bit definitions
 */
#define GC_BIT          ((uword)0x1)       /* GC mark bit (bit 0) */
#define TAG_BITS        ((uword)0xE)       /* Primary tag bits (bits 1-3) */
#define EXTENDED_BITS   ((uword)0x1E)      /* Extended tag bits (bits 1-4) */
#define IMM_ATM_BIT     ((uword)0x10)      /* Immediate atom bit (bit 4) */
#define SEC_TAG_BITS    ((uword)0x60)      /* Secondary tag bits (bits 5-6) for GEN types */
#define POINTER_MASK    (~(uword)0x7F)     /* Mask to clear all tag bits (bits 0-6) */

/* Combining masks for easier checking */
#define NUM_MASK        (TAG_BITS | IMM_ATM_BIT)  /* Combined mask for number check */
#define NUM_PATTERN     (IMM)              /* Pattern to match for numbers */
#define ATM_MASK        (TAG_BITS | IMM_ATM_BIT)  /* Combined mask for atom check */
#define ATM_PATTERN     (IMM | IMM_ATM_BIT)  /* Pattern to match for atoms */

/**
 * Tag values (shifted up by 1 bit from original spec)
 */
typedef enum {
    REF = 0x0,      /* Reference (000 in bits 1-3) */
    UVA = 0x2,      /* Unbound unconstrained variable (001 in bits 1-3) */
    LST = 0x4,      /* List cell (010 in bits 1-3) */
    GVA = 0x6,      /* Unbound constrained variable (011 in bits 1-3) */
    IMM = 0x8,      /* Immediate (100 in bits 1-3) */
    STR = 0xC,      /* Structure (110 in bits 1-3) */
    GEN = 0xE       /* Generic, float, or bignum (111 in bits 1-3) */
} tag_value;

/* Secondary tag values for generic objects - positioned in bits 5-6 */
#define BIG_TAG2        ((uword)0x00)      /* Bignum (00 in secondary bits) */
#define FLT_TAG2        ((uword)0x20)      /* Float (01 in secondary bits) */
#define GEN_TAG2        ((uword)0x40)      /* Generic (10 in secondary bits) */

/**
 * Term type definition
 */
typedef tagged_address Term;

/**
 * Basic tag and address operations
 */
#define Tad(X)          (X)                /* Identity function for Term access */
#define tad_to_term(A)  (A)                /* Identity function for Term conversion */

/* Tag extraction */
#define TagOf(X)        ((X) & TAG_BITS)   /* Extract primary tag bits */
#define ExtendedTagOf(X) ((X) & EXTENDED_BITS) /* Extract extended tag bits */

/* Address extraction (clear tags and shift by 5 to restore actual address) */
#define AddressOf(X)    ((void*)(((X) & POINTER_MASK) >> 5))

/* Term comparison */
#define Eq(X, Y)        ((X) == (Y))

/**
 * Small integer representation
 */
/* Calculated constants for small integer range based on SMALLNUMBITS */
#define MaxSmallInt ((sword)((((sword)1) << (SMALLNUMBITS - 1)) - 1))  /* Maximum small integer value */
#define MinSmallInt (-((sword)1 << (SMALLNUMBITS - 1)))  /* Minimum small integer value */

/* Legacy constants (kept for backward compatibility) */
#define MaxSmallNum     ((sword)288230376151711743)   /* Maximum small integer value (2^58-1) */
#define MinSmallNum     ((sword)-288230376151711744)  /* Minimum small integer value (-2^58) */

/**
 * Function to check if an integer value can be represented as a small integer
 * This verifies the value is within the range of our tagged small integers
 */
static inline bool IsSmallInt(int64_t num) {
    return (num >= MinSmallInt && num <= MaxSmallInt);
}

/* Integer encoding/decoding */
#define TagNum(I)       (((uword)(I) << 5) | IMM) /* Tag small integer - shifted by 5 to clear all tag bits */

/* Extract the integer value, preserving sign */
/* 
 * Extract the small integer value from a tagged pointer
 * First clear the bottom 5 bits (tag bits), then right-shift by 5
 * Cast to sword (signed word) before shifting to ensure sign extension for negative values
 */
#define GetSmall(X)     (((sword)((X) & ~(uword)0x1F)) >> 5)
#define MakeSmallNum(N) TagNum(N) /* Alias for TagNum */

/**
 * Type-specific tagging macros
 */
/* 
 * Pointer tagging (shift by 5 to make room for all tags).
 * This ensures that pointers are aligned properly before tagging.
 */
#define TagPtr(X, T)    (((uintptr_t)(X) << 5) | (T))

/* Type-specific tagging */
#define TagRef(X)       TagPtr(X, REF)
#define TagUva(X)       TagPtr(X, UVA)
#define TagGva(X)       TagPtr(X, GVA)
#define TagLst(X)       TagPtr(X, LST)
#define TagStr(X)       TagPtr(X, STR)
#define TagGen(X)       TagPtr(X, GEN)
#define TagAtm(A)       (((uword)(A) << 5) | IMM | IMM_ATM_BIT) /* Ensure atom bit is set */

/* Special tags */
#define TagBig(X)       (TagPtr(X, GEN) | BIG_TAG2) /* Bignum with proper tag */
#define TagFlt(X)       (TagPtr(X, GEN) | FLT_TAG2) /* Float with proper tag */

/**
 * Type checking macros
 */
#define IsRef(X)        (TagOf(X) == REF)
#define IsUva(X)        (TagOf(X) == UVA)
#define IsGva(X)        (TagOf(X) == GVA)
#define IsLst(X)        (TagOf(X) == LST)
#define IsStr(X)        (TagOf(X) == STR)
#define IsGen(X)        (TagOf(X) == GEN)
#define IsImm(X)        (TagOf(X) == IMM)
/* Improved number and atom checks using explicit patterns */
#define IsNum(X)        (((X) & NUM_MASK) == NUM_PATTERN)
#define IsAtm(X)        (((X) & ATM_MASK) == ATM_PATTERN)
#define IsVar(X)        (IsUva(X) || IsGva(X) || IsRef(X))

/* Generic object type checking */
#define IsBIG(X)        (IsGen(X) && (((X) & SEC_TAG_BITS) == BIG_TAG2))
#define IsFLT(X)        (IsGen(X) && (((X) & SEC_TAG_BITS) == FLT_TAG2))
#define IsGEN(X)        (IsGen(X) && (((X) & SEC_TAG_BITS) == GEN_TAG2))

/**
 * Garbage collection mark bit operations
 */
#define IsMarked(X)     ((X) & GC_BIT)
#define Mark(X)         ((X) | GC_BIT)
#define Unmark(X)       ((X) & ~GC_BIT)
#define MarkTerm(X)     Mark(X)
#define UnmarkTerm(X)   Unmark(X)

/**
 * Special values
 */
#define NullTerm        0UL

/**
 * Debugging helpers
 */
#ifdef AKL_DEBUG
#include <stdio.h>

/* Print tag information */
static inline void print_tag_info(Term t) {
    printf("Term: 0x%016lx\n", (unsigned long)t);
    printf("  GC bit: %d\n", (t & GC_BIT) ? 1 : 0);
    printf("  Tag: 0x%lx\n", (unsigned long)TagOf(t));
    printf("  Extended tag: 0x%lx\n", (unsigned long)ExtendedTagOf(t));
    printf("  IsRef: %d\n", IsRef(t));
    printf("  IsUva: %d\n", IsUva(t));
    printf("  IsGva: %d\n", IsGva(t));
    printf("  IsLst: %d\n", IsLst(t));
    printf("  IsStr: %d\n", IsStr(t));
    printf("  IsGen: %d\n", IsGen(t));
    printf("  IsImm: %d\n", IsImm(t));
    printf("  IsNum: %d\n", IsNum(t));
    printf("  IsAtm: %d\n", IsAtm(t));
    if (IsNum(t)) {
        printf("  Value: %lld\n", (long long)GetSmall(t));
    }
    if (!IsImm(t)) {
        printf("  Address: %p\n", AddressOf(t));
    }
}
#endif /* AKL_DEBUG */

#endif /* TAGGED_POINTERS_64BIT_H */
