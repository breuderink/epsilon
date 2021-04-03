#include <stddef.h>
#include <stdint.h>

uint32_t xorshift32(uint32_t y);

typedef struct {
	size_t n;
	float mean, squared_diff;
} online_stats_t;

void observe(online_stats_t *s, float x);
float mean(online_stats_t *const s);
float var(online_stats_t *const s);
