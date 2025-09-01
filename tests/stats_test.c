#include "stats.h"
#include <math.h>
#include <unity.h>

// Test for mean calculation
void test_welford_mean(void) {
	online_stats_t s = {0};
	for (int i = 0; i < 10; ++i) {
		observe(&s, i);
		TEST_ASSERT_FLOAT_WITHIN(1e-6, i / 2.0f, mean(&s));
	}
}

// Test for variance calculations
void test_welford_var(void) {
	online_stats_t s = {0};
	for (int i = 0; i < 10; ++i) {
		observe(&s, i);
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-6, 8.25, pvariance(&s));
	TEST_ASSERT_FLOAT_WITHIN(1e-4, 9.1667, variance(&s));
}

// Test for edge cases using a parameter loop
void test_welford_edge_cases(void) {
	// Define parameter sets for mu and sigma
	struct {
		float mu;
		float sigma;
	} params[] = {{0.0f, 1.0f}, {1.0f, 1.0f}, {-5.0f, 2.0f}, {1e4f, 0.1f}};

	// Loop over each parameter set
	for (size_t i = 0; i < sizeof(params) / sizeof(params[0]); i++) {
		float mu = params[i].mu;
		float sigma = params[i].sigma;

		// Initialize stats
		online_stats_t s = {0};

		// Test before observations
		TEST_ASSERT_MESSAGE(isnan(mean(&s)),
		                    "mean() before observations should be NaN!");
		TEST_ASSERT_MESSAGE(isnan(pvariance(&s)),
		                    "pvariance() before observations should be NaN!");
		TEST_ASSERT_MESSAGE(isnan(variance(&s)),
		                    "variance() before observations should be NaN!");

		// Test after one observation
		observe(&s, mu - sigma);
		TEST_ASSERT_EQUAL_FLOAT(mu - sigma, mean(&s));
		TEST_ASSERT_EQUAL_FLOAT(0.0, pvariance(&s));
		TEST_ASSERT_MESSAGE(
		    isnan(variance(&s)),
		    "variance() after first observation should be NaN!");

		// Test after two observations
		observe(&s, mu + sigma);
		TEST_ASSERT_EQUAL_FLOAT(mu, mean(&s));
		TEST_ASSERT_FLOAT_WITHIN(1e-4, sigma * sigma, pvariance(&s));
		TEST_ASSERT_FLOAT_WITHIN(1e-3, sigma * sigma * 2, variance(&s));
	}
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_welford_mean);
	RUN_TEST(test_welford_var);
	RUN_TEST(test_welford_edge_cases);
	return UNITY_END();
}
