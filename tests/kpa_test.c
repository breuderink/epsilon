#include "kpa.h"
#include <assert.h>
#include <greatest.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define FEATURE_DIMS 2
#define SUPPORT_VECTORS 5

// Use global pointer to point kernel to a set of instances.
static void *support_vectors;
static float linear_kernel(size_t i, size_t j) {
	float k = 1; // 1 for bias term.
	float(*X)[FEATURE_DIMS] = support_vectors;
	for (int f = 0; f < FEATURE_DIMS; ++f) {
		k += X[i][f] * X[j][f];
	}
	return k;
}

TEST test_kernel_projection() {
	// Set up kernel.
	float X[SUPPORT_VECTORS][FEATURE_DIMS] = {
	    {1, 0}, {2, 0}, {3, 0}, {5, 0}, {0, 7},
	};

	/* Test kernel.
	>>> import numpy as np
	>>> X = np.asarray([[1,0],[2,0],[3,0],[5,0],[0,7]])
	>>> K = X@X.T + 1
	>>> K
	array([[ 2,  3,  4,  6,  1],
	       [ 3,  5,  7, 11,  1],
	       [ 4,  7, 10, 16,  1],
	       [ 6, 11, 16, 26,  1],
	       [ 1,  1,  1,  1, 50]])
	*/
	support_vectors = &X;
	ASSERT_IN_RANGE(25 + 1, linear_kernel(3, 3), 1e-8);
	ASSERT_IN_RANGE(1, linear_kernel(4, 0), 1e-8);
	ASSERT_IN_RANGE(49 + 1, linear_kernel(4, 4), 1e-8);

	// Set up projection.
	float alpha[SUPPORT_VECTORS] = {1, -2, 3, -5, 7};
	KP_t km = {
	    .alpha = alpha,
	    .num_alpha = SUPPORT_VECTORS,
	    .kernel = &linear_kernel,
	};

	/* Test projection.
	>>> a = np.asarray([1, -2, 3, -5, 7])
	>>> K @ a
	array([-15, -34, -53, -91, 347])
	*/
	ASSERT_IN_RANGE(-15, KP_apply(&km, 0), 1e-8);
	ASSERT_IN_RANGE(-34, KP_apply(&km, 1), 1e-8);
	ASSERT_IN_RANGE(-53, KP_apply(&km, 2), 1e-8);
	ASSERT_IN_RANGE(-91, KP_apply(&km, 3), 1e-8);
	ASSERT_IN_RANGE(347, KP_apply(&km, 4), 1e-8);

	PASS();
}

TEST test_PA_regression() {
	for (float eps = 0; eps < 5; eps += 0.1) {
		// Specify margin with hard updates.
		PA_t hard = {.C = INFINITY, .eps = eps};
		for (float target = -5; target < 5; target += 0.1) {
			for (float pred = -3; pred < 3; pred += 0.1) {
				float change = PA1_regress_update(hard, pred, target);
				ASSERT(isfinite(change));

				if (fabsf(pred - target) > eps) {
					// Aggressive update.
					ASSERT_IN_RANGE(eps, fabs(pred + change - target), 1e-4);
				} else {
					// Passive, no update is needed.
					ASSERT_EQ_FMT(0.0, change, "%.f");
				}
			}
		}
	}

	// Test soft variant.
	PA_t soft = {.C = 1, .eps = 0};
	for (float target = -10; target <= 10; ++target) {
		float change = PA1_regress_update(soft, 0, target);
		ASSERT(fabsf(change) <= soft.C);
	}

	PASS();
}

TEST test_KPA_regression() { SKIP(); }

TEST test_idle() {
	float X[SUPPORT_VECTORS][FEATURE_DIMS];
	float alpha[SUPPORT_VECTORS] = {0, 2, 3, 5, 0};

	support_vectors = &X;
	KP_t km = {
	    .alpha = alpha, .num_alpha = SUPPORT_VECTORS, .kernel = &linear_kernel};
	ASSERTm("there should be 2 idle SVs!", KP_num_idle(&km) == 2);

	ASSERTm("first idle SV should be in position 0!",
	        KP_find_idle(&km, 0) == 0);
	ASSERTm("second idle SV should be in position 4!",
	        KP_find_idle(&km, 1) == 4);

	km.alpha[0] = -1;
	ASSERTm("there should be 1 idle SVs!", KP_num_idle(&km) == 1);
	ASSERTm("first idle SV should be in position 4!",
	        KP_find_idle(&km, 0) == 4);

	// TODO: should we test out-of-range calls?
	return 0;
}

SUITE(KPA_tests) {
	RUN_TEST(test_PA_regression);
	RUN_TEST(test_kernel_projection);
	RUN_TEST(test_KPA_regression);
	RUN_TEST(test_idle);
}
