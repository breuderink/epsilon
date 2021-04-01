#include <stdint.h>
#include <assert.h>

uint32_t xorshift32(uint32_t y) {
	assert(y != 0);
	y ^= (y << 13);
	y ^= (y >> 17);
	y ^= (y << 5);
	return y;
}
