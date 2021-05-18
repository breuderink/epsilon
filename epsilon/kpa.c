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
	float ii, ij, jj;
} kernel_pair_t;

void BPA_S_absorb(const kernel_pair_t k, float a_r, float *proj, float *L) {
	/*
	We want to to absorb a support vector r (remove) into a support vector t
	(target). That can be encode with a new weight vector a', where the
	coefficient of r is set to zero, and the coefficient of t is changed by
	adding p (projection). The goal is to find a a' that is minimizes the
	distortion:

	    L = [a' - a]^T K [a' - a].

	We can simplify L by expressing it with the submatrix of K with the elements
	that differ between a' and a:

	    L = [-a_r, p]^T [k_rr, k_rt; k_rt, k_tt] [-a_r, p].

	Note that we want to find the p that minimizes L. Expand L to:

	    L = [-a_r, p]^T [k_rr, k_rt; k_rt, k_tt] [-a_r, p]
	      = [-a_r k_rr + p k_rt, -a_r k_rt + p k_tt]  [-a_r, p]
	      = -a_r (-a_r k_rr + p k_rt) + p (-a_r k_rt + p k_tt)
	      = a_r^2 k_rr - a_r p k_rt - a_r p k_rt + p^2 k_tt
	      = a_r^2 k_rr - 2 a_r p k_rt + p^2 k_tt

	To minimize L, can set it's derivative with respect to p to zero:

	    ∂/∂p L = ∂/∂p (-2 a_r p k_rt + p^2 k_tt) = -2 a_r k_rt + 2 p k_tt = 0.
	    2 p ktt = 2 a_r k_rt => p = a_r k_rt / k_tt.

	This corresponds to the first term of equation (12) for BPA-S in [1].
	*/

	// Compute p = a_r k_rt / k_tt.
	float p = *proj = a_r * k.ij / k.jj;

	// Compute L = a_r^2 k_rr - 2 a_r p k_rt + p^2 k_tt.
	*L = (a_r * a_r * k.ii) - (2 * a_r * p * k.ij) + (p * p * k.jj);

	//assert(*L >= 0);
}

float BPA_simple(KP_t *kp, size_t target) {
	struct {
		size_t r;
		float proj, loss;
	} curr = {0}, best = {.r = target, .proj = NAN, .loss = INFINITY};

	float k_tt = kp->kernel(target, target);
	// Search for support vector r to absorb.
	for (curr.r = 0; curr.r < kp->num_alpha; ++curr.r) {
		if (curr.r == target || kp->alpha[curr.r] == 0) {
			continue;
		}
		kernel_pair_t k = {
		    .ii = kp->kernel(curr.r, curr.r),
		    .ij = kp->kernel(curr.r, target),
		    .jj = k_tt,
		};
		BPA_S_absorb(k, kp->alpha[curr.r], &curr.proj, &curr.loss);
		assert(isfinite(curr.loss));
		if (curr.loss < best.loss) {
			best = curr;
		}
	}

	// Perform projection of r on target t, and neutralize r.
	assert(best.r != target);
	kp->alpha[target] += best.proj;
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
