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

TEST welford_test_var100() {
	online_stats_t s = {0};

	for (int i = 0; i < 10; ++i) {
		observe(&s, i);
	}
	ASSERT_IN_RANGE(8.25, var(&s), 1e-6);
	PASS();
}

TEST welford_test_var(float mu, float sigma) {
	online_stats_t s = {0};

	for (int i = 0; i < 10; ++i) {
		observe(&s, mu + (i % 2) * sigma);
		if (i > 0) {
			ASSERT_IN_RANGE(sigma * sigma, var(&s), 1e-4);
		}
	}
	PASS();
}

TEST welford_edge_cases(float mu, float sigma) {
	online_stats_t s = {0};
	ASSERTm("Mean before observations should be NaN!", isnan(mean(&s)));
	ASSERTm("Variance before observations should be NaN!", isnan(var(&s)));

	observe(&s, mu - sigma);
	ASSERT_EQ_FMT(mean(&s), mu - sigma, "%f");
	ASSERT_EQ_FMT(var(&s), 0.0, "%f");

	observe(&s, mu + sigma);
	ASSERT_EQ_FMT(mean(&s), mu, "%f");
	ASSERT_IN_RANGE(sigma * sigma, var(&s), 1e-4);

	PASS();
}

SUITE(Welford) {
	RUN_TEST(welford_test_mean);
	RUN_TEST(welford_test_var100);

	RUN_TESTp(welford_test_var, 0, 1);
	RUN_TESTp(welford_test_var, 1e4, 0.1);

	RUN_TESTp(welford_edge_cases, 0, 1);
	RUN_TESTp(welford_edge_cases, 1, 1);
	RUN_TESTp(welford_edge_cases, -5, 2);
	RUN_TESTp(welford_edge_cases, 1e4, 0.1);
}
