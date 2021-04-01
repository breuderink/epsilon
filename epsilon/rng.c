#include <stdint.h>

uint32_t xorshift32(uint32_t y) {
	if (y == 0)
		y = 1;
	y ^= (y << 13);
	y ^= (y >> 17);
	y ^= (y << 5);
	return y;
}
