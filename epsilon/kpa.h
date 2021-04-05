#include <stddef.h>

typedef struct {
	float *alpha;
	void *instances;
	float (*kernel)(void *instances, size_t i, size_t j);
	size_t num_alpha;
} km_t;

size_t km_num_idle(const km_t *km);
size_t km_idle(const km_t *km, size_t n);
float km_bpa_simple(km_t *km, size_t t);

typedef struct {
	float C;   // aggressive updates with high C.
	float eps; // insensitive band for regression.
} pa_t;

float km_dot(km_t *km, size_t x);
float km_par(km_t *km, const pa_t pa, size_t x, float y);
float km_pac(km_t *km, pa_t pa, size_t xy_hat, size_t xy, float rho);
