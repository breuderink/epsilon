#include "epsilon.h"
#include <assert.h>
#include <greatest.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Find number of non-support vectors.
size_t KP_num_idle(const KP_t *km);

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

static float quadratic_kernel(size_t i, size_t j) {
	float k = linear_kernel(i, j);
	return k * k;
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

TEST test_KPA_regression() {
	const size_t N = 10;
	const float COST[] = {INFINITY, 100, 1e-4};
	const float MARGIN[] = {0, 0.1, 1};

	for (int c = 0; c < sizeof(COST) / sizeof(COST[0]); ++c) {
		for (int m = 0; m < sizeof(MARGIN) / sizeof(MARGIN[0]); ++m) {
			// Configure PA update.
			PA_t PA = {.C = COST[c], .eps = MARGIN[m]};

			// Initialize support vectors.
			float X[N][FEATURE_DIMS] = {0};
			support_vectors = &X;

			// Define regressor.
			float alpha[N] = {0};
			KP_t regressor = {
			    .alpha = alpha,
			    .num_alpha = N,
			    .kernel = &quadratic_kernel,
			};

			for (size_t x_i = 0; x_i < N; ++x_i) {
				// Add input to kernel.
				assert(regressor.alpha[x_i] == 0);
				float input = 10 * (rand() / (float)RAND_MAX) - 5;
				X[x_i][0] = input;

				// Update model.
				float target = input * input - 10;
				float pred_before = KPA_regress(&regressor, PA, x_i, target);
				float pred_after = KPA_regress(&regressor, PA, x_i, NAN);

				ASSERT_GTEm("Error should not get larger!",
				            fabsf(pred_before - target),
				            fabsf(pred_after - target));
				if (isinf(PA.C)) {
					ASSERT_GTEm("Loss should be zero after update with C=inf!",
					            PA.eps + 1e-4, fabsf(pred_after - target));
				}
			}
		}
	}

	PASS();
}

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
	RUN_TEST(test_kernel_projection);
	RUN_TEST(test_KPA_regression);
	RUN_TEST(test_idle);
}
