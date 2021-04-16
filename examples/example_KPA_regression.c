#include "epsilon.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Define problem-specific input structure. Our input is a single position.
typedef struct {
	float position;
} input_t;

/* Define a problem-specific kernel. A kernel defines a dot-product between
inputs, and should be adapted to the problem.

[1] Schölkopf, Bernhard, Alexander J. Smola, and Francis Bach. Learning with
kernels: support vector machines, regularization, optimization, and beyond.
MIT press, 2002. */

#define BUDGET 128
static input_t instances[BUDGET] = {0};
static float linear_kernel(size_t i, size_t j) {
	float dot = 1; // Use 1 to implicitly use a bias term.
	dot += instances[i].position * instances[j].position;
	return dot;
}

static float Gaussian_kernel(size_t a, size_t b) {
	float k_aa = linear_kernel(a, a);
	float k_ab = linear_kernel(a, b);
	float k_bb = linear_kernel(b, b);

	// (a - b)^T (a - b) = a^T a - 2 a^T b + b^T b.
	float squared_dist = k_aa - 2 * k_ab + k_bb;
	return expf(-squared_dist * 0.5);
}

int main() {
	PA_t PA = {.C = 100, .eps = 0.1};

	// Define regressor.
	float alpha[BUDGET] = {0};
	KP_t regressor = {
	    .alpha = alpha,
	    .num_alpha = BUDGET,
	    .kernel = &Gaussian_kernel,
	};

	online_stats_t loss = {0};

	for (size_t t = 0; t < 20*BUDGET; ++t) {
		// Get a new input.
		float input = 10 * (rand() / (float)RAND_MAX) - 5;

		// Store input in free instance to make it available to the kernel.
		size_t i = KP_find_idle(&regressor, 0);
		instances[i].position = input;

		// Predict on new input.
		float prediction = KPA_regress(&regressor, PA, i, NAN);

		// Define target and update model.
		float target = sinf(input);
		BKPA_regress(&regressor, PA, i, target);

		// Track loss.
		float error = prediction - target;
		observe(&loss, error * error);
	}

	// Display performance.
	printf("RMSE: %.3f.\n", sqrtf(mean(&loss)));
}
