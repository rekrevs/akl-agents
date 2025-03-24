/**
 * test_tagged_pointers_64bit_comprehensive.c - Comprehensive test suite for 64-bit tagged pointers
 *
 * This file contains an extensive test suite that validates all aspects of the
 * 64-bit tagged pointer implementation, including:
 * - Tag bit manipulation
 * - Small integer representation and extraction
 * - Address extraction
 * - Type checking
 * - GC mark bit operations
 * - Edge cases and boundary conditions
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <limits.h>

/* Include the 64-bit tagged pointer implementation */
#include "../../emulator/compat/tagged_pointers_64bit.h"

/* Test framework macros */
#define TEST_SECTION(name) printf("\nTesting %s...\n", name)
#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            printf("Assertion failed: %s (line %d)\n", #expr, __LINE__); \
            exit(1); \
        } \
    } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQUAL_INT(expected, actual) \
    do { \
        sword exp = (expected); \
        sword act = (actual); \
        if (exp != act) { \
            printf("Assertion failed: %s == %s (expected %" PRId64 ", got %" PRId64 ")\n", \
                   #expected, #actual, exp, act); \
            exit(1); \
        } \
    } while (0)

#define ASSERT_EQUAL_ADDR(expected, actual) \
    do { \
        void* exp = (expected); \
        void* act = (actual); \
        if (exp != act) { \
            printf("Assertion failed: %s == %s (expected %p, got %p)\n", \
                   #expected, #actual, exp, act); \
            exit(1); \
        } \
    } while (0)

#define ASSERT_EQUAL_UWORD(expected, actual) \
    do { \
        uword exp = (expected); \
        uword act = (actual); \
        if (exp != act) { \
            printf("Assertion failed: %s == %s (expected 0x%" PRIx64 ", got 0x%" PRIx64 ")\n", \
                   #expected, #actual, exp, act); \
            exit(1); \
        } \
    } while (0)

/**
 * Test basic tag operations 
 */
void test_tag_operations() {
    TEST_SECTION("basic tag operations");
    
    /* Test primary tag checking */
    Term ref_term = REF;
    Term uva_term = UVA;
    Term lst_term = LST;
    Term str_term = STR;
    Term imm_term = IMM;
    Term gen_term = GEN;
    
    ASSERT_EQUAL_UWORD(REF, TagOf(ref_term));
    ASSERT_EQUAL_UWORD(UVA, TagOf(uva_term));
    ASSERT_EQUAL_UWORD(LST, TagOf(lst_term));
    ASSERT_EQUAL_UWORD(STR, TagOf(str_term));
    ASSERT_EQUAL_UWORD(IMM, TagOf(imm_term));
    ASSERT_EQUAL_UWORD(GEN, TagOf(gen_term));
    
    printf("Primary tag checking: PASSED\n");
    
    /* Test tag checking predicates */
    ASSERT_TRUE(IsRef(ref_term));
    ASSERT_TRUE(IsUva(uva_term));
    ASSERT_TRUE(IsLst(lst_term));
    ASSERT_TRUE(IsStr(str_term));
    ASSERT_TRUE(IsImm(imm_term));
    ASSERT_TRUE(IsGen(gen_term));
    
    /* Test negative cases (should be false) */
    ASSERT_FALSE(IsRef(uva_term));
    ASSERT_FALSE(IsUva(lst_term));
    ASSERT_FALSE(IsLst(str_term));
    ASSERT_FALSE(IsStr(imm_term));
    ASSERT_FALSE(IsImm(gen_term));
    ASSERT_FALSE(IsGen(ref_term));
    
    printf("Tag predicate checking: PASSED\n");
}

/**
 * Test small integer representation
 */
void test_small_integers() {
    TEST_SECTION("small integer representation");
    
    /* Test basic integer tagging and untagging */
    sword int_values[] = {0, 1, -1, 100, -100, 1000, -1000, 1000000, -1000000};
    int num_values = sizeof(int_values) / sizeof(int_values[0]);
    
    for (int i = 0; i < num_values; i++) {
        sword value = int_values[i];
        Term tagged = TagNum(value);
        
        printf("TagNum(%" PRId64 ") = 0x%lx, IsNum=%d, IsAtm=%d, TagOf=0x%lx\n",
               value, tagged, IsNum(tagged), IsAtm(tagged), TagOf(tagged));
        
        ASSERT_TRUE(IsNum(tagged));
        ASSERT_FALSE(IsAtm(tagged));
        ASSERT_EQUAL_UWORD(IMM, TagOf(tagged));
        ASSERT_EQUAL_INT(value, GetSmall(tagged));
    }
    
    /* Test boundary values */
    printf("\nTesting integer boundaries...\n");
    
    /* Maximum small integer */
    sword max_small = MaxSmallNum;
    Term max_term = TagNum(max_small);
    ASSERT_TRUE(IsNum(max_term));
    ASSERT_EQUAL_INT(max_small, GetSmall(max_term));
    printf("MaxSmallNum (%" PRId64 ") tagged and extracted successfully\n", max_small);
    
    /* Minimum small integer */
    sword min_small = MinSmallNum;
    Term min_term = TagNum(min_small);
    ASSERT_TRUE(IsNum(min_term));
    ASSERT_EQUAL_INT(min_small, GetSmall(min_term));
    printf("MinSmallNum (%" PRId64 ") tagged and extracted successfully\n", min_small);
    
    /* Near-boundary values */
    sword near_max = max_small - 1000;
    sword near_min = min_small + 1000;
    
    ASSERT_TRUE(IsNum(TagNum(near_max)));
    ASSERT_TRUE(IsNum(TagNum(near_min)));
    ASSERT_EQUAL_INT(near_max, GetSmall(TagNum(near_max)));
    ASSERT_EQUAL_INT(near_min, GetSmall(TagNum(near_min)));
    
    printf("Near boundary values: PASSED\n");
}

/**
 * Test pointer type tagging and extraction
 */
void test_pointer_types() {
    TEST_SECTION("pointer type tagging and checking");
    
    /* Create some test pointers with different alignments */
    void* aligned_8 = malloc(64);
    uintptr_t addr = (uintptr_t)aligned_8;
    
    /* Ensure pointer is 8-byte aligned */
    ASSERT_EQUAL_UWORD(0, addr & 0x7);
    
    /* Test tagging and extracting references */
    Term ref_term = TagRef(aligned_8);
    ASSERT_TRUE(IsRef(ref_term));
    ASSERT_EQUAL_ADDR(aligned_8, AddressOf(ref_term));
    
    /* Test tagging and extracting list cells */
    Term lst_term = TagLst(aligned_8);
    ASSERT_TRUE(IsLst(lst_term));
    ASSERT_EQUAL_ADDR(aligned_8, AddressOf(lst_term));
    
    /* Test tagging and extracting structures */
    Term str_term = TagStr(aligned_8);
    ASSERT_TRUE(IsStr(str_term));
    ASSERT_EQUAL_ADDR(aligned_8, AddressOf(str_term));
    
    /* Free test memory */
    free(aligned_8);
    
    printf("Pointer tagging and extraction: PASSED\n");
}

/**
 * Test mark bit operations
 */
void test_mark_bit() {
    TEST_SECTION("mark bit operations");
    
    /* Test marking and unmarking different types */
    void* test_ptr = malloc(64);
    
    /* Test reference term */
    Term ref_term = TagRef(test_ptr);
    ASSERT_FALSE(IsMarked(ref_term));
    ref_term = Mark(ref_term);
    ASSERT_TRUE(IsMarked(ref_term));
    ref_term = Unmark(ref_term);
    ASSERT_FALSE(IsMarked(ref_term));
    
    /* Test list cell term */
    Term lst_term = TagLst(test_ptr);
    ASSERT_FALSE(IsMarked(lst_term));
    lst_term = Mark(lst_term);
    ASSERT_TRUE(IsMarked(lst_term));
    lst_term = Unmark(lst_term);
    ASSERT_FALSE(IsMarked(lst_term));
    
    /* Test structure term */
    Term str_term = TagStr(test_ptr);
    ASSERT_FALSE(IsMarked(str_term));
    str_term = Mark(str_term);
    ASSERT_TRUE(IsMarked(str_term));
    str_term = Unmark(str_term);
    ASSERT_FALSE(IsMarked(str_term));
    
    /* Test that marking doesn't affect tag checking */
    lst_term = TagLst(test_ptr);
    lst_term = Mark(lst_term);
    ASSERT_TRUE(IsMarked(lst_term));
    ASSERT_TRUE(IsLst(lst_term));
    
    /* Test that marking doesn't affect address extraction */
    ASSERT_EQUAL_ADDR(test_ptr, AddressOf(lst_term));
    
    /* Free test memory */
    free(test_ptr);
    
    printf("Mark bit operations: PASSED\n");
}

/**
 * Test platform-specific behavior
 */
void test_platform_specifics() {
    TEST_SECTION("platform-specific behavior");
    
    printf("Platform information:\n");
    printf("- 64-bit architecture detected\n");
    printf("- TADBITS = %d\n", TADBITS);
    printf("- sizeof(tagged_address) = %zu\n", sizeof(tagged_address));
    printf("- sizeof(uword) = %zu\n", sizeof(uword));
    printf("- MaxSmallNum = %" PRId64 "\n", MaxSmallNum);
    printf("- MinSmallNum = %" PRId64 "\n", MinSmallNum);
    
    /* Verify architecture assumptions */
    ASSERT_EQUAL_INT(64, TADBITS);
    ASSERT_EQUAL_INT(8, sizeof(tagged_address));
    ASSERT_EQUAL_INT(8, sizeof(uword));
    
    /* Verify tag bit definitions */
    ASSERT_EQUAL_UWORD(0x1, GC_BIT);
    ASSERT_EQUAL_UWORD(0xE, TAG_BITS);
    
    printf("Architecture verification: PASSED\n");
}

/**
 * Test alignment requirements
 */
void test_alignment() {
    TEST_SECTION("alignment requirements");
    
    /* Test different alignment scenarios */
    void* ptr = malloc(128);
    uintptr_t addr = (uintptr_t)ptr;
    
    /* Verify aligned pointer works correctly */
    uintptr_t aligned_addr = (addr + 7) & ~0x7;
    void* aligned_ptr = (void*)aligned_addr;
    
    Term lst_term = TagLst(aligned_ptr);
    ASSERT_TRUE(IsLst(lst_term));
    ASSERT_EQUAL_ADDR(aligned_ptr, AddressOf(lst_term));
    
    /* Free test memory */
    free(ptr);
    
    printf("Alignment requirements: PASSED\n");
}

/**
 * Test edge cases and combinations
 */
void test_edge_cases() {
    TEST_SECTION("edge cases and combinations");
    
    /* Test zero as an integer */
    Term zero = TagNum(0);
    ASSERT_TRUE(IsNum(zero));
    ASSERT_EQUAL_INT(0, GetSmall(zero));
    
    /* Test bit pattern edge cases */
    void* ptr = malloc(64);
    
    /* Create patterns that might confuse tag checking */
    uintptr_t addr = (uintptr_t)ptr;
    addr = (addr & ~0x7) | 0x0; /* Bottom 3 bits clear */
    void* aligned_ptr = (void*)addr;
    
    Term ref_term = TagRef(aligned_ptr);
    ASSERT_TRUE(IsRef(ref_term));
    ASSERT_FALSE(IsLst(ref_term));
    ASSERT_FALSE(IsStr(ref_term));
    
    /* Free test memory */
    free(ptr);
    
    printf("Edge cases: PASSED\n");
}

/**
 * Run all tests
 */
int main(int argc, char** argv) {
    printf("=== Comprehensive Testing: 64-bit Tagged Pointer Implementation ===\n\n");
    
    test_tag_operations();
    test_small_integers();
    test_pointer_types();
    test_mark_bit();
    test_platform_specifics();
    test_alignment();
    test_edge_cases();
    
    printf("\n=== Test Suite: 64-bit Tagged Pointers Comprehensive ===\n");
    printf("Status: PASSED\n");
    printf("All tests completed successfully!\n");
    
    return 0;
}
