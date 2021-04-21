#include <stddef.h>
#include <stdint.h>

uint32_t xorshift32(uint32_t y);

// Structure for computing the variance in one pass using Welfords' method [3].
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

// SORF is a pseudorandom matrix transformation based on fast Walsh-Hadamard
// transform and sign flipping. Based on [1, 2]. Note that SORF is patented:
// US2018/014145A1!
void SORF(float *x, uint8_t nbits);

// Repeat elements in x1 to form a vector x2 to increase input dimensions for
// SORF.
void SORF_repeat(float *x1, size_t n1, float *x2, size_t n2);


// Fowler-Noll-Vo 1a hash [4].
uint32_t FNV1a_update(uint32_t hash, uint8_t data);
uint32_t FNV1a_hash(const void *data, size_t n);

/*
# References
[1] Felix, X. Yu, et al. "Orthogonal random features." Advances in Neural
    Information Processing Systems. 2016.

[2] Choromanski, Krzysztof, and Vikas Sindhwani. "Recycling randomness
    with structure for sublinear time kernel expansions." International
    Conference on Machine Learning. 2016.

[3] Welford, B. P. "Note on a method for calculating corrected sums of
    squares and products." Technometrics 4.3 (1962): 419-420.

[4] Eastlake, Donald, et al. “The FNV Non-Cryptographic Hash Algorithm.”
    Tools.ietf.org, tools.ietf.org/html/draft-eastlake-fnv-17. Accessed 21 Apr.
    2021. ‌
*/
