#include <stddef.h>
#include <stdint.h>

#define FNV1A_OFFSET 0x811c9dc5
#define FNV1A_PRIME 0x01000193

static inline uint32_t FNV1a_update(uint32_t hash, uint8_t data) {
	hash ^= data;
	hash *= FNV1A_PRIME;
	return hash;
}

uint32_t FNV1a_hash(void *data, size_t n) {
    uint8_t *p = (uint8_t *) data;
	uint32_t h = FNV1A_OFFSET;
	while (n--) {
		h = FNV1a_update(h, *p++);
	}
	return h;
}
