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

// Use global pointer to point kernel to a set of support vectors.
static void *support_vectors;
static float linear_kernel(size_t i, size_t j) {
	float k = 1; // 1 for bias term.
	const float (*X)[FEATURE_DIMS] = support_vectors;
	for (int f = 0; f < FEATURE_DIMS; ++f) {
		k += X[i][f] * X[j][f];
	}
	return k;
}

static float quadratic_kernel(size_t i, size_t j) {
	float k = linear_kernel(i, j);
	return k * k;
}

static float identity_kernel(size_t i, size_t j) { return i == j; }

TEST test_squared_Euclidean(void) {
	// Set up kernel.
	float X[SUPPORT_VECTORS][FEATURE_DIMS] = {
	    {1, 0}, {2, 0}, {3, 0}, {5, 0}, {0, 7},
	};
	support_vectors = &X;

	/*
	>>> import numpy as np
	>>> X = np.asarray([[1,0],[2,0],[3,0],[5,0],[0,7]])
	>>> from sklearn.metrics import pairwise
	>>> pairwise.euclidean_distances(X)**2
	array([[ 0.,  1.,  4., 16., 50.],
	       [ 1.,  0.,  1.,  9., 53.],
	       [ 4.,  1.,  0.,  4., 58.],
	       [16.,  9.,  4.,  0., 74.],
	       [50., 53., 58., 74.,  0.]])
	*/

	ASSERT_IN_RANGE(0, squared_Euclidean(linear_kernel, 0, 0), 1e-8);
	ASSERT_IN_RANGE(1, squared_Euclidean(linear_kernel, 1, 0), 1e-8);
	ASSERT_IN_RANGE(0, squared_Euclidean(linear_kernel, 1, 1), 1e-8);
	ASSERT_IN_RANGE(4, squared_Euclidean(linear_kernel, 2, 0), 1e-8);
	ASSERT_IN_RANGE(74, squared_Euclidean(linear_kernel, 4, 3), 1e-8);
	PASS();
}

TEST test_squared_exponential_kernel(void) {
	// Test known values (see https://www.desmos.com/calculator/gwvqopaqkv).
	ASSERT_IN_RANGE(0.9460, squared_exponential_kernel(3, 1 * 1), 1e-4);
	ASSERT_IN_RANGE(0.2494, squared_exponential_kernel(3, 5 * 5), 1e-4);

	const float SIGMA[] = {0.1, 1, 100};
	for (size_t s = 0; s < sizeof(SIGMA) / sizeof(SIGMA[0]); ++s) {
		// Test extrema.
		float sigma = SIGMA[s];
		ASSERT_IN_RANGE(1, squared_exponential_kernel(sigma, 0), 1e-8);
		ASSERT_IN_RANGE(0, squared_exponential_kernel(sigma, INFINITY), 1e-8);
	}

	// Test that larger values of sigma have wider influence.
	ASSERT(squared_exponential_kernel(1, 1) < squared_exponential_kernel(2, 1));

	PASS();
}

TEST test_kernel_projection(void) {
	// Set up kernel.
	float X[SUPPORT_VECTORS][FEATURE_DIMS] = {
	    {1, 0}, {2, 0}, {3, 0}, {5, 0}, {0, 7},
	};
	support_vectors = &X;

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

TEST test_KPA_regression(void) {
	const float COST[] = {INFINITY, 100, 1e-4};
	const float MARGIN[] = {0, 0.1, 1};

	for (size_t c = 0; c < sizeof(COST) / sizeof(COST[0]); ++c) {
		for (size_t m = 0; m < sizeof(MARGIN) / sizeof(MARGIN[0]); ++m) {
			// Configure PA update.
			PA_t PA = {.C = COST[c], .eps = MARGIN[m]};

			// Re-initialize support vectors. They are modified below.
			float X[SUPPORT_VECTORS][FEATURE_DIMS] = {0};
			support_vectors = &X;

			// Define regressor.
			float alpha[SUPPORT_VECTORS] = {0};
			KP_t regressor = {
			    .alpha = alpha,
			    .num_alpha = SUPPORT_VECTORS,
			    .kernel = &quadratic_kernel,
			};

			for (size_t x_i = 0; x_i < SUPPORT_VECTORS; ++x_i) {
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

TEST test_idle(void) {
	float alpha[SUPPORT_VECTORS] = {0, 2, 3, 5, 0};
	KP_t km = {.alpha = alpha,
	           .num_alpha = SUPPORT_VECTORS,
	           .kernel = &identity_kernel};
	ASSERTm("there should be 2 idle SVs!", KP_num_idle(&km) == 2);

	ASSERTm("first idle SV should be in position 0!",
	        KP_find_idle(&km, 0) == 0);
	ASSERTm("second idle SV should be in position 4!",
	        KP_find_idle(&km, 1) == 4);

	km.alpha[0] = -1;
	ASSERTm("there should be 1 idle SVs!", KP_num_idle(&km) == 1);
	ASSERTm("first idle SV should be in position 4!",
	        KP_find_idle(&km, 0) == 4);

	return 0;
}

float BPA_simple(KP_t *kp, size_t t);
TEST test_BPA_simple(void) {
	// Set up kernel and projection.
	float X[SUPPORT_VECTORS][FEATURE_DIMS] = {
	    {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}};
	support_vectors = &X;
	KP_t km = {
	    .alpha = (float[SUPPORT_VECTORS]){1, -2, 3, -5, 7},
	    .num_alpha = SUPPORT_VECTORS,
	    .kernel = &linear_kernel,
	};

	size_t target = 0;
	float prev_loss = 0;
	for (size_t i = 0; i < SUPPORT_VECTORS - 1; ++i) {
		size_t idle_before = KP_num_idle(&km);
		float loss = BPA_simple(&km, target);
		size_t idle_after = KP_num_idle(&km);

		ASSERT(km.alpha[target] != 0);
		ASSERT_EQ_FMTm("BPA simple should absorb a single support vector!",
		               idle_before + 1, idle_after, "%zu");
		ASSERTm("BPA-simple loss should > 0!", loss > 0);
		ASSERTm("BPA-simple should absorb support vector with minimum loss!",
		        prev_loss <= loss);
		prev_loss = loss;
	}
	PASS();
}

TEST test_BKPA_regression(void) {
	PA_t PA = {.C = INFINITY, .eps = 0};

	// Define regressor.
	float alpha[SUPPORT_VECTORS] = {0};
	KP_t regressor = {
	    .alpha = alpha,
	    .num_alpha = SUPPORT_VECTORS,
	    .kernel = &identity_kernel,
	};

	for (size_t pass = 0; pass < 3; ++pass) {
		for (size_t t = 0; t < SUPPORT_VECTORS; ++t) {
			size_t x_i = KP_find_idle(&regressor, 0);
			float target = pass + 1;
			BKPA_regress(&regressor, PA, x_i, target);

			float pred_after = BKPA_regress(&regressor, PA, x_i, NAN);
			GREATEST_ASSERT_IN_RANGEm(
			    "BKPA loss should be zero on last example!", 0,
			    pred_after - target, 1e-8);

			size_t idle = KP_num_idle(&regressor);
			GREATEST_ASSERTm("BKPA should maintain budget!", idle >= 1);
		}
	}

	PASS();
}

SUITE(KPA_tests) {
	RUN_TEST(test_squared_Euclidean);
	RUN_TEST(test_squared_exponential_kernel);
	RUN_TEST(test_kernel_projection);
	RUN_TEST(test_KPA_regression);
	RUN_TEST(test_idle);
	RUN_TEST(test_BPA_simple);
	RUN_TEST(test_BKPA_regression);
}
