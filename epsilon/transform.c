#include "epsilon/transform.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

inline static void WHT_butterfly(float *const s, float *const d) {
	float temp = *s;
	*s += *d;
	*d = temp - *d;
}

// Perform in-place Fast Walsh-Hadamard transform.
void FWHT(float *const x, const uint8_t nbits) {
	const size_t n = 1 << nbits;
	for (size_t width = n; width > 1; width >>= 1) {
		// width halves each iteration.
		for (size_t block = 0; block < n; block += width) {
			// block shifts by with.
			for (size_t i = 0; i < (width >> 1); ++i) {
				// i loops to half a block.
				WHT_butterfly(x + block + i, x + (width >> 1) + block + i);
			}
		}
	}
}
