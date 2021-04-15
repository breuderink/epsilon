#include "kpa.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

float KP_apply(KP_t *kp, size_t xi) {
	float y_hat = 0;
	for (size_t i = 0; i < kp->num_alpha; ++i) {
		float a = kp->alpha[i];
		if (a != 0) {
			y_hat += a * kp->kernel(i, xi);
		}
	}
	return y_hat;
}

float PA1_regress_update(const PA_t pa, float y_hat, float y) {
	assert(pa.eps >= 0);
	assert(pa.C > 0);
	assert(isfinite(y_hat));
	assert(isfinite(y));

	float loss = fmaxf(0, fabs(y - y_hat) - pa.eps);
	float tau = fminf(pa.C, loss); // PA-1. FIXME: a factor ||x||^2 is missing!
	return copysign(tau, y - y_hat);
}

// Functions to maintain KP budget.
size_t KP_num_idle(const KP_t *kp) {
	size_t c = 0;
	for (size_t i = 0; i < kp->num_alpha; ++i) {
		c += kp->alpha[i] == 0;
	}
	return c;
}

size_t KP_find_idle(const KP_t *kp, size_t idle_index) {
	size_t c = idle_index + 1;
	for (size_t i = 0; i < kp->num_alpha; ++i) {
		c -= kp->alpha[i] == 0;
		if (c == 0)
			return i;
	}
	assert(0);
	return 0;
}

float BPA_simple(KP_t *kp, size_t t) {
	struct {
		size_t r;
		float proj;
		float loss;
	} curr, best = {.r = t, .proj = NAN, .loss = INFINITY};

	const float k_tt = kp->kernel(t, t);

	// Search for instance r to absorb.
	for (curr.r = 0; curr.r < kp->num_alpha; ++curr.r) {
		if (curr.r == t) {
			continue;
		}
		float a_r = kp->alpha[curr.r];
		if (a_r == 0) {
			// Freeing an empty space is trivial, but prevents serial
			// application of BPA_simple.
			continue;
		}

		// [w - w']^T K [w - w']
		// l(b) = [a b] [c d; d e] [a b] = aac + 2abd + bbe,
		// with a = -a_r, b = a_t - a_t + p, c = k(r,r), d = k(r,t),
		// and e = k(t,t).
		// Minimize l(b) = minimize 2abd + bbe.
		// dl(b)/db = 2ad + 2be = 0.
		// -> b = -ad/e ->  p = a_r k(r,t)/k(t,t)

		float k_rr = kp->kernel(curr.r, t);
		float k_rt = kp->kernel(curr.r, t);
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
	kp->alpha[t] += best.proj;
	kp->alpha[best.r] = 0;

	return best.loss;
}

float BKPA_regress(KP_t *kp, const PA_t pa, size_t x_i, float y) {
	float y_hat = KP_apply(kp, x_i);
	assert(isfinite(y_hat));

	if (isfinite(y)) {
		// Compute loss if y is provided.
		assert(kp->alpha[x_i] == 0);
		float loss = fmaxf(0, fabs(y_hat - y) - pa.eps);
		kp->alpha[x_i] =
		    copysign(fminf(pa.C, loss / kp->kernel(x_i, x_i)), y - y_hat);

		// Maintain budget.
		if (KP_num_idle(kp) < 1) {
			BPA_simple(kp, x_i);
		}
	}

	return y_hat;
}
