#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <unity.h>

void log_softmax(const float *logits, float *log_probs, size_t n, float temp) {
	// Find max for numerical stability.
	float max_logit = -INFINITY;
	for (size_t i = 0; i < n; i++)
		if (logits[i] > max_logit)
			max_logit = logits[i];

	// Compute sum for normalization.
	float sum = 0.0f;
	for (size_t i = 0; i < n; i++) {
		log_probs[i] = expf((logits[i] - max_logit) / temp);
		sum += log_probs[i];
	}

	// Normalize.
	float log_sum = logf(sum);
	for (size_t i = 0; i < n; i++)
		log_probs[i] = logf(log_probs[i]) - log_sum;
}

float nll(const float *log_probs, const float *y, size_t n) {
	float loss = 0.0f;
	for (size_t i = 0; i < n; i++) {
		assert(isfinite(log_probs[i]));
		loss -= y[i] * log_probs[i];
	}
	assert(isfinite(loss));
	return loss;
}

size_t sample(const float *log_probs, size_t n, float u) {
	assert(0.0f <= u && u < 1.0f);
	for (size_t i = 0; i < n; i++) {
		u -= expf(log_probs[i]);
		if (u < 0.0f) {
			return i;
		}
	}
	return n - 1;
}

void ema_grad(const float *log_probs, const float *y, float *logits_grad,
              size_t n, float decay) {
	assert(0 < decay && decay <= 1);

	for (size_t i = 0; i < n; i++) {
		float g = expf(log_probs[i]) - y[i];
		assert(isfinite(g));
		logits_grad[i] += decay * (g - logits_grad[i]);
	}
}

void grad_step(const float *logits_grad, float *logits, size_t n,
               float step_size) {
	for (size_t i = 0; i < n; i++) {
		logits[i] += step_size * logits_grad[i];
	}
}

void test_log_softmax(void) {
	const float input[] = {1, 2, 3};
	const size_t n = sizeof(input) / sizeof(input[0]);
	float output[n];

	log_softmax(input, output, n, 1);

	float expected[] = {
	    logf(0.09003057f),
	    logf(0.24472847f),
	    logf(0.66524096f),
	};
	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, output, n);
}

void test_sample(void) {
	const float log_probs[] = {
	    logf(0.1f),
	    logf(0.9f),
	    nextafterf(-INFINITY, 0),
	};
	const size_t n = sizeof(log_probs) / sizeof(log_probs[0]);

	TEST_ASSERT_EQUAL_size_t(0, sample(log_probs, n, 0.05f));
	TEST_ASSERT_EQUAL_size_t(1, sample(log_probs, n, 0.5f));
	TEST_ASSERT_EQUAL_size_t(1, sample(log_probs, n, 0.95f));
	TEST_ASSERT_EQUAL_size_t(1, sample(log_probs, n, 0.95f));
}

void test_nll(void) {
	const float log_probs[] = {
	    logf(0.1f),
	    logf(0.9f),
	    nextafterf(-INFINITY, 0.0f),
	};
	const float y[] = {0.0f, 1.0f, 0.0f};
	size_t n = sizeof(log_probs) / sizeof(log_probs[0]);

	float loss = nll(log_probs, y, n);
	TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.10536052f, loss);
}

void test_grad(void) {
	const float log_probs[] = {
	    logf(0.1f),
	    logf(0.9f),
	    nextafterf(-INFINITY, 0),
	};
	const float y[] = {0, 1, 0};

	// Set initial gradients.
	float logits_grad[] = {5, 5, 5};

	// Apply accumulate_grad with decay = 0.1.
	size_t n = sizeof(log_probs) / sizeof(log_probs[0]);
	const float decay = 0.1f;
	ema_grad(log_probs, y, logits_grad, n, decay);

	// Check expected gradients.
	float expected[] = {(1 - decay) * 5 + decay * (0.1f - 0),
	                    (1 - decay) * 5 + decay * (0.9f - 1),
	                    (1 - decay) * 5 + decay * (0.0f - 0)};

	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, logits_grad, n);
}

void test_grad_step(void) {
	float y[] = {0, 1, 0};
	float z[] = {0, 0, 0};
	float z_grad[] = {0, 0, 0};
	const size_t n = sizeof(y) / sizeof(y[0]);
	float log_probs[n];

	for (size_t i = 0; i < 100; i++) {
		log_softmax(z, log_probs, n, 1.0f);
		ema_grad(log_probs, y, z_grad, n, 0.1f);
		grad_step(z_grad, z, n, -1.0f); // learning rate = 1.0
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, nll(log_probs, y, n), 0);
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_log_softmax);
	RUN_TEST(test_sample);
	RUN_TEST(test_nll);
	RUN_TEST(test_grad);
	RUN_TEST(test_grad_step);
	return UNITY_END();
}
