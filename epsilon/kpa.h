#include <stddef.h>

typedef float (*kernel_t)(size_t i, size_t j);

typedef struct {
	float *alpha;
	size_t num_alpha;
	kernel_t kernel;
} KP_t;

// Apply kernel projection to input x1.
float KP_apply(KP_t *km, size_t x1);

// Find number of non-support vectors.
size_t KP_num_idle(const KP_t *km);

// Get index of n-th non-support vector.
size_t KP_find_idle(const KP_t *km, size_t n);

// Reduce budget kernel of projection by absorbing a support vector into last.
float BPA_simple(KP_t *km, size_t last);

// Hyper-parameters for the passive-aggressive family of algorithms.
typedef struct {
	float C;   // aggressive updates with high C.
	float eps; // insensitive band for regression.
} PA_t;

// Perform budgeted PA regression. Target y can be NAN.
float BKPA_regress(KP_t *km, const PA_t pa, size_t xi, float y);
