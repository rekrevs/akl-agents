#ifndef GMP_DEBUG_H
#define GMP_DEBUG_H

#include <stdio.h>
#include <stdlib.h>

/* 
 * Simple debug macros for GMP-related operations
 * These macros check for the GMP_DEBUG environment variable at runtime
 */

/* Define debug print macros that check for the environment variable */
#define GMP_DEBUG_PRINT(msg) \
    do { \
        if (getenv("GMP_DEBUG")) { \
            fprintf(stderr, "{GMP_DEBUG: %s}\n", msg); \
            fflush(stderr); \
        } \
    } while (0)
    
#define GMP_DEBUG_PRINT_VALUE(msg, val) \
    do { \
        if (getenv("GMP_DEBUG")) { \
            fprintf(stderr, "{GMP_DEBUG: %s: %ld}\n", msg, (long)(val)); \
            fflush(stderr); \
        } \
    } while (0)
    
#define GMP_DEBUG_PRINT_PTR(msg, ptr) \
    do { \
        if (getenv("GMP_DEBUG")) { \
            fprintf(stderr, "{GMP_DEBUG: %s: %p}\n", msg, (void*)(ptr)); \
            fflush(stderr); \
        } \
    } while (0)

#endif /* GMP_DEBUG_H */
