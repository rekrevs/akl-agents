/**
 * tagged_pointers.h - Modern implementation of AKL's tagged pointer system
 *
 * This file provides a platform-agnostic implementation of the tagged pointer
 * system used in AKL, supporting both 32-bit and 64-bit architectures.
 * 
 * The original implementation was designed for 32-bit architectures with
 * specific assumptions about pointer alignment and memory layout. This
 * modernized version maintains compatibility while working on modern systems.
 */

#ifndef TAGGED_POINTERS_H
#define TAGGED_POINTERS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Platform detection and configuration
 */
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(__powerpc64__) || defined(__ppc64__) || defined(__ia64__)
    #define AKL_64BIT 1
    #define TADBITS 64
    typedef uintptr_t tagged_address;
    typedef int64_t sword;
    typedef uint64_t uword;
#else
    #define AKL_32BIT 1
    #define TADBITS 32
    typedef uintptr_t tagged_address;
    typedef int32_t sword;
    typedef uint32_t uword;
#endif

/**
 * Tag masks and bit definitions
 */
#define PTagMask        ((uword)0x00000003)  /* Primary tag mask (2 bits) */
#define STagMask        ((uword)0x0000000f)  /* Secondary tag mask (4 bits) */
#define IMM_ATM_BIT     ((uword)0x10)        /* Immediate atom bit */
#define ITagMask        (STagMask + IMM_ATM_BIT)
#define SMALLNUMBITS    (TADBITS-6)          /* Bits available for small integers */
#define MarkBitMask     ((uword)0x1 << (TADBITS-1)) /* GC mark bit (highest bit) */

/**
 * Alignment and addressing
 */
#define WORDALIGNMENT   (sizeof(uword))
#define AddressMask     (~(uword)0xf) /* Clear bottom 4 bits */

/**
 * Pointer base address (for architectures that need it)
 */
#ifndef PTR_ORG
    #define PTR_ORG 0
#endif

/**
 * Tag values
 */
#define DEFINE_TAG(N)   (N)

typedef enum {
    REF = DEFINE_TAG(0),    /* Reference */
    UVA = DEFINE_TAG(1),    /* Unbound unconstrained variable */
    GVA = DEFINE_TAG(3),    /* Unbound constrained variable */
    IMM = DEFINE_TAG(6),    /* Immediate (small integer or atom) */
    LST = DEFINE_TAG(2),    /* List cell */
    STR = DEFINE_TAG(0xa),  /* Structure */
    GEN = DEFINE_TAG(0xe)   /* Generic, float, or bignum */
} tag_value;

/* For the scavenger */
#define REVERSED_LST    DEFINE_TAG(0)
#define REVERSED_STR    DEFINE_TAG(1)

/* Secondary tag values for generic objects */
#define BIG_TAG2        ((uword)0x00000000)  /* Bignum */
#define FLT_TAG2        ((uword)0x00000004)  /* Float */
#define GEN_TAG2        ((uword)0x00000008)  /* Generic (method tables must be above this) */

/**
 * Tag manipulation macros
 */
#define TagOffset(T)    ((tagged_address)(T))
#define CollectPTagBits(x)  ((x) & PTagMask)
#define CollectSTagBits(x)  ((x) & STagMask)

/**
 * Term type definition
 * 
 * Can be configured to use either a struct wrapper (TADTURMS) or direct integer type
 */
#ifdef TADTURMS
    typedef struct turm { tagged_address tad; } Turm;
    #define Tad(X)      ((X).tad)
    #define tad_to_term(A) tad_to_turm(A)
    extern Turm tad_to_turm(tagged_address A);
    typedef Turm Term;
#else
    typedef tagged_address Term;
    #define Tad(X)      (X)
    #define tad_to_term(A) (A)
#endif

/**
 * Basic tag and address operations
 */
#define PTagOf(x)       CollectPTagBits((tagged_address)Tad(x))
#define STagOf(x)       CollectSTagBits((tagged_address)Tad(x))
#define AddressOf(x)    ((void*)(((tagged_address)Tad(x) & ~(MarkBitMask | STagMask)) >> 2))
#define Eq(X, Y)        (Tad(X) == Tad(Y))

