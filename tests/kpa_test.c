#include "kpa.h"
#include <greatest.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define FEATURE_DIMS 2
#define SUPPORT_VECTORS 5

// Prepare for easy test setup.
typedef float X_t[SUPPORT_VECTORS][FEATURE_DIMS];
typedef float alpha_t[SUPPORT_VECTORS];

static float linear_kernel(void *instances, size_t i, size_t j) {
	float(*x)[FEATURE_DIMS] = (float(*)[FEATURE_DIMS])instances;

	float k = 1; // 1 for bias term.
	for (int f = 0; f < FEATURE_DIMS; ++f) {
		k += x[i][f] * x[j][f];
	}
	return k;
}

static KP_t setup(X_t X, alpha_t a) {
	return (KP_t){.instances = X,
	              .alpha = a,
	              .num_alpha = SUPPORT_VECTORS,
	              .kernel = &linear_kernel};
}

TEST test_idle() {
	KP_t km = setup((X_t){{0}}, (alpha_t){0, 2, 3, 5, 0});
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
	RUN_TEST(test_idle);
}
