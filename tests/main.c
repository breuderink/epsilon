#include <greatest.h>

GREATEST_MAIN_DEFS();

SUITE_EXTERN(xorshift);

int main(int argc, char **argv) {
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(xorshift);
	GREATEST_MAIN_END();
}
