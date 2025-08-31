#include <stdint.h>

// Marsaglia's xorshift random number generator [1];
uint32_t xorshift32(uint32_t y);

/*
# References

[1] Marsaglia, George. "Xorshift RNGs." Journal of Statistical Software 8.14
    (2003): 1-6.

*/
