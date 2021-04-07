#include <stdint.h>

// Algorithm `xor` on page 4 of [1].
//
// [1] Marsaglia, George. "Xorshift RNGs." Journal of Statistical Software 8.14
//     (2003): 1-6.
uint32_t xorshift32(uint32_t y) {
	if (y == 0)
		y = 1;
	y ^= (y << 13);
	y ^= (y >> 17);
	y ^= (y << 5);
	return y;
}
