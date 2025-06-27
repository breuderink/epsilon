#include <epsilon.h>
#include <inttypes.h>
#include <stdint.h>
#include <unity.h>

// Use Floyd's cycle finding algorithm to find a cycle. It works by updating
// two "pointers"; the tortoise and the hare. Each time the tortoise
// advances a step, the hare advances two. When the tortoise finishes a
// xorshift32 cycle, the tortoise should have finished its second cycle.
void xorshift32_cycle_length(void) {
	uint32_t tortoise = 0;
	uint32_t hare = tortoise;

	for (uint32_t i = 0; i < 512; ++i) {
		tortoise = xorshift32(tortoise);
		hare = xorshift32(xorshift32(hare));
		TEST_ASSERT_NOT_EQUAL_INT32(tortoise, hare);
	}
	TEST_ASSERT_EQUAL_INT32(tortoise, (uint32_t)2521350846);
}

void xorshift32_cycle_exhaustive(void) {
	uint32_t tortoise = 1;
	uint32_t hare = tortoise;

	for (uint32_t i = 0; i < UINT32_MAX - 1; ++i) {
		tortoise = xorshift32(tortoise);
		hare = xorshift32(xorshift32(hare));
		TEST_ASSERT_NOT_EQUAL_INT32(tortoise, hare);
	}
	tortoise = xorshift32(tortoise);
	hare = xorshift32(xorshift32(hare));
	TEST_ASSERT_EQUAL_INT32_MESSAGE(
	    1, hare, "xorshift32 should cycle in UINT32_MAX - 1 iterations");
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(xorshift32_cycle_length);
#ifdef XORSHIFT32_EXHAUSTIVE
	RUN_TEST(xorshift32_cycle_exhaustive);
#endif
	return UNITY_END();
}
