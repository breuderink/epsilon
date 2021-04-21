#include <epsilon.h>
#include <greatest.h>
#include <string.h>

TEST FNV1a32_test() {
	// Target hashes were computed with https://sha256calc.com/hash/fnv1a32.
	const char *str = "test";
	ASSERT_EQ_FMT(0xafd071e5, FNV1a_hash(str, strlen(str)), "%#10x");

	str = "Test";
	ASSERT_EQ_FMT(0x2ffcbe05, FNV1a_hash(str, strlen(str)), "%#10x");

	// Test initial value (i.e. VNV offset basis.)
	ASSERT_EQ_FMT(0x811c9dc5, FNV1a_hash(NULL, 0), "%#10x");

	// Test update step in isolation.
	ASSERT_EQ_FMT(0x340ca71c, FNV1a_update(FNV1a_hash(NULL, 0), '1'), "%#10x");

	PASS();
}

SUITE(hash) { RUN_TEST(FNV1a32_test); }
