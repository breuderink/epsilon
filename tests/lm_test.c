#include <assert.h>
#include <float.h>
#include <stddef.h>
#include <string.h>
#include <unity.h>

// In-place log-softmax computation.
void log_softmax(const float *z, float *s, size_t n) {
	// Find max for numerical stability
	float max_z = -INFINITY;
	for (size_t i = 0; i < n; i++)
		if (z[i] > max_z)
			max_z = z[i];

	// Subtract max, compute exp, sum
	float sum = 0.0f;
	for (size_t i = 0; i < n; i++) {
		s[i] = expf(z[i] - max_z);
		sum += s[i];
	}

	// Take log and normalize
	float log_sum = logf(sum);
	for (size_t i = 0; i < n; i++)
		s[i] = logf(s[i]) - log_sum;
}

// Compute NLL loss given log-softmax output and target y
float nll_from_logp(const float *logp, const float *y, size_t n) {
	float loss = 0.0f;
	for (size_t i = 0; i < n; i++) {
		assert(isfinite(logp[i]));
		loss -= y[i] * logp[i];
	}
	assert(isfinite(loss));
	return loss;
}

void accumulate_grad(const float *logp, const float *y, float *z_grad, size_t n,
                     float ff) {
	assert(0 < ff && ff <= 1);
	for (size_t i = 0; i < n; i++) {
		float grad = expf(logp[i]) - y[i];
		z_grad[i] += ff * (grad - z_grad[i]);
	}
}

void apply_update(float *params, const float *update, size_t n, float scale) {
	for (size_t i = 0; i < n; i++) {
		params[i] += scale * update[i];
	}
}

void test_log_softmax(void) {
	const size_t n = 3;
	const float input[n] = {1.0f, 2.0f, 3.0f};
	float output[n];

	log_softmax(input, output, n);

	float expected[n] = {
	    logf(0.09003057f),
	    logf(0.24472847f),
	    logf(0.66524096f),
	};
	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, output, n);
}

void test_nll_from_logp(void) {
	const size_t n = 3;
	const float logp[n] = {logf(0.1f), logf(0.9f), nextafterf(-INFINITY, 0.0f)};
	const float y[n] = {0.0f, 1.0f, 0.0f};

	float loss = nll_from_logp(logp, y, n);
	TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.10536052f, loss);
}

void test_accumulate_grad(void) {
	const size_t n = 3;
	const float logp[n] = {logf(0.1f), logf(0.9f), nextafterf(-INFINITY, 0.0f)};
	const float y[n] = {0.0f, 1.0f, 0.0f};

	// Set initial gradients to 5.
	float z_grad[n] = {5.0f, 5.0f, 5.0f};

	// Apply accumulate_grad with ff = 0.1.
	const float ff = 0.1f;
	accumulate_grad(logp, y, z_grad, n, ff);

	// Check expected gradients.
	float expected[n] = {(1 - ff) * 5.0f + ff * (0.1f - 0.0f),
	                     (1 - ff) * 5.0f + ff * (0.9f - 1.0f),
	                     (1 - ff) * 5.0f + ff * (0.0f - 0.0f)};

	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, z_grad, n);
}

void test_apply_update(void) {
	const size_t n = 3;
	float z[n] = {0.0f};
	float z_grad[n] = {0.0f};

	float y[n] = {0.0f, 1.0f, 0.0f};
	float logp[n];

	for (size_t i = 0; i < 100; i++) {
		log_softmax(z, logp, n);
		accumulate_grad(logp, y, z_grad, n, 0.1f);
		apply_update(z, z_grad, n, -1.0f); // learning rate = 1.0f
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, nll_from_logp(logp, y, n), 0.0f);
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_log_softmax);
	RUN_TEST(test_nll_from_logp);
	RUN_TEST(test_accumulate_grad);
	RUN_TEST(test_apply_update);
	return UNITY_END();
}
