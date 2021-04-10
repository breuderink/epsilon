#include <stddef.h>

typedef struct {
	float *alpha;
	void *instances;
	float (*kernel)(void *instances, size_t i, size_t j);
	size_t num_alpha;
} KP_t;

float KP_project(KP_t *km, size_t x);
size_t KP_num_idle(const KP_t *km);
size_t KP_find_idle(const KP_t *km, size_t n);

float BPA_simple(KP_t *km, size_t t);

typedef struct {
	float C;   // aggressive updates with high C.
	float eps; // insensitive band for regression.
} PA_t;

float BKPA_regress(KP_t *km, const PA_t pa, size_t x, float y);
