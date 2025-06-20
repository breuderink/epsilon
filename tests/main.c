#include <unity.h>

void setUp(void) {
	// Optional setup code, if needed
}

void tearDown(void) {
	// Optional teardown code, if needed
}

void run_hash_tests(void);
void run_welford_tests(void);
void run_kpa_tests(void);

int main(void) {
	UNITY_BEGIN();

	// Run tests from each file (no suites in Unity, so call directly)
	// run_rng_tests();
	run_hash_tests();
	run_kpa_tests();
	// run_sorf_tests();
	run_welford_tests();

	return UNITY_END();
}
