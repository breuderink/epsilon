#include <epsilon.h>
#include <greatest.h>
#include <math.h>

TEST welford_test_mean() { FAILm("Implement me."); }

TEST welford_test_var() { FAILm("Implement me."); }

TEST welford_edge_cases(float mu, float sigma) {
	online_stats_t s = {0};
	ASSERTm("Mean before observations should be NaN!", isnan(mean(&s)));
	ASSERTm("Variance before observations should be NaN!", isnan(var(&s)));

	observe(&s, mu - sigma);
	ASSERT_EQ_FMT(mean(&s), mu - sigma, "%f");
	ASSERT_EQ_FMT(var(&s), 0.0, "%f");

	observe(&s, mu + sigma);
	ASSERT_EQ_FMT(mean(&s), mu, "%f");
	ASSERT_IN_RANGE(var(&s), sigma * sigma, 1e-4);

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