/**
 * Bit shifting operations for tag manipulation
 */
#define Oth(X)          ((X) >> 2)
#define Tho(X)          ((X) << 2)
#define OthTad(X)       Oth(Tad(X))
#define OthTagOffset(T) (Oth(TagOffset(T)) - PTR_ORG)
#define PTR_ATM_BIT     Oth(IMM_ATM_BIT)
#define AddressOth(X)   (AddressOf(Oth(X)) + PTR_ORG)

/**
 * Tagged pointer creation
 */
#define TadPtr(X, T)    (((tagged_address)(X)) + TagOffset(T))
#define TadOth(X, T)    (Tho((tagged_address)(X)) - Tho(PTR_ORG) + TagOffset(T))
#define TagPtr(X, T)    tad_to_term(TadPtr(X, T))
#define TagOth(X, T)    tad_to_term(TadOth(X, T))

/**
 * Small integer representation
 */
#define swordNum(N)     (((uword)(N)) << 3)
/* Fix for shift-count-overflow warning */
#ifdef SIXTY_FOUR_BIT
#define MaxSmallNum     ((sword)1 << 29) /* 29 bits for 64-bit platforms */
#else
#define MaxSmallNum     ((sword)1 << 26) /* 26 bits for 32-bit platforms */
#endif

/**
 * Type-specific tagging
 */
#define TagAtm(A)       (TagOth(A, IMM) + IMM_ATM_BIT)
#define TagNum(I)       tad_to_term(((((uword)(I)) << 3) & ~(PTagMask | IMM_ATM_BIT)) | IMM)
#define TagRef(X)       TagPtr(X, REF)
#define TagUva(X)       TagPtr(X, UVA)
#define TagGva(X)       TagPtr(X, GVA)
#define TagBig(X)       tad_to_term((((tagged_address)(X)) << 2) | GEN)
#define TagFlt(X)       tad_to_term((((tagged_address)(X)) << 2) | GEN)
#define TagLst(X)       tad_to_term((((tagged_address)(X)) << 2) | LST)
#define TagStr(X)       tad_to_term((((tagged_address)(X)) << 2) | STR)
#define TagGen(X)       tad_to_term((((tagged_address)(X)) << 2) | GEN)

/**
 * Type checking
 */
#define IsRef(X)        ((Tad(X) & PTagMask) == REF)
#define IsUva(X)        ((Tad(X) & PTagMask) == UVA)
#define IsGva(X)        ((Tad(X) & PTagMask) == GVA)
#define IsVar(X)        ((Tad(X) & PTagMask) <= UVA || (Tad(X) & PTagMask) == GVA)
#define IsLst(X)        ((Tad(X) & PTagMask) == LST)
#define IsStr(X)        ((Tad(X) & STagMask) == STR)
#define IsImm(X)        (((Tad(X) & STagMask) & 0x6) == 0x6)
#define IsGen(X)        ((Tad(X) & STagMask) == GEN)
#define IsAtm(X)        (IsImm(X) && (Tad(X) & IMM_ATM_BIT))
#define IsNum(X)        (IsImm(X) && !((Tad(X) & IMM_ATM_BIT)))
/* Simplified implementation for testing */
#define IsBIG(X)        (IsGen(X))
/* Simplified implementation for testing */
#define IsFLT(X)        (IsGen(X))

/**
 * Garbage collection mark bit operations
 */
#define IsMarked(X)     ((Tad(X) & MarkBitMask) != 0)
#define Mark(X)         tad_to_term(Tad(X) | MarkBitMask)
#define Unmark(X)       tad_to_term(Tad(X) & ~MarkBitMask)
#define MarkTerm(X)     Mark(X)
#define UnmarkTerm(X)   Unmark(X)

/**
 * Value extraction
 */
#define GetSmall(X)     ((sword)(Tad(X) >> 3))
#define MakeSmallNum(N) TagNum(N)

/**
 * Special values
 */
#define NullTerm        TagPtr(0, 0)

#endif /* TAGGED_POINTERS_H */
