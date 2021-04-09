#include <epsilon.h>
#include <greatest.h>

TEST welford_edge_cases() {
	online_stats_t s = {0};

	ASSERT_EQ_FMT(mean(&s), 0, "%f");
	PASS();
}

SUITE(Welford) {
	RUN_TEST(welford_edge_cases);
}
