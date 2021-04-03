#include "epsilon.h"
#include <math.h>
#include <stddef.h>

void observe(online_stats_t *s, float x) {
	s->n++;
	float delta = (x - s->mean);
	s->mean += delta / s->n;

	float delta2 = (x - s->mean);
	s->squared_diff += delta * delta2;
}

float mean(online_stats_t *const s) { return s->mean; }
float var(online_stats_t *const s) {
	switch (s->n) {
	case 0:
		return NAN;
	case 1:
		return 0;
	default:
		return s->squared_diff / (s->n - 1);
	}
}
