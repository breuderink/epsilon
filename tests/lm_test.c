#include <assert.h>
#include <math.h>
#include <stddef.h>
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

float cross_entropy(const float *log_probs, const float *y, size_t n) {
	float loss = 0.0f;
	for (size_t i = 0; i < n; i++) {
		assert(isfinite(log_probs[i]));
		loss -= y[i] * log_probs[i];
	}
	assert(isfinite(loss));
	return loss;
}

size_t sample(const float *log_probs, size_t n, float u) {
	assert(0.0f <= u && u <= 1.0f);
	for (size_t i = 0; i < n; i++) {
		u -= expf(log_probs[i]);
		if (u < 0.0f) {
			return i;
		}
	}
	return n - 1;
}

void ema_grad(const float *log_probs, size_t y, float *logits_grad, size_t n,
              float decay) {
	assert(0 < decay && decay <= 1);

	for (size_t i = 0; i < n; i++) {
		float g = expf(log_probs[i]) - (float)(i == y);
		assert(isfinite(g));
		logits_grad[i] += decay * (g - logits_grad[i]);
	}
}

void grad_step(const float *logits_grad, float *logits, size_t n,
               float step_size) {
	for (size_t i = 0; i < n; i++) {
		logits[i] += step_size * logits_grad[i];
		assert(isfinite(logits[i]));
	}
}

void test_log_softmax(void) {
	enum { N = 3 };
	const float input[N] = {1, 2, 3};
	float output[N] = {0, 0, 0};

	log_softmax(input, output, N, 1);

	float expected[N] = {
	    logf(0.09003057f),
	    logf(0.24472847f),
	    logf(0.66524096f),
	};
	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, output, N);
}

void test_sample(void) {
	enum { N = 3 };
	const float logits[N] = {-5, 0, 1};
	float log_probs[N] = {0, 0, 0};
	log_softmax(logits, log_probs, N, 1);

	TEST_ASSERT_EQUAL_size_t(0, sample(log_probs, N, 0.0f));
	TEST_ASSERT_EQUAL_size_t(1, sample(log_probs, N, 0.26f));
	TEST_ASSERT_EQUAL_size_t(2, sample(log_probs, N, 0.28f));
	TEST_ASSERT_EQUAL_size_t(2, sample(log_probs, N, 1.0f));
}

void test_cross_entropy(void) {
	enum { N = 3 };
	const float logits[N] = {-3, 1, 2};
	float log_probs[N];
	log_softmax(logits, log_probs, N, 1);

	const float y[N] = {0, 1, 0};
	float loss = cross_entropy(log_probs, y, N);

	// >>> from keras.ops import categorical_crossentropy
	// >>> categorical_crossentropy([0, 1, 0], [-3., 1., 2.], from_logits=True)
	// <tf.Tensor: shape=(), dtype=float32, numpy=1.3181754350662231>
	TEST_ASSERT_FLOAT_WITHIN(1e-4f, 1.3181, loss);
}

void test_grad(void) {
	const float log_probs[] = {
	    logf(0.1f),
	    logf(0.9f),
	    nextafterf(-INFINITY, 0),
	};
	const size_t y = 1;

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
	enum { N = 3 };
	float z[N] = {0, 0, 0};
	float z_grad[N] = {0, 0, 0};
	float log_probs[N];

	size_t y = 1;
	log_softmax(z, log_probs, N, 1.0f);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, logf(1.0f / N), log_probs[0]);

	for (size_t i = 0; i < 100; i++) {
		log_softmax(z, log_probs, N, 1.0f);
		ema_grad(log_probs, y, z_grad, N, 0.1f);
		grad_step(z_grad, z, N, -1.0f); // learning rate = 1.0
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, 0, log_probs[y]);
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_log_softmax);
	RUN_TEST(test_sample);
	RUN_TEST(test_cross_entropy);
	RUN_TEST(test_grad);
	RUN_TEST(test_grad_step);
	return UNITY_END();
}
