#include "hash.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	// A hash function is a function that maps data of an arbitrary size to a
	// fixed value. It should be fast to compute, and it should minimize
	// collisions, where different inputs result in the same output hash value.

	// Compute the hash of similar strings. Their hashes should be
	// different.
	const char *data[] = {"Foo", "foo", "foo."};
	for (size_t s = 0; s < sizeof(data) / sizeof(data[0]); ++s) {
		// Set string.
		const char *str = data[s];

		// Compute hash.
		uint32_t hash = FNV1a32_hash(str, strlen(str));

		// Display results.
		printf("Hashed '%s' to %#010x.\n", str, hash);
	}
	return 0;
}
