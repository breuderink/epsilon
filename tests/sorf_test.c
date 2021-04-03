#include <epsilon.h>
#include <greatest.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FX_UNIT 1.

int tests_run = 0;

#define FX_TEST_L2D 8
#define FX_TEST_D (1 << FX_TEST_L2D)

uint16_t fx_randflip(float *const x, const size_t n, uint16_t lfsr);

TEST test_fwht() {
	// Test properties:
	// 1). W W = n I.
	// 2). W \neq \sqrt(n) I.

	float x0[FX_TEST_D];
	float x[FX_TEST_D];

	for (int d = 0; d < FX_TEST_D; ++d) {
		// Set x and x0 to unit vector.
		memset(x0, 0, sizeof(x));
		x0[d] = FX_UNIT;
		memcpy(x, x0, sizeof(x));

		fx_fwht(x, FX_TEST_L2D);
		for (int i = 0; i < FX_TEST_D; ++i) {
			// Test property 2.
			ASSERTm("Expected fwht(x) in {-1, 1}^d!",
			        x[i] == FX_UNIT || x[i] == -FX_UNIT);
		}

		fx_fwht(x, FX_TEST_L2D);
		for (int i = 0; i < 1 << FX_TEST_L2D; ++i) {
			// Test property 1.
			ASSERTm("Expected fwht(fwht(x)) == d x!",
			        x[i] == FX_TEST_D * x0[i]);
		}
	}

	PASS();
}

TEST test_sorf() {
	float x0[FX_TEST_D];
	float x[FX_TEST_D];

	for (int d = 0; d < FX_TEST_D; ++d) {
		// Set x and x0 to varying unit vector.
		memset(x0, 0, sizeof(x));
		x0[d] = FX_UNIT;
		memcpy(x, x0, sizeof(x));
		fx_sorf(x, FX_TEST_L2D);

		// Check for the right normalization. We know that the HDHDHD steps
		// preserve the norm. SORF contains an additional scale factor of
		// D^{1/2}. Hence, we have D^{1/2} |x| = |SORF(x)|, or
		// D |x|^2 = |SORF(x)|^2.
		float ss_before = 0;
		float ss_after = 0;

		for (int i = 0; i < FX_TEST_D; ++i) {
			ss_before += x0[i] * (x0[i] / (float)FX_UNIT);
			ss_after += x[i] * (x[i] / (float)FX_UNIT);
		}
		ASSERTm("Expected SORF normalization of D^(-1/2)!",
		        fabs(FX_TEST_D * ss_before - ss_after) < 1e-6);

		// Check SORF mixing, which is the main use of the SORF transform.
		// Mixing implies that a sparse vector becomes a dense vector.
		int nonzero = 0;
		for (int i = 0; i < FX_TEST_D; ++i) {
			nonzero += x[i] != 0;
		}
		ASSERTm("Expected more than one non-zero element in SORF(x)",
		        nonzero > 1);
	}

	PASS();
}

TEST test_randflip() {
	// Initialize a vector with a counting pattern.
	float x[FX_TEST_D];
	for (int i = 0; i < FX_TEST_D; i++) {
		x[i] = i;
	}

	// Flip.
	fx_randflip(x, FX_TEST_D, 1);

	// Count flips.
	int negs = 0;
	for (int i = 0; i < FX_TEST_D; i++) {
		ASSERTm("randflip8 should only change sign", x[i] == -i || x[i] == i);
		negs += x[i] < 0;
	}

	// Use approximate biomial 95% confidence interval as test.
	float p_hat = (float)negs / FX_TEST_D;
	float ci = 1.96 * sqrt(p_hat * (1 - p_hat) / FX_TEST_D);
	ASSERTm("randflip flipped unexpected fraction of array",
	        0.5 - ci < p_hat && p_hat < 0.5 + ci);

	PASS();
}

TEST test_repeat() {
	float source[FX_TEST_D], target[FX_TEST_D];
	for (int i = 0; i < FX_TEST_D; i++) {
		source[i] = i;
	}

	// Test copy repeating.
	fx_repeat(source, 3, target, FX_TEST_D);
	for (int i = 0; i < FX_TEST_D; i++) {
		ASSERTm("unexpected value in copy repeat", target[i] == source[i % 3]);
	}

	// Test in-place repeating.
	fx_repeat(source, 3, source, FX_TEST_D);
	for (int i = 0; i < FX_TEST_D; i++) {
		ASSERTm("unexpected value in in-place repeat",
		        source[i] == source[i % 3]);
	}

	PASS();
}

SUITE(sorf) {
	RUN_TEST(test_fwht);
	RUN_TEST(test_sorf);
	RUN_TEST(test_randflip);
	RUN_TEST(test_repeat);
}
