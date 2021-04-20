#include <stdint.h>

#define FNV1A_OFFSET 0x811c9dc5
#define FNV1A_PRIME 0x01000193

uint32_t fnv1a_hash(uint32_t hash, uint8_t data) {
	return (hash ^ data) * FNV1A_PRIME;
}
