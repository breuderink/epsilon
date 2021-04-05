#include "kpa.h"
#include <greatest.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NFEAT 2
#define NACTIVE 5

// Prepare for easy test setup.
typedef float X_t[NACTIVE][NFEAT];
typedef float alpha_t[NACTIVE];

static float lin_kernel(void *instances, size_t i, size_t j) {
	float(*x)[NFEAT] = (float(*)[NFEAT])instances;

	float k = 1; // 1 for bias term.
	for (int f = 0; f < NFEAT; ++f) {
		k += x[i][f] * x[j][f];
	}
	return k;
}

static km_t setup(X_t X, alpha_t a) {
	return (km_t){.instances = X,
	              .alpha = a,
	              .num_alpha = NACTIVE,
	              .kernel = &lin_kernel};
}

TEST test_idle() {
	km_t km = setup((X_t){{0}}, (alpha_t){0, 2, 3, 5, 0});
	ASSERTm("there should be 2 idle SVs!", km_num_idle(&km) == 2);

	ASSERTm("first idle SV should be in position 0!", km_idle(&km, 0) == 0);
	ASSERTm("second idle SV should be in position 4!", km_idle(&km, 1) == 4);

	km.alpha[0] = -1;
	ASSERTm("there should be 1 idle SVs!", km_num_idle(&km) == 1);
	ASSERTm("first idle SV should be in position 4!", km_idle(&km, 0) == 4);

	// TODO: should we test out-of-range calls?
	return 0;
}

SUITE(KPA_tests) {
	RUN_TEST(test_idle);
}