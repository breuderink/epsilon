#include <assert.h>
#include <epsilon.h>
#include <kpa.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Define a problem-specific kernel. A kernel defines a dot-product between
inputs, and should be adapted to the problem.

[1] Schölkopf, Bernhard, Alexander J. Smola, and Francis Bach. Learning with
	kernels: support vector machines, regularization, optimization, and beyond.
	MIT press, 2002. */

#define BUDGET 64
static struct { float position; } instances[BUDGET] = {0};
static float kernel(size_t i, size_t j) {
	float dot = 1; // Use 1 to implicitly use a bias term.
	dot += instances[i].position * instances[j].position;
	return dot * dot;
}

int main() {
	PA_t PA = {.C = INFINITY, .eps = 0.1};

	// Define regressor.
	float alpha[BUDGET] = {0};
	KP_t regressor = {
	    .alpha = alpha,
	    .num_alpha = BUDGET,
	    .kernel = &kernel,
	};

	online_stats_t loss = {0};

	for (size_t t = 0; t < 10*BUDGET; ++t) {
		// Get a new input.
		float input = 10 * (rand() / (float)RAND_MAX) - 5;

		// Store input in free instance to make it available to the kernel.
		size_t i = KP_find_idle(&regressor, 0);
		instances[i].position = input;

		// Predict on new input.
		float prediction = KPA_regress(&regressor, PA, i, NAN);

		// Define target and update model.
		float target = input * input - 10;
		BKPA_regress(&regressor, PA, i, target);

		// Track loss.
		float l = fmaxf(0, fabsf(prediction - target) - PA.eps);
		observe(&loss, l);
	}

	// Display performance.
	printf("Loss %.2f (%.2f).\n", mean(&loss), sqrtf(variance(&loss)));
}
