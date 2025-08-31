#include <hash.h>
#include <string.h>
#include <unity.h>

void test_FNV1a32(void) {
	const char *str = "test";
	TEST_ASSERT_EQUAL_HEX32(0xafd071e5, FNV1a32_hash(str, strlen(str)));
	str = "Test";
	TEST_ASSERT_EQUAL_HEX32(0x2ffcbe05, FNV1a32_hash(str, strlen(str)));
	TEST_ASSERT_EQUAL_HEX32(0x811c9dc5, FNV1a32_hash(NULL, 0));
	TEST_ASSERT_EQUAL_HEX32(0x340ca71c,
	                        FNV1a32_update(FNV1a32_hash(NULL, 0), '1'));
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_FNV1a32);
	return UNITY_END();
}
