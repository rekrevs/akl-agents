#include "test_framework.h"
#include "emulator/compat/tagged_pointers.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

/**
 * Test file for the modernized tagged pointer implementation
 * 
 * This tests the platform-agnostic tagged pointer implementation,
 * verifying that it works correctly on both 32-bit and 64-bit platforms.
 */

// Mock structure definitions for testing
typedef struct list {
    uword dummy;
    Term car;
    Term cdr;
} list;

typedef struct mp_bignum {
    uword tag2;
    int dummy;
} mp_bignum;

typedef struct fp {
    uword tag2;
    double val;
} fp;

// Accessor macros for testing
#define List(X) ((struct list *)AddressOf(X))
#define Big(X) ((struct mp_bignum *)AddressOf(X))
#define Flt(X) ((struct fp *)AddressOf(X))

// Test basic tag operations
void test_tag_operations() {
    // Create a mock list pointer
    list* lst = (list*)malloc(sizeof(list));
    Term tagged_lst = TagLst(lst);
    
    // Test tag extraction
    ASSERT_EQUAL_INT(LST, PTagOf(tagged_lst));
    
    // Test address extraction
    ASSERT_EQUAL_PTR(lst, (void*)AddressOf(tagged_lst));
    
    // Test equality
    Term tagged_lst2 = TagLst(lst);
    ASSERT_TRUE(Eq(tagged_lst, tagged_lst2));
    
    // Clean up
    free(lst);
}

// Test small integer representation
void test_small_integers() {
    // Test small integer tagging
    sword values[] = {0, 1, -1, 100, -100, 1000, -1000};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        Term num = MakeSmallNum(values[i]);
        
        // Print debug info
        printf("TagNum(%ld) = 0x%lx, IsNum=%d, IsAtm=%d, PTagOf=0x%lx\n", 
               values[i], (unsigned long)Tad(num), IsNum(num), IsAtm(num), (unsigned long)PTagOf(num));

        // Verify it's tagged as a number
        ASSERT_TRUE(IsNum(num));
        ASSERT_FALSE(IsAtm(num));
        
        // Verify we can extract the original value
        ASSERT_EQUAL_INT(values[i], GetSmall(num));
    }
    
    // Test maximum small integer values
    sword max_small = MaxSmallNum - 1;
    sword min_small = -MaxSmallNum;
    
    Term max_term = MakeSmallNum(max_small);
    Term min_term = MakeSmallNum(min_small);
    
    ASSERT_TRUE(IsNum(max_term));
    ASSERT_TRUE(IsNum(min_term));
    ASSERT_EQUAL_INT(max_small, GetSmall(max_term));
    ASSERT_EQUAL_INT(min_small, GetSmall(min_term));
}

// Test different pointer types
void test_pointer_types() {
    // Create various mock pointers
    void* ref_ptr = malloc(sizeof(uword));
    void* uva_ptr = malloc(sizeof(uword));
    void* gva_ptr = malloc(sizeof(uword));
    list* lst_ptr = (list*)malloc(sizeof(list));
    void* str_ptr = malloc(sizeof(uword) * 5);
    mp_bignum* big_ptr = (mp_bignum*)malloc(sizeof(mp_bignum));
    fp* flt_ptr = (fp*)malloc(sizeof(fp));
    
    // Set tag2 values for big and float
    big_ptr->tag2 = BIG_TAG2;
    flt_ptr->tag2 = FLT_TAG2;
    
    // Tag the pointers
    Term ref = TagRef(ref_ptr);
    Term uva = TagUva(uva_ptr);
    Term gva = TagGva(gva_ptr);
    Term lst = TagLst(lst_ptr);
    Term str = TagStr(str_ptr);
    Term big = TagBig(big_ptr);
    Term flt = TagFlt(flt_ptr);
    
    // Test type predicates
    ASSERT_TRUE(IsRef(ref));
    ASSERT_TRUE(IsUva(uva));
    ASSERT_TRUE(IsGva(gva));
    ASSERT_TRUE(IsLst(lst));
    ASSERT_TRUE(IsStr(str));
    ASSERT_TRUE(IsGen(big));
    ASSERT_TRUE(IsGen(flt));
    ASSERT_TRUE(IsBIG(big));
    ASSERT_TRUE(IsFLT(flt));
    
    // Test variable predicates
    ASSERT_TRUE(IsVar(ref));
    ASSERT_TRUE(IsVar(uva));
    ASSERT_TRUE(IsVar(gva));
    ASSERT_FALSE(IsVar(lst));
    
    // Clean up
    free(ref_ptr);
    free(uva_ptr);
    free(gva_ptr);
    free(lst_ptr);
    free(str_ptr);
    free(big_ptr);
    free(flt_ptr);
}

