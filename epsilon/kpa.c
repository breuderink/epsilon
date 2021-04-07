#include "kpa.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

size_t km_num_idle(const kernel_projection_t *km) {
	size_t c = 0;
	for (size_t i = 0; i < km->num_alpha; ++i) {
		c += km->alpha[i] == 0;
	}
	return c;
}

size_t km_idle(const kernel_projection_t *km, size_t n) {
	size_t c = n + 1;
	for (size_t i = 0; i < km->num_alpha; ++i) {
		c -= km->alpha[i] == 0;
		if (!c)
			return i;
	}
	assert(0);
	return 0;
}

float km_bpa_simple(kernel_projection_t *km, size_t t) {
	struct {
		size_t r;
		float proj;
		float loss;
	} curr, best = {.r = t, .proj = NAN, .loss = INFINITY};

	const float k_tt = km->kernel(km->instances, t, t);

	// Search for instance r to absorb.
	for (curr.r = 0; curr.r < km->num_alpha; ++curr.r) {
		if (curr.r == t) {
			continue;
		}
		float a_r = km->alpha[curr.r];
		if (a_r == 0) {
			// Freeing an empty space is trivial, but prevents serial
			// application of km_bpa_simple.
			continue;
		}

		// [w - w']^T K [w - w']
		// l(b) = [a b] [c d; d e] [a b] = aac + 2abd + bbe,
		// with a = -a_r, b = a_t - a_t + p, c = k(r,r), d = k(r,t),
		// and e = k(t,t).
		// Minimize l(b) = minimize 2abd + bbe.
		// dl(b)/db = 2ad + 2be = 0.
		// -> b = -ad/e ->  p = a_r k(r,t)/k(t,t)

		float k_rr = km->kernel(km->instances, curr.r, t);
		float k_rt = km->kernel(km->instances, curr.r, t);
		curr.proj = a_r * k_rt / k_tt;

		curr.loss = a_r * a_r * k_rr;              // l(b) = aac
		curr.loss += 2 * a_r * curr.proj * k_rt;   // + 2abd
		curr.loss += curr.proj * curr.proj * k_tt; // + bbe.

		if (curr.loss < best.loss) {
			best = curr;
		}
	}

	if (!isfinite(best.proj)) {
		// Could not find a value for r to absorb.
		return 0;
	}

	// Perform projection of r on target t, and neutralize r.
	km->alpha[t] += best.proj;
	km->alpha[best.r] = 0;

	return best.loss;
}

float km_dot(kernel_projection_t *km, size_t x) {
	float y_hat = 0;
	for (size_t i = 0; i < km->num_alpha; ++i) {
		float a = km->alpha[i];
		if (a != 0) {
			y_hat += a * km->kernel(km->instances, i, x);
		}
	}
	return y_hat;
}

float par(const passive_aggressive_t pa, float y_hat, float y) {
	assert(pa.eps >= 0);
	assert(pa.C > 0);
	assert(isfinite(y_hat));
	assert(isfinite(y));

	float loss = fmaxf(0, fabs(y - y_hat) - pa.eps);
	float tau = fminf(pa.C, loss); // PA-1.
	return copysign(tau, y - y_hat);
}

float km_par(kernel_projection_t *km, const passive_aggressive_t pa, size_t x, float y) {
	float y_hat = km_dot(km, x);
	assert(isfinite(y_hat));

	if (isfinite(y)) {
		// Compute loss if y is provided.
		assert(km->alpha[x] == 0);
		km->alpha[x] = par(pa, y_hat, y);

		// Maintain budget.
		if (km_num_idle(km) < 1) {
			km_bpa_simple(km, x);
		}
	}

	return y_hat;
}
