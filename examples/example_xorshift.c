#include <epsilon.h>
#include <stdio.h>
#include <inttypes.h>

int main() { 
    // Initialize value to seed 0.
    uint32_t value = 0; 
    
    // Generate sequence of pseudo-random numbers based on seed.
    for (int i = 0; i < 5; ++i) {
        value = xorshift32(value); 
        printf("%" PRIu32 "\n", value);
    }
    return 0; 
}
