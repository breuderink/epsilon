#include <epsilon.h>
#include <greatest.h>
#include <math.h>

TEST welford_test_mean() {
	online_stats_t s = {0};

	for (int i = 0; i < 10; ++i) {
		observe(&s, i);
		ASSERT_IN_RANGE(i / 2.0f, mean(&s), 1e-6);
	}
	PASS();
}

TEST welford_test_var() {
	online_stats_t s = {0};

	for (int i = 0; i < 10; ++i) {
		observe(&s, i);
	}
	/*
	>>> import numpy as np
	>>> np.var(np.arange(10))
	8.25
	>>> np.var(np.arange(10), ddof=1)
	9.166666666666666
	*/
	ASSERT_IN_RANGE(8.25, pvariance(&s), 1e-6);
	ASSERT_IN_RANGE(9.1667, variance(&s), 1e-4);
	PASS();
}

TEST welford_edge_cases(float mu, float sigma) {
	online_stats_t s = {0};
	ASSERTm("mean() before observations should be NaN!", isnan(mean(&s)));
	ASSERTm("pvariance() before observations should be NaN!",
	        isnan(pvariance(&s)));
	ASSERTm("variance() before observations should be NaN!",
	        isnan(variance(&s)));

	observe(&s, mu - sigma);
	ASSERT_EQ_FMT(mean(&s), mu - sigma, "%f");
	ASSERT_EQ_FMT(pvariance(&s), 0.0, "%f");
	ASSERTm("variance() after first observation should be NaN!",
	        isnan(variance(&s)));

	observe(&s, mu + sigma);
	ASSERT_EQ_FMT(mean(&s), mu, "%f");
	ASSERT_IN_RANGE(sigma * sigma, pvariance(&s), 1e-4);
	ASSERT_IN_RANGE(sigma * sigma * (2/1), variance(&s), 1e-3);

	PASS();
}

SUITE(Welford) {
	RUN_TEST(welford_test_mean);
	RUN_TEST(welford_test_var);

	RUN_TESTp(welford_edge_cases, 0, 1);
	RUN_TESTp(welford_edge_cases, 1, 1);
	RUN_TESTp(welford_edge_cases, -5, 2);
	RUN_TESTp(welford_edge_cases, 1e4, 0.1);
}
