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

#define BUDGET 32
static input_t support_vectors[BUDGET] = {0};
static float inner_product(size_t a, size_t b) {
	float k_ab = 1; // Use 1 to implicitly use a bias term.
	k_ab += support_vectors[a].position * support_vectors[b].position;
	return k_ab;
}

// Define a RBF kernel by wrapping the inner product defined above.
static float kernel(size_t a, size_t b) {
	float d2 = squared_Euclidean(inner_product, a, b);
	return RBF_kernel(1.0, d2);
}

int main() {
	PA_t PA = {.C = 10, .eps = 0.01};

	// Initialize a kernel regression model.
	float alpha[BUDGET] = {0};
	KP_t regressor = {
	    .alpha = alpha,
	    .num_alpha = BUDGET,
	    .kernel = &kernel,
	};

	// Initialize loss statistics to track performance.
	online_stats_t loss = {0};

	for (size_t t = 0; t < 100 * BUDGET; ++t) {
		// Get a new input.
		float input = 10 * (rand() / (float)RAND_MAX) - 5;

		// Store input in free support vector to make it available to the
		// kernel.
		size_t i = KP_find_idle(&regressor, 0);
		support_vectors[i].position = input;

		// Predict on new input.
		float prediction = KPA_regress(&regressor, PA, i, NAN);

		// Define target, see https://www.desmos.com/calculator/zxllrku62c.
		float target = sinf(input);

		// Update model.
		BKPA_regress(&regressor, PA, i, target);

		// Track loss.
		float error = prediction - target;
		observe(&loss, error * error);
	}

	// Display root mean squared error of predictions. It should be slightly
	// above zero, due to initial mistakes and the epsilon-insensitive hinge
	// loss used for training.
	printf("RMSE: %.3f.\n", sqrtf(mean(&loss)));
}
