#include <epsilon.h>
#include <greatest.h>
#include <inttypes.h>
#include <stdint.h>

// Use Floyd's cycle finding algorithm to find a cycle. It works by updating
// two "pointers"; the tortoise and the hare. Each time the tortoise
// advances a step, the hare advances two. When the tortoise finishes a
// xorshift32 cycle, the tortoise should have finished its second cycle.
TEST xorshift32_cycle_length() {
	uint32_t tortoise = 0;
	uint32_t hare = tortoise;

	for (uint32_t i = 0; i < 512; ++i) {
		tortoise = xorshift32(tortoise);
		hare = xorshift32(xorshift32(hare));
		ASSERT(tortoise != hare);
	}
	ASSERT_EQ_FMT(tortoise, (uint32_t)2521350846, "%" PRIu32);
	PASS();
}

#ifdef XORSHIFT32_EXHAUSTIVE
TEST xorshift32_cycle_exhaustive() {
	uint32_t tortoise = 1;
	uint32_t hare = tortoise;

	for (uint32_t i = 0; i < UINT32_MAX - 1; ++i) {
		tortoise = xorshift32(tortoise);
		hare = xorshift32(xorshift32(hare));
		ASSERT(tortoise != hare);
	}
	tortoise = xorshift32(tortoise);
	hare = xorshift32(xorshift32(hare));
	ASSERT_EQm("xorshift32 should cycle in UINT32_MAX - 1 iterations", 1, hare);
	PASS();
}
#endif

SUITE(RNG) {
	RUN_TEST(xorshift32_cycle_length);
#ifdef XORSHIFT32_EXHAUSTIVE
	RUN_TEST(xorshift32_cycle_exhaustive);
#endif
}
