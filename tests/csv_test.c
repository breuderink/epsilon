#include "csv.h"
#include <string.h>
#include <unity.h>

size_t parse_csv_row(char *buf, csv_row_t *row);
void test_parse_csv_row(void) {
	csv_row_t row;
	char line[80] = "field1,field2,field3";
	size_t n_fields = parse_csv_row(line, &row);
	TEST_ASSERT_EQUAL(3, n_fields);
	TEST_ASSERT_EQUAL_STRING("field1", row.fields[0]);
	TEST_ASSERT_EQUAL_STRING("field2", row.fields[1]);
	TEST_ASSERT_EQUAL_STRING("field3", row.fields[2]);
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_parse_csv_row);
	return UNITY_END();
}
