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

// SORF is a pseudorandom matrix transformation based on fast Walsh-Hadamard
// transform and sign flipping. Based on [4, 5]. Note that SORF is patented:
// US2018/014145A1!
void SORF(float *x, uint8_t nbits);

// Repeat elements in x1 to form a vector x2 to increase input dimensions for
// SORF.
void SORF_repeat(float *x1, size_t n1, float *x2, size_t n2);


// Kernels.
typedef float (*kernel_t)(size_t i, size_t j);
float squared_Euclidean(kernel_t kernel, size_t a, size_t b);
float RBF_kernel(float sigma, float squared_dist);

// Kernel projection used to implement kernel passive-aggressive algorithms.
typedef struct {
	float *alpha;
	size_t num_alpha;
	kernel_t kernel;
} KP_t;

// Apply kernel projection to input x1.
float KP_apply(KP_t *km, size_t x1);

// Get n-th idle (i.e. alpha=0) support vector of kernel projection.
size_t KP_find_idle(const KP_t *km, size_t n);

// Passive-aggressive parameters.
typedef struct {
	float C;   // Perform aggressive updates with high C.
	float eps; // Size of insensitive band for regression.
} PA_t;

// Perform kernel PA regression. Target y can be NAN for inference.
float KPA_regress(KP_t *km, const PA_t pa, size_t xi, float y);

// Perform budgeted kernel PA regression. Target y can be NAN for inference.
float BKPA_regress(KP_t *km, const PA_t pa, size_t xi, float y);

/*
# References
[1] Marsaglia, George. "Xorshift RNGs." Journal of Statistical Software 8.14
    (2003): 1-6.

[2] Fowler, Glenn, et al. “The FNV Non-Cryptographic Hash Algorithm.” IETF
    Tools, Network Working Group, tools.ietf.org/html/draft-eastlake-fnv-03.

[3] Welford, B. P. "Note on a method for calculating corrected sums of
    squares and products." Technometrics 4.3 (1962): 419-420.

[4] Felix, X. Yu, et al. "Orthogonal random features." Advances in Neural
    Information Processing Systems. 2016.

[5] Choromanski, Krzysztof, and Vikas Sindhwani. "Recycling randomness
    with structure for sublinear time kernel expansions." International
    Conference on Machine Learning. 2016.
*/