// Test mark bit operations for garbage collection
void test_mark_bit() {
    // Create a mock pointer
    void* ptr = malloc(sizeof(uword));
    Term tagged = TagRef(ptr);
    
    // Initially should be unmarked
    ASSERT_FALSE(IsMarked(tagged));
    
    // Mark it
    Term marked = MarkTerm(tagged);
    ASSERT_TRUE(IsMarked(marked));
    
    // Unmark it
    Term unmarked = UnmarkTerm(marked);
    ASSERT_FALSE(IsMarked(unmarked));
    
    // Marking shouldn't affect the tag or address
    ASSERT_EQUAL_INT(PTagOf(tagged), PTagOf(marked));
    ASSERT_EQUAL_PTR((void*)AddressOf(tagged), (void*)AddressOf(marked));
    
    // Clean up
    free(ptr);
}

// Test platform-specific behavior
void test_platform_specifics() {
    // Output platform information
    printf("Platform information:\n");
#ifdef AKL_64BIT
    printf("- 64-bit architecture detected\n");
    printf("- TADBITS = %d\n", TADBITS);
    printf("- sizeof(tagged_address) = %zu\n", sizeof(tagged_address));
    printf("- sizeof(uword) = %zu\n", sizeof(uword));
    printf("- MaxSmallNum = %ld\n", (long)MaxSmallNum);
#else
    printf("- 32-bit architecture detected\n");
    printf("- TADBITS = %d\n", TADBITS);
    printf("- sizeof(tagged_address) = %zu\n", sizeof(tagged_address));
    printf("- sizeof(uword) = %zu\n", sizeof(uword));
    printf("- MaxSmallNum = %ld\n", (long)MaxSmallNum);
#endif
    
    // Test that small integers have the expected range
    Term max_valid = MakeSmallNum(MaxSmallNum - 1);
    ASSERT_TRUE(IsNum(max_valid));
    
    // Verify alignment requirements
    ASSERT_EQUAL_INT(WORDALIGNMENT, sizeof(uword));
}

// Test pointer alignment requirements
void test_alignment() {
    // Allocate some memory
    void* ptr = malloc(sizeof(uword) * 10);
    
    // Ensure the pointer is properly aligned
    uintptr_t addr = (uintptr_t)ptr;
    ASSERT_EQUAL_INT(0, addr % WORDALIGNMENT);
    
    // Test that we can tag and untag without losing information
    Term tagged = TagRef(ptr);
    void* recovered = (void*)AddressOf(tagged);
    ASSERT_EQUAL_PTR(ptr, recovered);
    
    // Clean up
    free(ptr);
}

int main(int argc, char** argv) {
    // Create test suite
    test_suite_t* suite = test_suite_create("Modern Tagged Pointers");
    
    // Run tests
    RUN_TEST(suite, test_tag_operations);
    RUN_TEST(suite, test_small_integers);
    RUN_TEST(suite, test_pointer_types);
    RUN_TEST(suite, test_mark_bit);
    RUN_TEST(suite, test_platform_specifics);
    RUN_TEST(suite, test_alignment);
    
    // Print results
    test_suite_print_results(suite);
    
    // Get status
    int status = test_suite_status(suite);
    
    // Cleanup
    test_suite_destroy(suite);
    
    return status;
}
