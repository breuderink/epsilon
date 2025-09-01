
#include <stddef.h>

// Online mean and variance (Welford's method) interface.
typedef struct {
	float mean, squared_diff;
} Welfords_method_t;

typedef struct {
	size_t n;
	Welfords_method_t Welford;
} online_stats_t;

void observe(online_stats_t *s, float x);

float mean(const online_stats_t *s);

float pvariance(const online_stats_t *s);

float variance(const online_stats_t *s);

/*
 * References
 * [1] Welford, B. P. "Note on a method for calculating corrected sums of
 *     squares and products." Technometrics 4.3 (1962): 419-420.
 */
