#include <greatest.h>

GREATEST_MAIN_DEFS();

SUITE_EXTERN(RNG);
SUITE_EXTERN(SORF_tests);
SUITE_EXTERN(Welford);

int main(int argc, char **argv) {
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(RNG);
	RUN_SUITE(SORF_tests);
	RUN_SUITE(Welford);
	GREATEST_MAIN_END();
}
