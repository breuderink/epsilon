#include "epsilon/transform.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

#define L2D 8
#define DIMS (1 << L2D)

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

#

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_FWHT);
	return UNITY_END();
}
