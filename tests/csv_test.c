#include "csv.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

char *csv_trim(char *s);
void test_csv_trim(void) {
	const char *cases[][2] = {
	    {"", ""},
	    {"a", "a"},
	    {" a", "a"},
	    {"a ", "a"},
	    {" a ", "a"},
	    {" a b ", "a b"},
	    {"  a b c  ", "a b c"},
	};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		char buf[80];
		strncpy(buf, cases[i][0], sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		char *out = csv_trim(buf);
		TEST_ASSERT_EQUAL_STRING(cases[i][1], out);
	}
}

char *csv_unquote(char *s);
void test_csv_unquote(void) {
	const char *cases[][2] = {
	    {"", ""},       {"\"a\"", "a"},       {"a\"", "a\""},
	    {"\"a", "\"a"}, {" \"a\"", " \"a\""}, {"\"a\" ", "\"a\" "}};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		char buf[80];
		strncpy(buf, cases[i][0], sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		char *out = csv_unquote(buf);
		TEST_ASSERT_EQUAL_STRING(cases[i][1], out);
	}
}

char *csv_unescape(char *s);
void test_csv_unescape(void) {
	const char *cases[][2] = {
	    {"", ""},           {"a", "a"},
	    {"\"\"", "\""},     {"\"\"\"\"", "\"\""},
	    {"a\"\"b", "a\"b"}, {"\"\"", "\""},
	};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		char buf[80];
		strncpy(buf, cases[i][0], sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		char *out = csv_unescape(buf);
		TEST_ASSERT_EQUAL_STRING(cases[i][1], out);
	}
}

typedef struct {
	const char *input, *fields[CSV_FIELDS];
	size_t num_fields, remainder;
} csv_test_case_t;

void test_csv_tokenize(void) {
	csv_test_case_t cases[] = {
	    {"x", {"x"}, 1, 0},
	    {"x,y", {"x", "y"}, 2, 0},
	    {"x,y\n", {"x", "y"}, 2, 0},
	    {"x,y,z\n", {"x", "y", "z"}, 3, 0},
	    {"x,y,z\r\n", {"x", "y", "z"}, 3, 0},
	    {"x,y,z\r\na", {"x", "y", "z"}, 3, 1},
	    {"x,y,z\r\na,b", {"x", "y", "z"}, 3, 3},

	    {"", {""}, 1, 0},
	    {",", {"", ""}, 2, 0},
	    {",y", {"", "y"}, 2, 0},
	    {",,z", {"", "", "z"}, 3, 0},
	    {",,,", {"", "", ""}, 3, 0},
	};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		csv_test_case_t c = cases[i];

		csv_record_t r = {0};
		size_t consumed = csv_tokenize(c.input, strlen(c.input), &r);
		TEST_ASSERT_EQUAL_STRING_ARRAY(c.fields, r.fields, c.num_fields);
		TEST_ASSERT_EQUAL(c.remainder, strlen(c.input) - consumed);
	}
}
void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_csv_trim);
	RUN_TEST(test_csv_unquote);
	RUN_TEST(test_csv_unescape);
	RUN_TEST(test_csv_tokenize);
	return UNITY_END();
}
