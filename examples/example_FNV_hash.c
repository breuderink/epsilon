#include "epsilon.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main() {
    // A hash function is a function that maps data of an arbitrary size to a
    // fixed value. It should be fast to compute, and it should mimise
    // collisions, where different inputs result in the same output hash value.

    // Compute the hash of two similar strings. Their hashes should be
    // different.
    const char *data[] = {"Epsilon", "epsilon", "epsilon."};
    for (int s = 0; s < sizeof(data) / sizeof(data[0]); ++s) {
        // Set string.
        const char *str = data[s];

        // Compute hash.
        uint32_t hash = FNV1a_hash(str, strlen(str));

        // Display results. Compare with results on
        // https://sha256calc.com/hash/fnv1a32.
        printf("Hashed '%s' to %#010x.\n", str, hash);
    }
    return 0;
}
