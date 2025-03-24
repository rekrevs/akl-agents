/**
 * debug_tagged_pointers_64bit.c - Diagnostic test for the 64-bit tagged pointer implementation
 *
 * This file contains detailed diagnostics to understand what's happening with the
 * 64-bit tagged pointer system, with extensive output to identify issues.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

/* Include the 64-bit tagged pointer implementation */
#include "../../emulator/compat/tagged_pointers_64bit.h"

/* Test cases */
void test_tag_constants() {
    printf("===== TAG CONSTANTS INSPECTION =====\n");
    printf("GC_BIT: 0x%lx\n", (unsigned long)GC_BIT);
    printf("TAG_BITS: 0x%lx\n", (unsigned long)TAG_BITS);
    printf("EXTENDED_BITS: 0x%lx\n", (unsigned long)EXTENDED_BITS);
    printf("IMM_ATM_BIT: 0x%lx\n", (unsigned long)IMM_ATM_BIT);
    printf("SEC_TAG_BITS: 0x%lx\n", (unsigned long)SEC_TAG_BITS);
    printf("POINTER_MASK: 0x%lx\n", (unsigned long)POINTER_MASK);
    printf("NUM_MASK: 0x%lx\n", (unsigned long)NUM_MASK);
    printf("NUM_PATTERN: 0x%lx\n", (unsigned long)NUM_PATTERN);
    printf("ATM_MASK: 0x%lx\n", (unsigned long)ATM_MASK);
    printf("ATM_PATTERN: 0x%lx\n", (unsigned long)ATM_PATTERN);
    
    printf("\n== Tag values ==\n");
    printf("REF: 0x%x\n", REF);
    printf("UVA: 0x%x\n", UVA);
    printf("LST: 0x%x\n", LST);
    printf("GVA: 0x%x\n", GVA);
    printf("IMM: 0x%x\n", IMM);
    printf("STR: 0x%x\n", STR);
    printf("GEN: 0x%x\n", GEN);
    
    printf("\n== Secondary tag values ==\n");
    printf("BIG_TAG2: 0x%lx\n", (unsigned long)BIG_TAG2);
    printf("FLT_TAG2: 0x%lx\n", (unsigned long)FLT_TAG2);
    printf("GEN_TAG2: 0x%lx\n", (unsigned long)GEN_TAG2);
    
    printf("\n");
}

