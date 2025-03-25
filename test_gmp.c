#include <stdio.h>
#include <gmp.h>

int main() {
    mpz_t z;
    
    printf("Testing mpz_init_set_si with various values...\n");
    
    // Test with small value (should fit in a small integer)
    printf("Testing with value 0...\n");
    mpz_init_set_si(z, 0);
    gmp_printf("Result: %Zd\n", z);
    mpz_clear(z);
    
    // Test with a larger value
    printf("Testing with value 1000000...\n");
    mpz_init_set_si(z, 1000000);
    gmp_printf("Result: %Zd\n", z);
    mpz_clear(z);
    
    // Test with a negative value
    printf("Testing with value -1000000...\n");
    mpz_init_set_si(z, -1000000);
    gmp_printf("Result: %Zd\n", z);
    mpz_clear(z);
    
    // Test with a very large value
    printf("Testing with value 2147483647 (INT_MAX)...\n");
    mpz_init_set_si(z, 2147483647);
    gmp_printf("Result: %Zd\n", z);
    mpz_clear(z);
    
    printf("All tests completed successfully!\n");
    return 0;
}
