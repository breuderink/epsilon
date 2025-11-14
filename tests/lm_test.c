#include "hash.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
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

size_t sample(const float *probs, size_t n, float u) {
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

typedef struct {
	size_t context_size, vocab_size;
	float *param; // [c x 2 x v]
	float *probs; // [v]
	float decay;
} policy_t;

policy_t create_policy(size_t c, size_t v) {
	return (policy_t){
	    .context_size = c,
	    .vocab_size = v,
	    .probs = calloc(v, sizeof(float)),
	    .param = calloc(c * 2 * v, sizeof(float)),
	    .decay = 0.1,
	};
}

void policy_destroy(policy_t *p) {
	free(p->probs);
	free(p->param);
	*p = (policy_t){0};
}

static inline float *policy_logits(policy_t *p, size_t c) {
	return &p->param[c * 2 * p->vocab_size];
}

static inline float *policy_logits_grad(policy_t *p, size_t c) {
	return &p->param[(c * 2 + 1) * p->vocab_size];
}

// Perform a policy step. When the action y is -1, it is sampled. The
// gradients are accumulated with an EMA and can be used for a gradient step
// later.
int policy_step(policy_t *p, size_t context, int y) {
	size_t v = p->vocab_size;
	float *logits = policy_logits(p, context);
	float *logits_grad = policy_logits_grad(p, context);

	// Compute probabilities.
	log_softmax(logits, p->probs, v, 1.0f);
	for (size_t j = 0; j < v; j++) {
		p->probs[j] = expf(p->probs[j]);
	}

	// Sample when y is negative.
	if (y < 0) {
		float u = rand() / (float)RAND_MAX;
		y = sample(p->probs, v, u);
		assert(0 <= y && y < (int)v);
	}

	// Accumulate gradient.
	ema_grad(p->probs, y, logits_grad, p->vocab_size, p->decay);
	return y;
}

// Perform a gradient step on the logits using the accumulated gradients.
void grad_step(policy_t *p, float step_size) {
	for (size_t c = 0; c < p->context_size; ++c) {
		float *logits = policy_logits(p, c);
		float *logits_grad = policy_logits_grad(p, c);

		for (size_t i = 0; i < p->vocab_size; i++) {
			logits[i] += step_size * logits_grad[i];
			assert(isfinite(logits[i]));
		}
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
	float probs[N] = {0, 0.25, 0.75};

	TEST_ASSERT_EQUAL_size_t(1, sample(probs, N, 0.0f));
	TEST_ASSERT_EQUAL_size_t(1, sample(probs, N, 0.24f));
	TEST_ASSERT_EQUAL_size_t(2, sample(probs, N, 0.26f));
	TEST_ASSERT_EQUAL_size_t(2, sample(probs, N, 1.0f));
}

void setUp(void) {}
void tearDown(void) {}

void test_grad(void) {
	const float probs[3] = {0.1f, 0.9f, 0.0f};
	const size_t y = 1;
	float logits_grad[3] = {5, 5, 5};
	float decay = 0.1f;

	// Apply accumulate_grad with decay = 0.1.
	ema_grad(probs, y, logits_grad, 3, decay);

	// Check expected gradients.
	float expected[] = {(1 - decay) * 5 + decay * (0.1f - 0),
	                    (1 - decay) * 5 + decay * (0.9f - 1),
	                    (1 - decay) * 5 + decay * (0.0f - 0)};

	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, logits_grad, 3);
}

void test_policy_forced(void) {
	policy_t p = create_policy(1, 3);
	p.decay = 0.1;
	float step_size = -1.0f;

	size_t context = 0;
	int y = 2;
	policy_step(&p, context, y);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f / p.vocab_size, p.probs[y]);

	for (size_t i = 0; i < 100; i++) {
		policy_step(&p, context, y);
		TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f,
		                         p.probs[0] + p.probs[1] + p.probs[2]);
		grad_step(&p, step_size);
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, 0, logf(p.probs[y]));
	policy_destroy(&p);
}

void test_policy_sampled(void) {
	policy_t p = create_policy(1, 5);
	p.decay = 0.1f;

	size_t context = 0;
	int y = p.vocab_size - 1;
	policy_step(&p, context, y);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f / p.vocab_size, p.probs[y]);

	for (size_t i = 0; i < 100; i++) {
		int y_hat = policy_step(&p, context, -1);
		grad_step(&p, (y_hat == y ? -1 : 1));
		printf("Iteration %zu: y_hat=%d -> p=[", i, y_hat);
		for (size_t j = 0; j < p.vocab_size; ++j) {
			printf("%.2f ", p.probs[j]);
		}
		printf("\b]\n");
	}
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, 0, logf(p.probs[y]));
	policy_destroy(&p);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_log_softmax);
	RUN_TEST(test_sample);
	RUN_TEST(test_grad);
	RUN_TEST(test_policy_forced);
	RUN_TEST(test_policy_sampled);
	return UNITY_END();
}
