#include <assert.h>
#include <epsilon.h>
#include <kpa.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SUPPORT_VECTORS 512

// Data for kernel projection.
static struct { float position; } X[SUPPORT_VECTORS] = {0};
static float alpha[SUPPORT_VECTORS];

// TODO: refer to book on kernels. The kernel is specific to the problem.
static float linear_kernel(size_t i, size_t j) {
	float dot;
	dot += X[i].position * X[j].position;
	return dot;
}

// TODO: provide kernels that wrap linear kernel.
static float kernel(size_t i, size_t j) {
	float K_ij = 1 + linear_kernel(i, j);
	return K_ij; // * K_ij;
}

float target(float x) { return x + 10; }

int main() {
	float alpha[SUPPORT_VECTORS] = {0};
	KP_t regressor = {
	    .num_alpha = SUPPORT_VECTORS,
	    .alpha = alpha,
	    .kernel = &kernel,
	};
	PA_t hyper_params = {.C = INFINITY, .eps = 0};

	// Train model.
	for (int t = 0; t < 32; ++t) {
		// Find free slot in regressor, and fill to make kernel aware of new
		// data.
		float u = rand() / (float)RAND_MAX;
		float x = 10 * u - 5;

		for (int r = 0; r < 2; ++r) {
			size_t x_i = KP_find_idle(&regressor, 0);
			assert(regressor.alpha[x_i] == 0);
			X[x_i].position = x;

			// Update model.
			float y = target(x);
			float y_hat = BKPA_regress(&regressor, hyper_params, x_i, y);
			float error = y_hat - y;
			printf("x_i = %zu, (%.2f) \t = %.2f, \t error = %.2f.\n", x_i, x, y_hat,
				error);
		}
	}

	/*
	// Evaluate model.
	for (float x = -5; x < 5; x += 0.1) {
	    // Put x in kernel projection.
	    size_t xi = KP_find_idle(&regressor, 0);
	    X[xi].position = x;

	    float y_hat = BKPA_regress(&regressor, hyper_params, xi, NAN);
	    printf("f(%.2f) = %.2f, target(%.2f) = %.2f.\n", x, y_hat, x,
	           target(x));
	}
	*/
}
