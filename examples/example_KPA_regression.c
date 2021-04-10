#include <assert.h>
#include <epsilon.h>
#include <kpa.h>
#include <math.h>
#include <stdio.h>

#define SUPPORT_VECTORS 5

// Data for kernel projection.
static struct { float position; } X[SUPPORT_VECTORS] = {0};
static float alpha[SUPPORT_VECTORS];

// TODO: refer to book on kernels. The kernel is specific to the problem.
static float linear_kernel(size_t i, size_t j) {
	float k = 1; // Include a bias term.
	k += X[i].position * X[j].position;
	return k * k;
}

static float kernel(void *instances, size_t i, size_t j) {
	assert(instances == NULL); // FIXME: update interface.
	// TODO: provide kernels that wrap linear kernel.
	float K_ij = linear_kernel(i, j);
	return K_ij * K_ij;
}

float target(float x) { return x * x; }

int main() {
	float alpha[SUPPORT_VECTORS] = {0};
	KP_t regressor = {
	    .num_alpha = SUPPORT_VECTORS, .alpha = alpha, .kernel = kernel};

	// Train model.
	for (int pass = 0; pass < 1; ++pass) {
		for (float x = -5; x < 5; x += 0.1) {
			// Find free slot in regressor, and fill to make kernel aware of new
			// data.
			size_t xi = KP_find_idle(&regressor, 0);
            assert(regressor.alpha[xi] == 0);
			X[xi].position = x;

			// Update model.
			BKPA_regress(&regressor, (PA_t){.C = 1e4, .eps = 0.01}, xi,
			            target(x));
		}
	}

	// Evaluate model.
	for (float x = -5; x < 5; x += 0.1) {
		// Put x in kernel projection.
		size_t xi = KP_find_idle(&regressor, 0);
		X[xi].position = x;
		float y_hat =
		    BKPA_regress(&regressor, (PA_t){.C = 1, .eps = 0.1}, xi, NAN);

		float y = target(x);
		printf("f(%.2f) = %.2f, target(%.2f) = %.2f.\n", x, y_hat, x, y);
	}
}
