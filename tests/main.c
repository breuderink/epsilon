#include <greatest.h>

GREATEST_MAIN_DEFS();

SUITE_EXTERN(RNG);

int main(int argc, char **argv) {
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(RNG);
	GREATEST_MAIN_END();
}
