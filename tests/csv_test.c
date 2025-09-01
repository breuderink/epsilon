#include <stddef.h>
#include <string.h>
#include <unity.h>

#define CSV_RECORD_SIZE 4096
#define CSV_FIELDS 512

typedef struct {
	char buffer[CSV_RECORD_SIZE];
	char *fields[CSV_FIELDS];
	size_t num_fields;
} csv_record_t;

size_t csv_parse_record(const char *s, size_t len, csv_record_t *r) {
	(void)s;
	(void)len;
	r->num_fields = 0;
	return 0;
}

typedef struct {
	const char *input, *expected[10];
	size_t expected_count;
} csv_test_case;

void test_parse_field(void) {
	csv_test_case cases[] = {
	    {"single", {"single"}, 1},
	    {"first, second, third", {"first", "second", "third"}, 3},
	};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		csv_test_case c = cases[i];

		csv_record_t r = {0};
		csv_parse_record(c.input, strlen(c.input), &r);

		TEST_ASSERT_EQUAL_STRING_ARRAY(c.expected, r.fields, c.expected_count);
	}
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_parse_field);
	return UNITY_END();
}
