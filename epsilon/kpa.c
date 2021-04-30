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

float PA_regress_base(const PA_t pa, float xx, float y_hat, float y) {
	float error = y_hat - y;
	float loss = fmaxf(0, fabs(error) - pa.eps);
	return copysignf(fminf(pa.C, loss / xx), -error);
}

float KPA_regress(KP_t *kp, const PA_t pa, size_t x_i, float y) {
	float y_hat = KP_apply(kp, x_i);
	assert(isfinite(y_hat));

	if (isfinite(y)) {
		assert(kp->alpha[x_i] == 0);
		kp->alpha[x_i] = PA_regress_base(pa, kp->kernel(x_i, x_i), y_hat, y);
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

/*
BPA simple update presented in [1].  It maintains the kernel budget by
absorbing a support vector into the target support vector with minimal
distortion.  The BPA simple update in equation (12) of [1] can be split in a
regular learning update, and a budget maintenance update.  Here we only
implement the part that maintains the budget, so that it can be combined with
different kernel methods.

[1] Wang, Zhuang, and Slobodan Vucetic.  "Online passive-aggressive
    algorithms on a budget." Proceedings of the Thirteenth International
    Conference on Artificial Intelligence and Statistics.  JMLR Workshop and
    Conference Proceedings, 2010.
*/

typedef struct {
	float rr, rt, tt;
} kernel_pair_t;

void BPA_S_update(const kernel_pair_t k, float a_r, float *proj, float *loss) {
	/*
	[w - w']^T K [w - w']
	l(b) = [a b] [c d; d e] [a b] = aac + 2abd + bbe,
	with a = -a_r, b = a_t - a_t + p, c = k(r,r), d = k(r,t),
	and e = k(t,t).
	Minimize l(b) = minimize 2abd + bbe.
	dl(b)/db = 2ad + 2be = 0.
	-> b = -ad/e ->  p = a_r k(r,t)/k(t,t)
	This corresponds to first part of equation (12) in [1].
	*/
	float p = *proj = a_r * k.rt / k.tt;

	*loss = a_r * a_r * k.rr;    // l(b) = aac
	*loss += 2 * a_r * p * k.rt; // + 2abd
	*loss += p * p * k.tt;       // + bbe.
}

float BPA_simple(KP_t *kp, size_t t) {
	struct {
		size_t r;
		float proj, loss;
	} curr = {0}, best = {.r = t, .proj = NAN, .loss = INFINITY};

	float k_tt = kp->kernel(t, t);
	// Search for instance r to absorb.
	for (curr.r = 0; curr.r < kp->num_alpha; ++curr.r) {
		if (curr.r == t || kp->alpha[curr.r] == 0) {
			continue;
		}
		kernel_pair_t k = {
		    .rr = kp->kernel(curr.r, curr.r),
		    .rt = kp->kernel(curr.r, t),
		    .tt = k_tt,
		};
		BPA_S_update(k, kp->alpha[curr.r], &curr.proj, &curr.loss);
		if (curr.loss < best.loss) {
			best = curr;
		}
	}

	// Perform projection of r on target t, and neutralize r.
	assert(best.r != t);
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
