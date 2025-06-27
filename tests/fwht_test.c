#include <epsilon.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

#define L2D 8
#define DIMS (1 << L2D)

uint16_t SORF_randflip(float *const x, const size_t n, uint16_t lfsr);

void test_FWHT(void) {
	// Test properties:
	// 1). W W = n I.
	// 2). W \neq \sqrt(n) I.

	float x0[DIMS];
	float x[DIMS];

	for (int d = 0; d < DIMS; ++d) {
		// Set x and x0 to unit vector.
		memset(x0, 0, sizeof(x));
		x0[d] = 1;
		memcpy(x, x0, sizeof(x));

		FWHT(x, L2D);
		for (int i = 0; i < DIMS; ++i) {
			// Test property 2.
			TEST_ASSERT_MESSAGE(x[i] == 1 || x[i] == -1,
			                    "Expected FWHT(x) in {-1, 1}^d!");
		}

		FWHT(x, L2D);
		for (int i = 0; i < 1 << L2D; ++i) {
			// Test property 1.
			TEST_ASSERT_EQUAL_MESSAGE(DIMS * x0[i], x[i],
			                          "Expected FWHT(FWHT(x)) == d x!");
		}
	}
}

#ifdef SORF_ENABLED
void test_SORF(void) {
	float x0[DIMS];
	float x[DIMS];

	for (int d = 0; d < DIMS; ++d) {
		// Set x and x0 to varying unit vector.
		memset(x0, 0, sizeof(x));
		x0[d] = 1;
		memcpy(x, x0, sizeof(x));
		SORF(x, L2D);

		// Check for the right normalization. We know that the HDHDHD steps
		// preserve the norm. SORF contains an additional scale factor of
		// D^{1/2}. Hence, we have D^{1/2} |x| = |SORF(x)|, or
		// D |x|^2 = |SORF(x)|^2.
		float ss_before = 0;
		float ss_after = 0;

		for (int i = 0; i < DIMS; ++i) {
			ss_before += x0[i] * x0[i];
			ss_after += x[i] * x[i];
		}
		TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
		    1e-6, DIMS * ss_before, ss_after,
		    "Expected SORF normalization of D^(-1/2)!");

		// Check SORF mixing, which is the main use of the SORF transform.
		// Mixing implies that a sparse vector becomes a dense vector.
		int nonzero = 0;
		for (int i = 0; i < DIMS; ++i) {
			nonzero += x[i] != 0;
		}

		TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(
		    1, nonzero, "Expected more than one non-zero element in SORF(x)");
	}
}

void test_randflip(void) {
	// Initialize a vector with a counting pattern.
	float x[DIMS];
	for (int i = 0; i < DIMS; i++) {
		x[i] = i;
	}

	// Flip.
	SORF_randflip(x, DIMS, 1);

	// Count flips.
	int negs = 0;
	for (int i = 0; i < DIMS; i++) {
		TEST_ASSERT_MESSAGE(x[i] == -i || x[i] == i,
		                    "randflip8 should only change sign");
		negs += x[i] < 0;
	}

	// Use approximate binomial 95% confidence interval as test.
	float p_hat = (float)negs / DIMS;
	float ci = 1.96 * sqrtf(p_hat * (1 - p_hat) / DIMS);
	TEST_ASSERT_MESSAGE(0.5 - ci < p_hat && p_hat < 0.5 + ci,
	                    "randflip flipped unexpected fraction of array");
}

void test_repeat(void) {
	float source[DIMS], target[DIMS];
	for (int i = 0; i < DIMS; i++) {
		source[i] = i;
	}

	// Test copy repeating.
	SORF_repeat(source, 3, target, DIMS);
	for (int i = 0; i < DIMS; i++) {
		TEST_ASSERT_EQUAL_FLOAT_MESSAGE(target[i], source[i % 3],
		                                "unexpected value in copy repeat");
	}

	// Test in-place repeating.
	SORF_repeat(source, 3, source, DIMS);
	for (int i = 0; i < DIMS; i++) {
		TEST_ASSERT_EQUAL_FLOAT_MESSAGE(source[i], source[i % 3],
		                                "unexpected value in in-place repeat");
	}
}
#endif

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_FWHT);

#ifdef SORF_ENABLED
	RUN_TEST(test_SORF);
	RUN_TEST(test_randflip);
	RUN_TEST(test_repeat);
#endif
	return UNITY_END();
}
