#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <unity.h>

// Log-softmax computation
void log_softmax(const float *z, float *logp, size_t n, float temp) {
	// Find max for numerical stability
	float max_z = -INFINITY;
	for (size_t i = 0; i < n; i++)
		if (z[i] > max_z)
			max_z = z[i];

	// Subtract max, compute exp, sum
	float s = 0.0f;
	for (size_t i = 0; i < n; i++) {
		logp[i] = expf((z[i] - max_z) / temp);
		s += logp[i];
	}

	// Take log and normalize
	float log_sum = logf(s);
	for (size_t i = 0; i < n; i++)
		logp[i] = logf(logp[i]) - log_sum;
}

// Compute NLL loss given log-softmax output and target
float nnl(const float *logp, const float *y, size_t n) {
	float loss = 0.0f;
	for (size_t i = 0; i < n; i++) {
		assert(isfinite(logp[i]));
		loss -= y[i] * logp[i];
	}
	assert(isfinite(loss));
	return loss;
}

void ema_grad(const float *logp, const float *y, float *zg, size_t n,
              float ff) {
	assert(0 < ff && ff <= 1);
	for (size_t i = 0; i < n; i++) {
		float grad = expf(logp[i]) - y[i];
		zg[i] += ff * (grad - zg[i]);
	}
}

// Apply parameter update: params += step_size * grad
// Argument order: grad (input), params (output), length, step_size
void grad_step(const float *zg, float *w, size_t n, float step_size) {
	for (size_t i = 0; i < n; i++) {
		w[i] += step_size * zg[i];
	}
}

void test_log_softmax(void) {
	const size_t n = 3;
	const float input[n] = {1.0f, 2.0f, 3.0f};
	float output[n];

	log_softmax(input, output, n, 1.0f);

	float expected[n] = {
	    logf(0.09003057f),
	    logf(0.24472847f),
	    logf(0.66524096f),
	};
	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, output, n);
}

void test_nll(void) {
	const size_t n = 3;
	const float log_probs[n] = {
	    logf(0.1f),
	    logf(0.9f),
	    nextafterf(-INFINITY, 0.0f),
	};
	const float y[n] = {0.0f, 1.0f, 0.0f};

	float loss = nnl(log_probs, y, n);
	TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.10536052f, loss);
}

void test_grad(void) {
	const size_t n = 3;
	const float log_probs[n] = {
	    logf(0.1f),
	    logf(0.9f),
	    nextafterf(-INFINITY, 0.0f),
	};
	const float y[n] = {0.0f, 1.0f, 0.0f};

	// Set initial gradients to 5.
	float logits_grad[n] = {5.0f, 5.0f, 5.0f};

	// Apply accumulate_grad with decay = 0.1.
	const float decay = 0.1f;
	ema_grad(log_probs, y, logits_grad, n, decay);

	// Check expected gradients.
	float expected[n] = {(1 - decay) * 5.0f + decay * (0.1f - 0.0f),
	                     (1 - decay) * 5.0f + decay * (0.9f - 1.0f),
	                     (1 - decay) * 5.0f + decay * (0.0f - 0.0f)};

	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, logits_grad, n);
}

void test_grad_step(void) {
	const size_t n = 3;
	float z[n] = {0.0f};
	float z_grad[n] = {0.0f};

	float y[n] = {0.0f, 1.0f, 0.0f};
	float log_probs[n];

	for (size_t i = 0; i < 100; i++) {
		log_softmax(z, log_probs, n, 1.0f);
		ema_grad(log_probs, y, z_grad, n, 0.1f);
		grad_step(z_grad, z, n, -1.0f); // learning rate = 1.0f
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, nnl(log_probs, y, n), 0.0f);
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_log_softmax);
	RUN_TEST(test_nll);
	RUN_TEST(test_grad);
	RUN_TEST(test_grad_step);
	return UNITY_END();
}
