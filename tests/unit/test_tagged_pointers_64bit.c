/**
 * test_tagged_pointers_64bit.c - Test suite for the 64-bit tagged pointer implementation
 *
 * This file contains comprehensive tests for the 64-bit tagged pointer system,
 * verifying all tag operations, type checking, and address extraction work correctly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

/* Include the 64-bit tagged pointer implementation */
#include "../../emulator/compat/tagged_pointers_64bit.h"

/* Enable debug output */
#define AKL_DEBUG 1

/* Test assertion macros */
#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        printf("Assertion failed: %s is not true\n", #cond); \
        test_failed = true; \
        return; \
    } \
} while (0)

#define ASSERT_FALSE(cond) do { \
    if (cond) { \
        printf("Assertion failed: %s is not false\n", #cond); \
        test_failed = true; \
        return; \
    } \
} while (0)

#define ASSERT_EQUAL_INT(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf("Assertion failed: %s == %s (expected %lld, got %lld)\n", \
               #expected, #actual, (long long)(expected), (long long)(actual)); \
        test_failed = true; \
        return; \
    } \
} while (0)

#define ASSERT_EQUAL_PTR(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf("Assertion failed: %s == %s (expected %p, got %p)\n", \
               #expected, #actual, (void*)(expected), (void*)(actual)); \
        test_failed = true; \
        return; \
    } \
} while (0)

/* Global flag to track test failures */
static bool test_failed = false;

/* Mock structure definitions for testing */
typedef struct {
    uword tag2;
    /* other fields as needed */
} mp_bignum;

typedef struct {
    uword tag2;
    /* other fields as needed */
} fp;

typedef struct {
    Term head;
    Term tail;
} list;

/* Test tag operations (tagging and untagging) */
void test_tag_operations() {
    printf("Testing basic tag operations...\n");
    
    /* Create a mock list cell */
    list* lst = (list*)malloc(sizeof(list));
    lst->head = NullTerm;
    lst->tail = NullTerm;
    
    /* Tag the list */
    Term tagged_lst = TagLst(lst);
    
    /* Check if the list is correctly tagged */
    ASSERT_TRUE(IsLst(tagged_lst));
    ASSERT_FALSE(IsStr(tagged_lst));
    ASSERT_FALSE(IsImm(tagged_lst));
    
    /* Check address extraction */
    ASSERT_EQUAL_PTR(lst, AddressOf(tagged_lst));
    
    /* Clean up */
    free(lst);
}

/* Test small integer representation */
void test_small_integers() {
    printf("Testing small integer representation...\n");
    
    /* Test small integer tagging */
    sword values[] = {0, 1, -1, 100, -100, 1000, -1000};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        Term num = MakeSmallNum(values[i]);
        
        /* Print debug info */
        printf("TagNum(%lld) = 0x%lx, IsNum=%d, IsAtm=%d, TagOf=0x%lx\n", 
               (long long)values[i], (unsigned long)num, IsNum(num), IsAtm(num), (unsigned long)TagOf(num));
        
        /* Verify it's tagged as a number */
        ASSERT_TRUE(IsNum(num));
        ASSERT_FALSE(IsAtm(num));
        
        /* Verify we can extract the original value */
        ASSERT_EQUAL_INT(values[i], GetSmall(num));
    }
    
    /* Test maximum small integer values */
    sword max_small = MaxSmallNum;
    sword min_small = MinSmallNum;
    
    Term max_term = MakeSmallNum(max_small);
    Term min_term = MakeSmallNum(min_small);
    
    ASSERT_TRUE(IsNum(max_term));
    ASSERT_TRUE(IsNum(min_term));
    ASSERT_EQUAL_INT(max_small, GetSmall(max_term));
    ASSERT_EQUAL_INT(min_small, GetSmall(min_term));
}

