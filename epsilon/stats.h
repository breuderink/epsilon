#include <stddef.h>

// Structure for computing the variance in one pass using Welford's method [1].
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

/*
# References
[1] Welford, B. P. "Note on a method for calculating corrected sums of
    squares and products." Technometrics 4.3 (1962): 419-420.
*/
