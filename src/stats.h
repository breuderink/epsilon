#ifndef EPSILON_STATS_H
#define EPSILON_STATS_H

#include <stddef.h>

// Online mean and variance (Welford's method) interface
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

#endif // EPSILON_STATS_H
