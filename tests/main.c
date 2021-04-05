#include <greatest.h>

GREATEST_MAIN_DEFS();

SUITE_EXTERN(RNG);
SUITE_EXTERN(KPA_tests);
SUITE_EXTERN(SORF_tests);

int main(int argc, char **argv) {
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(RNG);
	RUN_SUITE(KPA_tests);
	RUN_SUITE(SORF_tests);
	GREATEST_MAIN_END();
}
