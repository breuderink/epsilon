#include "epsilon.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

void Welford_observe(Welfords_method_t *w, size_t n, float x) {
	float delta_before = (x - w->mean);
	w->mean += delta_before / n;
	assert(isfinite(w->mean));

	float delta_after = (x - w->mean);
	w->squared_diff += delta_before * delta_after;
	assert(isfinite(w->squared_diff));

	// Note that we can track the population variance instead of the squared
	// difference. That seems more symmetrical, and would prevent squared_diff
	// from getting to big for a fixed-point implementation.
}

void observe(online_stats_t *s, float x) {
	assert(isfinite(x));
	if (s->n < SIZE_MAX) {
		s->n++;
	}

	Welford_observe(&s->Welford, s->n, x);
}

float mean(const online_stats_t *s) {
	if (s->n == 0)
		return NAN;
	return s->Welford.mean;
}

float pvariance(const online_stats_t *s) {
	return s->Welford.squared_diff / s->n;
}

float variance(const online_stats_t *s) {
	float n = s->n;
	return n * pvariance(s) / (n - 1);
}
