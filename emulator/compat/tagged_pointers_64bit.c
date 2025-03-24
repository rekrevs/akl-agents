/**
 * tagged_pointers_64bit.c - Debug utilities for 64-bit tagged pointer system
 *
 * This file contains only debug and testing utilities for the 64-bit tagged pointer
 * implementation. All core functionality has been moved to tagged_pointers_64bit.h
 * as macros and inline functions for better performance and compiler optimization.
 */

#include "emulator/compat/tagged_pointers_64bit.h"
#include "emulator/compat/debug_checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * Print a term for debugging
 */
void DebugPrintTerm(Term term) {
    printf("Term: 0x%016llx\n", (unsigned long long)term);
    
    if (IsRef(term)) {
        printf("  Type: Reference\n");
        printf("  Address: %p\n", AddressOf(term));
    } else if (IsLst(term)) {
        printf("  Type: List\n");
        printf("  Address: %p\n", AddressOf(term));
    } else if (IsStr(term)) {
        printf("  Type: Structure\n");
        printf("  Address: %p\n", AddressOf(term));
    } else if (IsGen(term)) {
        printf("  Type: Generic\n");
        printf("  Address: %p\n", AddressOf(term));
    } else if (IsNum(term)) {
        printf("  Type: Small Integer\n");
        printf("  Value: %lld\n", (long long)GetSmall(term));
    } else {
        printf("  Type: Unknown\n");
    }
    
    printf("  GC Mark: %s\n", IsMarked(term) ? "Yes" : "No");
}
