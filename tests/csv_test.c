#include "csv.h"
#include <string.h>
#include <unity.h>

// const char *csv_parse_field(const char *r, char *buf, size_t offset);
ssize_t csv_parse_field(const char **in, char **out, size_t *rem) {
	ssize_t n;
	for (n = 0; **in && **in != ',' && **in != '\n' && **in != '\r'; n++) {
		if (*rem <= 1)
			return -1;
		*(*out)++ = *(*in)++;
		(*rem)--;
	}
	// Add null terminator
	*(*out)++ = '\0';
	(*rem)--;
	return n;
}

void test_field_basic(void) {
	const char *input = "field1,...";
	size_t buf_remaining = 80;
	char buf[buf_remaining];
	char *w = buf;

	ssize_t bytes_parsed = csv_parse_field(&input, &w, &buf_remaining);
	TEST_ASSERT_EQUAL(bytes_parsed, strlen("field1"));
	TEST_ASSERT_EQUAL_STRING("field1", buf);
	TEST_ASSERT_EQUAL(buf_remaining, 80 - strlen("field1") - 1);
	TEST_ASSERT_EQUAL(buf + 80, w + buf_remaining);
}

/*

void test_csv_parse_field(void) {
    struct {
        const char *input, *expected;
        ptrdiff_t offset;
    } tests[] = {
        {"field1", "field1", strlen("field1")},
        {"field1,field2", "field1", strlen("field1")},
        {",field2", "", 1},
        {",,field2", "", 1},

    };
    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        char buf[80];
        const char *delim = csv_parse_field(tests[i].input, buf, sizeof(buf));
        printf("Input: '%s' => Parsed: '%s'\n", tests[i].input, buf);
        TEST_ASSERT_EQUAL_STRING(tests[i].expected, buf);
        TEST_ASSERT_EQUAL(tests[i].offset, delim - tests[i].input);
    }
}

size_t csv_parse_row(char *buf, csv_row_t *row);

void test_row_single(void) {
    csv_row_t row;
    char line[] = "singlefield";
    TEST_ASSERT_EQUAL(1, csv_parse_row(line, &row));
    TEST_ASSERT_EQUAL(1, row.n_fields);
    TEST_ASSERT_EQUAL_STRING("singlefield", row.fields[0]);
}

void test_row_basic(void) {
    csv_row_t row;
    char line[] = "field1,field2,field3";
    TEST_ASSERT_EQUAL(3, csv_parse_row(line, &row));
    TEST_ASSERT_EQUAL(3, row.n_fields);
    TEST_ASSERT_EQUAL_STRING("field1", row.fields[0]);
    TEST_ASSERT_EQUAL_STRING("field2", row.fields[1]);
    TEST_ASSERT_EQUAL_STRING("field3", row.fields[2]);
}

void test_row_newline_at_end(void) {
    csv_row_t row;
    char line[] = "field1,field2,field3\n";
    TEST_ASSERT_EQUAL(3, csv_parse_row(line, &row));
    TEST_ASSERT_EQUAL(3, row.n_fields);
    TEST_ASSERT_EQUAL_STRING("field1", row.fields[0]);
    TEST_ASSERT_EQUAL_STRING("field2", row.fields[1]);
    TEST_ASSERT_EQUAL_STRING("field3", row.fields[2]);
}

void test_row_empty_fields(void) {
    csv_row_t row;
    char line[] = "field1,,field3,";
    TEST_ASSERT_EQUAL(4, csv_parse_row(line, &row));
    TEST_ASSERT_EQUAL(4, row.n_fields);
    TEST_ASSERT_EQUAL_STRING("field1", row.fields[0]);
    TEST_ASSERT_EQUAL_STRING("", row.fields[1]);
    TEST_ASSERT_EQUAL_STRING("field3", row.fields[2]);
    TEST_ASSERT_EQUAL_STRING("", row.fields[3]);
}
*/
void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_field_basic);

	/*
	RUN_TEST(test_csv_parse_field);
	RUN_TEST(test_row_single);
	RUN_TEST(test_row_basic);
	RUN_TEST(test_row_newline_at_end);
	RUN_TEST(test_row_empty_fields);
	*/

	return UNITY_END();
}
