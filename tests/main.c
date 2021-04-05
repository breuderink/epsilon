#include <greatest.h>

GREATEST_MAIN_DEFS();

SUITE_EXTERN(RNG);
SUITE_EXTERN(KPA);

int main(int argc, char **argv) {
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(RNG);
	RUN_SUITE(KPA);
	GREATEST_MAIN_END();
}
