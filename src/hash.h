
// FNV-1a 32-bit hash interface.
#include <stddef.h>
#include <stdint.h>

uint32_t FNV1a32_update(uint32_t hash, uint8_t data);
uint32_t FNV1a32_hash(const void *data, size_t n);
