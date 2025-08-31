#include <stddef.h>
#include <stdint.h>

// Marsaglia's xorshift random number generator [1];
uint32_t xorshift32(uint32_t y);

// Fowler-Noll-Vo 1a hash [2].
uint32_t FNV1a32_update(uint32_t hash, uint8_t data);
uint32_t FNV1a32_hash(const void *data, size_t n);

// Structure for computing the variance in one pass using Welford's method [3].
typedef struct {
	float mean, squared_diff;
} Welfords_method_t;

typedef struct {
	size_t n;
	Welfords_method_t Welford;
} online_stats_t;

// Update online statistics with observation.
void observe(online_stats_t *s, float x);

// Current mean from online statistics.
float mean(const online_stats_t *s);

// Current (biased) population variance.
float pvariance(const online_stats_t *s);

// Current sample variance.
float variance(const online_stats_t *s);

// Perform in-place Fast Walsh-Hadamard transform.
void FWHT(float *x, uint8_t nbits);

// Passive-aggressive parameters, see [4].
typedef struct {
	float C;   // Perform aggressive updates with high C.
	float eps; // Size of insensitive band for regression.
} PA_t;

/*
# References

[1] Marsaglia, George. "Xorshift RNGs." Journal of Statistical Software 8.14
    (2003): 1-6.

[2] Fowler, Glenn, et al. “The FNV Non-Cryptographic Hash Algorithm.” IETF
    Tools, Network Working Group, tools.ietf.org/html/draft-eastlake-fnv-03.

[3] Welford, B. P. "Note on a method for calculating corrected sums of
    squares and products." Technometrics 4.3 (1962): 419-420.

[4] Crammer, K. et al. “Online Passive-Aggressive Algorithms.” J. Mach. Learn.
    Res. (2003).
*/
