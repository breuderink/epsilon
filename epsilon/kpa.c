#include "epsilon.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

float squared_Euclidean(kernel_t kernel, size_t a, size_t b) {
	// (a - b)^T (a - b) = a^T a - 2 a^T b + b^T b.
	return kernel(a, a) - 2 * kernel(a, b) + kernel(b, b);
}

float RBF_kernel(float sigma, float squared_dist) {
	return expf(-squared_dist / (2 * sigma * sigma));
}

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

float KPA_regress(KP_t *kp, const PA_t pa, size_t x_i, float y) {
	float y_hat = KP_apply(kp, x_i);
	assert(isfinite(y_hat));

	if (isfinite(y)) {
		assert(kp->alpha[x_i] == 0);
		float error = y_hat - y;
		float loss = fmaxf(0, fabs(error) - pa.eps);
		float xx = kp->kernel(x_i, x_i);
		kp->alpha[x_i] = copysignf(fminf(pa.C, loss / xx), -error);
	}

	return y_hat;
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
	size_t idle_count = 0;
	for (size_t i = 0; i < kp->num_alpha; ++i) {
		if (kp->alpha[i] == 0) {
			if (idle_count++ == idle_index) {
				return i;
			}
		}
	}
	abort();
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
	float y_hat = KPA_regress(kp, pa, x_i, y);
	if (isfinite(y)) {
		// Maintain budget.
		if (KP_num_idle(kp) < 1) {
			BPA_simple(kp, x_i);
		}
	}
	return y_hat;
}
