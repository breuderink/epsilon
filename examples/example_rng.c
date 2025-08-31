#include "epsilon/rng.h"
#include <inttypes.h>
#include <stdio.h>

int main(void) {
	// Initialize value to seed 0.
	uint32_t value = 0;

	// Generate sequence of pseudo-random numbers based on seed.
	for (int i = 0; i < 5; ++i) {
		value = xorshift32(value);
		printf("%" PRIu32 "\n", value);
	}
	return 0;
}
