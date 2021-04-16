#include <assert.h>
#include <epsilon.h>
#include <kpa.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define BUDGET 64

// Data for kernel projection.
static struct { float position; } instances[BUDGET] = {0};
static float alpha[BUDGET];

/* Define a problem-specific kernel. A kernel defines a dot-product between
inputs, and should be adapted to the problem.

[1] Schölkopf, Bernhard, Alexander J. Smola, and Francis Bach. Learning with
	kernels: support vector machines, regularization, optimization, and beyond.
	MIT press, 2002. */

static float kernel(size_t i, size_t j) {
	float dot = 1;
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

	for (size_t i = 0; i < BUDGET; ++i) {
		// Initialize instance with input so that the kernel can access it.
		float input = 10 * (rand() / (float)RAND_MAX) - 5;
		instances[i].position = input;

		// Predict on new input.
		float prediction = KPA_regress(&regressor, PA, i, NAN);

		// Train model. This can be combined with previous step.
		float target = input * input - 10;
		KPA_regress(&regressor, PA, i, target);

		// Track loss.
		float l = fmaxf(0, fabsf(prediction - target) - PA.eps);
		observe(&loss, l);
	}

	// Display performance.
	printf("Loss %.2f (%.2f).\n", mean(&loss), sqrtf(variance(&loss)));
}
