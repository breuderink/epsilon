#include "epsilon.h"
#include <math.h>
#include <stddef.h>
#include <assert.h>

void observe(online_stats_t *s, float x) {
	assert(isfinite(x));
	if (s->n < SIZE_MAX)
		s->n++;

	float delta = (x - s->mean);
	s->mean += delta / s->n;
	assert(isfinite(s->mean));

	float delta2 = (x - s->mean);
	s->squared_diff += delta * delta2;
	assert(isfinite(s->squared_diff));
}

float mean(const online_stats_t *s) {
	switch (s->n) {
	case 0:
		return NAN;
	default:
		return s->mean;
	}
}

float var(const online_stats_t *s) {
	switch (s->n) {
	case 0:
		return NAN;
	case 1:
		return 0;
	default:
		return s->squared_diff / s->n;
	}
}
