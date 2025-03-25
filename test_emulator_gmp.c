#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

// Simplified version of the mp_bignum structure
typedef struct {
    unsigned long tag2;
    mpz_t mp_int;
} mp_bignum, *Bignum;

// Simplified version of the NEW macro
#define NEW(ret, type) \
{ \
    (ret) = (type *) malloc(sizeof(type)); \
}

// Simplified version of the make_bignum function
Bignum make_bignum(long value) {
    Bignum new;
    
    printf("make_bignum called with value %ld\n", value);
    printf("About to allocate memory with NEW macro\n");
    NEW(new, mp_bignum);
    printf("Memory allocated at %p\n", (void*)new);
    printf("Setting tag2 field\n");
    new->tag2 = 123; // Arbitrary value for tag2
    printf("About to call mpz_init_set_si\n");
    mpz_init_set_si(new->mp_int, value);
    printf("mpz_init_set_si completed successfully\n");
    
    return new;
}

int main() {
    Bignum b;
    
    printf("Testing make_bignum with various values...\n");
    
    // Test with small value
    printf("\nTest 1: make_bignum(0)\n");
    b = make_bignum(0);
    gmp_printf("Result: %Zd\n", b->mp_int);
    mpz_clear(b->mp_int);
    free(b);
    
    // Test with a larger value
    printf("\nTest 2: make_bignum(1000000)\n");
    b = make_bignum(1000000);
    gmp_printf("Result: %Zd\n", b->mp_int);
    mpz_clear(b->mp_int);
    free(b);
    
    // Test with a negative value
    printf("\nTest 3: make_bignum(-1000000)\n");
    b = make_bignum(-1000000);
    gmp_printf("Result: %Zd\n", b->mp_int);
    mpz_clear(b->mp_int);
    free(b);
    
    // Test with a very large value
    printf("\nTest 4: make_bignum(2147483647)\n");
    b = make_bignum(2147483647);
    gmp_printf("Result: %Zd\n", b->mp_int);
    mpz_clear(b->mp_int);
    free(b);
    
    printf("\nAll tests completed successfully!\n");
    return 0;
}
