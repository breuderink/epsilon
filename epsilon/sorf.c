#include "epsilon.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

void static inline wht_butterfly(float *const s, float *const d) {
	float temp = *s;
	*s += *d;
	*d = temp - *d;
}

// Perform in-place Fast Walsh-Hadamard transform.
void fx_fwht(float *const x, const uint8_t nbits) {
	const size_t n = 1 << nbits;
	for (int width = n; width > 1; width >>= 1) {
		// width halves each iteration.
		for (int block = 0; block < n; block += width) {
			// block shifts by with.
			for (int i = 0; i < (width >> 1); ++i) {
				// i loops to half a block.
				wht_butterfly(x + block + i, x + (width >> 1) + block + i);
			}
		}
	}
}

// SORF contains a multiplication with a diagonal matrix where each
// diagonal element is sampled from the Rademacher distribution. This
// transformation randomly flips the signs of elements in vector x.
uint16_t fx_randflip(float *const x, const size_t n, uint16_t lfsr) {
	assert(lfsr != 0);
	for (int i = 0; i < n; i++) {
		if (lfsr & 1) {
			lfsr ^= 0xb400; // Update the Galois LFSR.
			x[i] *= -1;     // Flip sign.
		}
		lfsr >>= 1;
	}
	return lfsr;
}

// Pseudorandom matrix transformation based on fast Walsh-Hadamard
// transform and sign flipping. Based on [2, 3].
//
// [2] Felix, X. Yu, et al. "Orthogonal random features." Advances in Neural
// Information Processing Systems. 2016.
//
// [3] Choromanski, Krzysztof, and Vikas Sindhwani. "Recycling randomness
// with structure for sublinear time kernel expansions." International
// Conference on Machine Learning. 2016.
void fx_sorf(float *const x, const uint8_t nbits) {
	const size_t n = 1 << nbits;
	uint16_t state = 1;

	float s = 1;

	for (int i = 0; i < 3; i++) {
		fx_fwht(x, nbits);
		s *= pow(2, -nbits / 2.);
		state = fx_randflip(x, n, state);
	}

	// Rescale vector to make transformation independent of basis size.
	// Each Hadamard transform scales by 2^{-b/2}, thus we need to correct
	// with 2^{-3b/2}. Further, equation (5) in [2] has an additional scale
	// factor \sqrt{n} = \sqrt{2^b} = (2^b)^{1/2} = 2^{b/2}.
	// Combined, this leads to a correction of 2^{b/2}*2^{-3b/2} = 2^{-b}.

	s *= pow(2, nbits / 2.);
	assert(fabs(1. / s - n) < 1e-4);

	for (int i = 0; i < n; ++i) {
		x[i] *= s;
	}
}

// We may need to increase the dimensionality of a feature
// vector before applying SORF. Function fx_repeat repeats elements in x1
// to form a vector x2. The vectors can be the same vector.
void fx_repeat(float *const x1, const size_t n1, float *const x2,
               const size_t n2) {
	for (int i = 0; i < n2; ++i) {
		if (i < n1) {
			// Copy from x1 to x2.
			x2[i] = x1[i];
		} else {
			// Repeat previously copied elements in x2.
			x2[i] = x2[i - n1];
		}
	}
}