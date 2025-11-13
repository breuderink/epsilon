#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
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

int sample(const float *probs, size_t n, float u) {
	assert(0.0f <= u && u <= 1.0f);
	for (size_t i = 0; i < n; i++) {
		u -= probs[i];
		if (u < 0.0f) {
			return i;
		}
	}
	return n - 1;
}

void ema_grad(const float *probs, size_t y, float *logits_grad, size_t n,
              float decay) {
	assert(0 < decay && decay <= 1);

	for (size_t i = 0; i < n; i++) {
		float g = probs[i] - (float)(i == y);
		assert(isfinite(g));
		logits_grad[i] += decay * (g - logits_grad[i]);
	}
}

int act(const float *logits, float *logits_grad, float *probs, size_t n, int y,
        float decay) {
	// Compute probabilities.
	log_softmax(logits, probs, n, 1.0f);
	for (size_t j = 0; j < n; j++) {
		probs[j] = expf(probs[j]);
	}

	// Sample when y is negative.
	if (y < 0) {
		float u = (float)rand() / (float)RAND_MAX;
		y = sample(probs, n, u);
		assert(0 <= y && y < (int)n);
	}

	// Accumulate gradient.
	ema_grad(probs, y, logits_grad, n, decay);
	return y;
}

void grad_step(const float *logits_grad, float *logits, size_t n,
               float step_size) {
	for (size_t i = 0; i < n; i++) {
		logits[i] += step_size * logits_grad[i];
		assert(isfinite(logits[i]));
	}
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
	float probs[N] = {0, 0.25, 0.75};

	TEST_ASSERT_EQUAL_size_t(1, sample(probs, N, 0.0f));
	TEST_ASSERT_EQUAL_size_t(1, sample(probs, N, 0.24f));
	TEST_ASSERT_EQUAL_size_t(2, sample(probs, N, 0.26f));
	TEST_ASSERT_EQUAL_size_t(2, sample(probs, N, 1.0f));
}

void test_grad(void) {
	enum { N = 3 };
	const float probs[] = {0.1f, 0.9f, 0.0f};
	const size_t y = 1;

	// Set initial gradients.
	float logits_grad[] = {5, 5, 5};

	// Apply accumulate_grad with decay = 0.1.
	const float decay = 0.1f;
	ema_grad(probs, y, logits_grad, N, decay);

	// Check expected gradients.
	float expected[] = {(1 - decay) * 5 + decay * (0.1f - 0),
	                    (1 - decay) * 5 + decay * (0.9f - 1),
	                    (1 - decay) * 5 + decay * (0.0f - 0)};

	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, logits_grad, N);
}

void test_act(void) {
	enum { N = 3 };
	float z[N] = {0, 0, 0};
	float z_grad[N] = {0, 0, 0};
	float probs[N];
	float learning_rate = 1.0f;

	size_t y = 2;
	act(z, z_grad, probs, N, y, 0.1f);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, logf(1.0f / N), logf(probs[y]));

	for (size_t i = 0; i < 100; i++) {
		act(z, z_grad, probs, N, y, 0.1f);
		TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, probs[0] + probs[1] + probs[2]);

		grad_step(z_grad, z, N, -learning_rate);
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, 0, logf(probs[y]));
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

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_log_softmax);
	RUN_TEST(test_sample);
	RUN_TEST(test_grad);
	RUN_TEST(test_act);
	RUN_TEST(test_cross_entropy);
	return UNITY_END();
}
