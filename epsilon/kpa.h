#include <stddef.h>

typedef struct {
	float *alpha;
	void *instances;
	float (*kernel)(void *instances, size_t i, size_t j);
	size_t num_alpha;
} kernel_projection_t;

size_t km_num_idle(const kernel_projection_t *km);
size_t km_idle(const kernel_projection_t *km, size_t n);
float km_bpa_simple(kernel_projection_t *km, size_t t);

typedef struct {
	float C;   // aggressive updates with high C.
	float eps; // insensitive band for regression.
} passive_aggressive_t;

float km_dot(kernel_projection_t *km, size_t x);
float km_par(kernel_projection_t *km, const passive_aggressive_t pa, size_t x, float y);
float km_pac(kernel_projection_t *km, passive_aggressive_t pa, size_t xy_hat, size_t xy, float rho);