/* Test different pointer types */
void test_pointer_types() {
    printf("Testing pointer type tagging and checking...\n");
    
    /* Create various mock pointers */
    void* ref_ptr = malloc(sizeof(uword));
    void* uva_ptr = malloc(sizeof(uword));
    void* gva_ptr = malloc(sizeof(uword));
    list* lst_ptr = (list*)malloc(sizeof(list));
    void* str_ptr = malloc(sizeof(uword) * 5);
    mp_bignum* big_ptr = (mp_bignum*)malloc(sizeof(mp_bignum));
    fp* flt_ptr = (fp*)malloc(sizeof(fp));
    
    /* Set tag2 values for big and float */
    big_ptr->tag2 = BIG_TAG2;
    flt_ptr->tag2 = FLT_TAG2;
    
    /* Tag the pointers */
    Term ref_term = TagRef(ref_ptr);
    Term uva_term = TagUva(uva_ptr);
    Term gva_term = TagGva(gva_ptr);
    Term lst_term = TagLst(lst_ptr);
    Term str_term = TagStr(str_ptr);
    Term big_term = TagBig(big_ptr);
    Term flt_term = TagFlt(flt_ptr);
    
    /* Verify type checking */
    ASSERT_TRUE(IsRef(ref_term));
    ASSERT_TRUE(IsUva(uva_term));
    ASSERT_TRUE(IsGva(gva_term));
    ASSERT_TRUE(IsLst(lst_term));
    ASSERT_TRUE(IsStr(str_term));
    ASSERT_TRUE(IsGen(big_term));
    ASSERT_TRUE(IsGen(flt_term));
    ASSERT_TRUE(IsBIG(big_term));
    ASSERT_TRUE(IsFLT(flt_term));
    
    /* Verify address extraction */
    ASSERT_EQUAL_PTR(ref_ptr, AddressOf(ref_term));
    ASSERT_EQUAL_PTR(uva_ptr, AddressOf(uva_term));
    ASSERT_EQUAL_PTR(gva_ptr, AddressOf(gva_term));
    ASSERT_EQUAL_PTR(lst_ptr, AddressOf(lst_term));
    ASSERT_EQUAL_PTR(str_ptr, AddressOf(str_term));
    ASSERT_EQUAL_PTR(big_ptr, AddressOf(big_term));
    ASSERT_EQUAL_PTR(flt_ptr, AddressOf(flt_term));
    
    /* Clean up */
    free(ref_ptr);
    free(uva_ptr);
    free(gva_ptr);
    free(lst_ptr);
    free(str_ptr);
    free(big_ptr);
    free(flt_ptr);
}

/* Test mark bit operations for garbage collection */
void test_mark_bit() {
    printf("Testing mark bit operations...\n");
    
    void* ptr = malloc(sizeof(uword));
    Term term = TagLst(ptr);
    
    /* Initially unmarked */
    ASSERT_FALSE(IsMarked(term));
    
    /* Mark the term */
    Term marked = Mark(term);
    ASSERT_TRUE(IsMarked(marked));
    
    /* Verify it's still a list after marking */
    ASSERT_TRUE(IsLst(marked));
    
    /* Verify the address is preserved */
    ASSERT_EQUAL_PTR(ptr, AddressOf(marked));
    
    /* Unmark the term */
    Term unmarked = Unmark(marked);
    ASSERT_FALSE(IsMarked(unmarked));
    ASSERT_TRUE(IsLst(unmarked));
    ASSERT_EQUAL_PTR(ptr, AddressOf(unmarked));
    
    free(ptr);
}

/* Test platform-specific behavior */
void test_platform_specifics() {
    printf("Testing platform-specific behavior...\n");
    
    printf("Platform information:\n");
    printf("- 64-bit architecture detected\n");
    printf("- TADBITS = %d\n", TADBITS);
    printf("- sizeof(tagged_address) = %zu\n", sizeof(tagged_address));
    printf("- sizeof(uword) = %zu\n", sizeof(uword));
    printf("- MaxSmallNum = %" PRId64 "\n", MaxSmallNum);
    
    /* Test the max small integer value */
    sword max_valid = MaxSmallNum;
    Term max_term = MakeSmallNum(max_valid);
    
    ASSERT_TRUE(IsNum(max_term));
    ASSERT_EQUAL_INT(max_valid, GetSmall(max_term));
}

/* Test alignment requirements */
void test_alignment() {
    printf("Testing alignment requirements...\n");
    
    /* Allocate memory and ensure it's 8-byte aligned */
    void* ptr = malloc(sizeof(uword) * 2);
    
    /* Verify the pointer is 8-byte aligned */
    ASSERT_EQUAL_INT(0, ((uintptr_t)ptr & 0x7));
    
    /* Tag and untag the pointer */
    Term tagged = TagLst(ptr);
    void* recovered = AddressOf(tagged);
    
    /* Verify we can recover the original pointer */
    ASSERT_EQUAL_PTR(ptr, recovered);
    
    free(ptr);
}

/* Run all tests */
int main(int argc, char** argv) {
    printf("=== Testing 64-bit Tagged Pointer Implementation ===\n\n");
    
    /* Run all test functions */
    test_tag_operations();
    test_small_integers();
    test_pointer_types();
    test_mark_bit();
    test_platform_specifics();
    test_alignment();
    
    /* Report results */
    printf("\n=== Test Suite: 64-bit Tagged Pointers ===\n");
    if (test_failed) {
        printf("Status: FAILED\n");
        return 1;
    } else {
        printf("Status: PASSED\n");
        printf("All tests passed successfully!\n");
        return 0;
    }
}
