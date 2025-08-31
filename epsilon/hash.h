#include <stddef.h>
#include <stdint.h>

// Fowler-Noll-Vo 1a hash [1].
uint32_t FNV1a32_update(uint32_t hash, uint8_t data);
uint32_t FNV1a32_hash(const void *data, size_t n);

/*
# References

[1] Fowler, Glenn, et al. “The FNV Non-Cryptographic Hash Algorithm.” IETF
    Tools, Network Working Group, tools.ietf.org/html/draft-eastlake-fnv-03.
*/
