#include "kpa.h"
#include <greatest.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define FEATURE_DIMS 2
#define SUPPORT_VECTORS 5

// Use global pointer to point kernel to a set of instances.
static void *support_vectors;
static float linear_kernel(size_t i, size_t j) {
	float k = 1; // 1 for bias term.
	float (*X)[FEATURE_DIMS] = support_vectors;
	for (int f = 0; f < FEATURE_DIMS; ++f) {
		k += X[i][f] * X[j][f];
	}
	return k;
}

TEST test_kernel_projection() {
	SKIP();
}

TEST test_PA_regression() {
	SKIP();
}

TEST test_KPA_regression() {
	SKIP();
}


TEST test_idle() {
	float X[SUPPORT_VECTORS][FEATURE_DIMS];
	float alpha[SUPPORT_VECTORS] = {0, 2, 3, 5, 0};

	support_vectors = &X;
	KP_t km = {.alpha = alpha, .num_alpha= SUPPORT_VECTORS, .kernel = &linear_kernel};
	ASSERTm("there should be 2 idle SVs!", KP_num_idle(&km) == 2);

	ASSERTm("first idle SV should be in position 0!", KP_find_idle(&km, 0) == 0);
	ASSERTm("second idle SV should be in position 4!", KP_find_idle(&km, 1) == 4);

	km.alpha[0] = -1;
	ASSERTm("there should be 1 idle SVs!", KP_num_idle(&km) == 1);
	ASSERTm("first idle SV should be in position 4!", KP_find_idle(&km, 0) == 4);

	// TODO: should we test out-of-range calls?
	return 0;
}

SUITE(KPA_tests) {
	RUN_TEST(test_PA_regression);
	RUN_TEST(test_kernel_projection);
	RUN_TEST(test_KPA_regression);
	RUN_TEST(test_idle);
}