void test_tag_operations() {
    printf("===== TAG OPERATIONS =====\n");
    
    /* We'll use a sample value to test tag extraction */
    uword sample_ref = TagPtr((void*)0x1234, REF);
    uword sample_imm = TagNum(42);
    uword sample_lst = TagLst((void*)0x1234);
    uword sample_gen = TagGen((void*)0x1234);
    uword sample_flt = TagFlt((void*)0x1234);
    uword sample_big = TagBig((void*)0x1234);
    uword sample_atm = TagAtm(42);
    
    printf("\n== Tag extraction ==\n");
    printf("Sample REF: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_ref, (unsigned long)TagOf(sample_ref));
    printf("Sample IMM: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_imm, (unsigned long)TagOf(sample_imm));
    printf("Sample LST: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_lst, (unsigned long)TagOf(sample_lst));
    printf("Sample GEN: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_gen, (unsigned long)TagOf(sample_gen));
    printf("Sample FLT: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_flt, (unsigned long)TagOf(sample_flt));
    printf("Sample BIG: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_big, (unsigned long)TagOf(sample_big));
    printf("Sample ATM: 0x%lx, TagOf=0x%lx\n", 
           (unsigned long)sample_atm, (unsigned long)TagOf(sample_atm));
    
    printf("\n== Type checking ==\n");
    printf("Sample REF: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d\n", 
           IsRef(sample_ref), IsImm(sample_ref), IsNum(sample_ref), IsAtm(sample_ref), IsGen(sample_ref));
    printf("Sample IMM: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d\n", 
           IsRef(sample_imm), IsImm(sample_imm), IsNum(sample_imm), IsAtm(sample_imm), IsGen(sample_imm));
    printf("Sample LST: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d\n", 
           IsRef(sample_lst), IsImm(sample_lst), IsNum(sample_lst), IsAtm(sample_lst), IsGen(sample_lst));
    printf("Sample GEN: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d\n", 
           IsRef(sample_gen), IsImm(sample_gen), IsNum(sample_gen), IsAtm(sample_gen), IsGen(sample_gen));
    printf("Sample FLT: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d, IsFLT=%d\n", 
           IsRef(sample_flt), IsImm(sample_flt), IsNum(sample_flt), IsAtm(sample_flt), IsGen(sample_flt), IsFLT(sample_flt));
    printf("Sample BIG: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d, IsBIG=%d\n", 
           IsRef(sample_big), IsImm(sample_big), IsNum(sample_big), IsAtm(sample_big), IsGen(sample_big), IsBIG(sample_big));
    printf("Sample ATM: IsRef=%d, IsImm=%d, IsNum=%d, IsAtm=%d, IsGen=%d\n", 
           IsRef(sample_atm), IsImm(sample_atm), IsNum(sample_atm), IsAtm(sample_atm), IsGen(sample_atm));
    
    printf("\n== Address extraction ==\n");
    printf("Sample REF: Original=0x1234, Extracted=%p, Diff=%ld\n", 
           (void*)0x1234, AddressOf(sample_ref), 
           (long)((uintptr_t)AddressOf(sample_ref) - (uintptr_t)0x1234));
    printf("Sample LST: Original=0x1234, Extracted=%p, Diff=%ld\n", 
           (void*)0x1234, AddressOf(sample_lst), 
           (long)((uintptr_t)AddressOf(sample_lst) - (uintptr_t)0x1234));
    printf("Sample GEN: Original=0x1234, Extracted=%p, Diff=%ld\n", 
           (void*)0x1234, AddressOf(sample_gen), 
           (long)((uintptr_t)AddressOf(sample_gen) - (uintptr_t)0x1234));
    printf("Sample FLT: Original=0x1234, Extracted=%p, Diff=%ld\n", 
           (void*)0x1234, AddressOf(sample_flt), 
           (long)((uintptr_t)AddressOf(sample_flt) - (uintptr_t)0x1234));
    printf("Sample BIG: Original=0x1234, Extracted=%p, Diff=%ld\n", 
           (void*)0x1234, AddressOf(sample_big), 
           (long)((uintptr_t)AddressOf(sample_big) - (uintptr_t)0x1234));
    
    printf("\n== Bit manipulation detail ==\n");
    printf("Sample REF (0x%lx):\n", (unsigned long)sample_ref);
    printf("  & TAG_BITS (0x%lx) = 0x%lx\n", 
           (unsigned long)TAG_BITS, (unsigned long)(sample_ref & TAG_BITS));
    printf("  & EXTENDED_BITS (0x%lx) = 0x%lx\n", 
           (unsigned long)EXTENDED_BITS, (unsigned long)(sample_ref & EXTENDED_BITS));
    printf("  & POINTER_MASK (0x%lx) = 0x%lx\n", 
           (unsigned long)POINTER_MASK, (unsigned long)(sample_ref & POINTER_MASK));
    printf("  & ~POINTER_MASK (0x%lx) = 0x%lx\n", 
           (unsigned long)(~POINTER_MASK), (unsigned long)(sample_ref & ~POINTER_MASK));
    
    printf("\n");
}

void test_small_integers() {
    printf("===== SMALL INTEGER DIAGNOSTICS =====\n");
    
    /* Test integer tagging and extraction */
    sword values[] = {0, 1, -1, 42, -42, 1000000, -1000000};
    int count = sizeof(values) / sizeof(values[0]);
    
    printf("\n== Integer tagging debug ==\n");
    for (int i = 0; i < count; i++) {
        sword original = values[i];
        Term tagged = TagNum(original);
        sword extracted = GetSmall(tagged);
        
        printf("Original: %ld\n", (long)original);
        printf("  Tagged value: 0x%lx\n", (unsigned long)tagged);
        printf("  TagOf: 0x%lx\n", (unsigned long)TagOf(tagged));
        printf("  IsImm: %d\n", IsImm(tagged));
        printf("  IsNum: %d\n", IsNum(tagged));
        printf("  IsAtm: %d\n", IsAtm(tagged));
        printf("  Extracted: %ld\n", (long)extracted);
        printf("  Original == Extracted: %s\n", 
               original == extracted ? "PASS" : "FAIL");
        
        /* Detailed bit analysis */
        printf("  Bit analysis:\n");
        printf("    Shift amount in TagNum: 5\n");
        printf("    Original << 5: 0x%lx\n", (unsigned long)(original << 5));
        printf("    (Original << 5) | IMM: 0x%lx\n", 
               (unsigned long)((original << 5) | IMM));
        printf("    Tagged >> 5: 0x%lx\n", (unsigned long)(tagged >> 5));
        
        printf("\n");
    }
    
    /* Test the maximum and minimum small integers */
    printf("\n== Maximum and minimum small integers ==\n");
    
    sword max_small = MaxSmallNum - 1;
    sword min_small = MinSmallNum;
    
    Term max_term = TagNum(max_small);
    Term min_term = TagNum(min_small);
    
    printf("MaxSmallNum (0x%lx):\n", (unsigned long)MaxSmallNum);
    printf("  Tagged: 0x%lx\n", (unsigned long)max_term);
    printf("  IsNum: %d\n", IsNum(max_term));
    printf("  Extracted: %ld\n", (long)GetSmall(max_term));
    printf("  Original == Extracted: %s\n", 
           max_small == GetSmall(max_term) ? "PASS" : "FAIL");
    
    printf("MinSmallNum (0x%lx):\n", (unsigned long)MinSmallNum);
    printf("  Tagged: 0x%lx\n", (unsigned long)min_term);
    printf("  IsNum: %d\n", IsNum(min_term));
    printf("  Extracted: %ld\n", (long)GetSmall(min_term));
    printf("  Original == Extracted: %s\n", 
           min_small == GetSmall(min_term) ? "PASS" : "FAIL");
    
    printf("\n");
}

void test_pointer_types() {
    printf("===== POINTER TYPE DIAGNOSTICS =====\n");
    
    /* Create various mock pointers */
    void* ref_ptr = malloc(sizeof(uword));
    void* uva_ptr = malloc(sizeof(uword));
    void* gva_ptr = malloc(sizeof(uword));
    void* lst_ptr = malloc(sizeof(uword) * 2);
    void* str_ptr = malloc(sizeof(uword) * 5);
    void* big_ptr = malloc(sizeof(uword) * 2);
    void* flt_ptr = malloc(sizeof(uword) * 2);
    
    /* Print the original addresses */
    printf("\n== Original pointer addresses ==\n");
    printf("ref_ptr: %p\n", ref_ptr);
    printf("uva_ptr: %p\n", uva_ptr);
    printf("gva_ptr: %p\n", gva_ptr);
    printf("lst_ptr: %p\n", lst_ptr);
    printf("str_ptr: %p\n", str_ptr);
    printf("big_ptr: %p\n", big_ptr);
    printf("flt_ptr: %p\n", flt_ptr);
    
    /* Tag the pointers */
    Term ref_term = TagRef(ref_ptr);
    Term uva_term = TagUva(uva_ptr);
    Term gva_term = TagGva(gva_ptr);
    Term lst_term = TagLst(lst_ptr);
    Term str_term = TagStr(str_ptr);
    Term big_term = TagBig(big_ptr);
    Term flt_term = TagFlt(flt_ptr);
    
    /* Print the tagged values */
    printf("\n== Tagged values ==\n");
    printf("ref_term: 0x%lx\n", (unsigned long)ref_term);
    printf("uva_term: 0x%lx\n", (unsigned long)uva_term);
    printf("gva_term: 0x%lx\n", (unsigned long)gva_term);
    printf("lst_term: 0x%lx\n", (unsigned long)lst_term);
    printf("str_term: 0x%lx\n", (unsigned long)str_term);
    printf("big_term: 0x%lx\n", (unsigned long)big_term);
    printf("flt_term: 0x%lx\n", (unsigned long)flt_term);
    
    /* Print tag and type information */
    printf("\n== Tag info ==\n");
    printf("ref_term: TagOf=0x%lx, IsRef=%d\n", 
           (unsigned long)TagOf(ref_term), IsRef(ref_term));
    printf("uva_term: TagOf=0x%lx, IsUva=%d\n", 
           (unsigned long)TagOf(uva_term), IsUva(uva_term));
    printf("gva_term: TagOf=0x%lx, IsGva=%d\n", 
           (unsigned long)TagOf(gva_term), IsGva(gva_term));
    printf("lst_term: TagOf=0x%lx, IsLst=%d\n", 
           (unsigned long)TagOf(lst_term), IsLst(lst_term));
    printf("str_term: TagOf=0x%lx, IsStr=%d\n", 
           (unsigned long)TagOf(str_term), IsStr(str_term));
    printf("big_term: TagOf=0x%lx, ExtendedTagOf=0x%lx, IsGen=%d, IsBIG=%d\n", 
           (unsigned long)TagOf(big_term), (unsigned long)ExtendedTagOf(big_term), 
           IsGen(big_term), IsBIG(big_term));
    printf("flt_term: TagOf=0x%lx, ExtendedTagOf=0x%lx, IsGen=%d, IsFLT=%d\n", 
           (unsigned long)TagOf(flt_term), (unsigned long)ExtendedTagOf(flt_term), 
           IsGen(flt_term), IsFLT(flt_term));
    
    /* Address extraction */
    printf("\n== Address extraction ==\n");
    void* ref_extracted = AddressOf(ref_term);
    void* uva_extracted = AddressOf(uva_term);
    void* gva_extracted = AddressOf(gva_term);
    void* lst_extracted = AddressOf(lst_term);
    void* str_extracted = AddressOf(str_term);
    void* big_extracted = AddressOf(big_term);
    void* flt_extracted = AddressOf(flt_term);
    
    printf("ref_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           ref_ptr, ref_extracted, ref_ptr == ref_extracted ? "YES" : "NO");
    printf("uva_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           uva_ptr, uva_extracted, uva_ptr == uva_extracted ? "YES" : "NO");
    printf("gva_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           gva_ptr, gva_extracted, gva_ptr == gva_extracted ? "YES" : "NO");
    printf("lst_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           lst_ptr, lst_extracted, lst_ptr == lst_extracted ? "YES" : "NO");
    printf("str_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           str_ptr, str_extracted, str_ptr == str_extracted ? "YES" : "NO");
    printf("big_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           big_ptr, big_extracted, big_ptr == big_extracted ? "YES" : "NO");
    printf("flt_ptr: Original=%p, Extracted=%p, Match=%s\n", 
           flt_ptr, flt_extracted, flt_ptr == flt_extracted ? "YES" : "NO");
    
    /* Detailed bit analysis for one example */
    printf("\n== Detailed bit analysis (lst_term) ==\n");
    printf("Original pointer: %p (0x%lx)\n", lst_ptr, (unsigned long)(uintptr_t)lst_ptr);
    printf("Shifted value: 0x%lx\n", (unsigned long)((uintptr_t)lst_ptr << 5));
    printf("Tagged value: 0x%lx\n", (unsigned long)lst_term);
    printf("Masked value: 0x%lx\n", (unsigned long)(lst_term & POINTER_MASK));
    printf("Shifted back: 0x%lx\n", (unsigned long)((lst_term & POINTER_MASK) >> 5));
    printf("Extracted pointer: %p\n", AddressOf(lst_term));
    
    /* Clean up */
    free(ref_ptr);
    free(uva_ptr);
    free(gva_ptr);
    free(lst_ptr);
    free(str_ptr);
    free(big_ptr);
    free(flt_ptr);
    
    printf("\n");
}

void test_mark_bit() {
    printf("===== MARK BIT DIAGNOSTICS =====\n");
    
    /* Test mark bit for different types */
    void* lst_ptr = malloc(sizeof(uword) * 2);
    sword num_val = 42;
    
    Term lst_term = TagLst(lst_ptr);
    Term num_term = TagNum(num_val);
    
    printf("\n== Mark bit operations ==\n");
    
    /* List term */
    printf("Original lst_term: 0x%lx\n", (unsigned long)lst_term);
    printf("  IsLst: %d\n", IsLst(lst_term));
    printf("  IsMarked: %d\n", IsMarked(lst_term));
    
    Term marked_lst = Mark(lst_term);
    printf("Marked lst_term: 0x%lx\n", (unsigned long)marked_lst);
    printf("  IsLst: %d\n", IsLst(marked_lst));
    printf("  IsMarked: %d\n", IsMarked(marked_lst));
    printf("  Original ptr: %p\n", lst_ptr);
    printf("  AddressOf(marked): %p\n", AddressOf(marked_lst));
    
    Term unmarked_lst = Unmark(marked_lst);
    printf("Unmarked lst_term: 0x%lx\n", (unsigned long)unmarked_lst);
    printf("  IsLst: %d\n", IsLst(unmarked_lst));
    printf("  IsMarked: %d\n", IsMarked(unmarked_lst));
    printf("  Original ptr: %p\n", lst_ptr);
    printf("  AddressOf(unmarked): %p\n", AddressOf(unmarked_lst));
    
    /* Num term */
    printf("\nOriginal num_term: 0x%lx\n", (unsigned long)num_term);
    printf("  IsNum: %d\n", IsNum(num_term));
    printf("  IsMarked: %d\n", IsMarked(num_term));
    
    Term marked_num = Mark(num_term);
    printf("Marked num_term: 0x%lx\n", (unsigned long)marked_num);
    printf("  IsNum: %d\n", IsNum(marked_num));
    printf("  IsMarked: %d\n", IsMarked(marked_num));
    printf("  Original val: %ld\n", (long)num_val);
    printf("  GetSmall(marked): %ld\n", (long)GetSmall(marked_num));
    
    Term unmarked_num = Unmark(marked_num);
    printf("Unmarked num_term: 0x%lx\n", (unsigned long)unmarked_num);
    printf("  IsNum: %d\n", IsNum(unmarked_num));
    printf("  IsMarked: %d\n", IsMarked(unmarked_num));
    printf("  Original val: %ld\n", (long)num_val);
    printf("  GetSmall(unmarked): %ld\n", (long)GetSmall(unmarked_num));
    
    /* Clean up */
    free(lst_ptr);
    
    printf("\n");
}

void test_special_cases() {
    printf("===== SPECIAL CASES =====\n");
    
    /* Test edge case values */
    printf("\n== Edge cases ==\n");
    
    /* Null term */
    printf("NullTerm: 0x%lx\n", (unsigned long)NullTerm);
    printf("  IsRef: %d\n", IsRef(NullTerm));
    printf("  IsLst: %d\n", IsLst(NullTerm));
    printf("  IsImm: %d\n", IsImm(NullTerm));
    printf("  IsNum: %d\n", IsNum(NullTerm));
    
    /* Maximum small integer */
    sword max_int = (sword)0x7fffffffffffffffULL >> 5;
    Term max_int_term = TagNum(max_int);
    printf("Max int value: %ld (0x%lx)\n", (long)max_int, (unsigned long)max_int);
    printf("  Tagged: 0x%lx\n", (unsigned long)max_int_term);
    printf("  IsNum: %d\n", IsNum(max_int_term));
    printf("  GetSmall: %ld\n", (long)GetSmall(max_int_term));
    
    /* Minimum small integer */
    sword min_int = (sword)0x8000000000000000ULL >> 5;
    Term min_int_term = TagNum(min_int);
    printf("Min int value: %ld (0x%lx)\n", (long)min_int, (unsigned long)min_int);
    printf("  Tagged: 0x%lx\n", (unsigned long)min_int_term);
    printf("  IsNum: %d\n", IsNum(min_int_term));
    printf("  GetSmall: %ld\n", (long)GetSmall(min_int_term));
    
    printf("\n");
}

/* Run all tests */
int main() {
    printf("=== Detailed Diagnostics for 64-bit Tagged Pointer Implementation ===\n\n");
    
    /* Run all test functions */
    test_tag_constants();
    test_tag_operations();
    test_small_integers();
    test_pointer_types();
    test_mark_bit();
    test_special_cases();
    
    printf("=== Diagnostics Complete ===\n");
    return 0;
}
