#include <assert.h>
#include <string.h>
#include <unity.h>

void softmax(const float *z, float *s, size_t n) {
	float max = 0.0f;
	for (size_t i = 0; i < n; i++) {
		if (z[i] > max) {
			max = z[i];
		}
	}

	float sum = 0.0f;
	for (size_t i = 0; i < n; i++) {
		s[i] = expf(z[i] - max);
		sum += s[i];
	}

	for (size_t i = 0; i < n; i++) {
		s[i] /= sum;
	}
}

float cross_entropy_loss(const float *p, const float *y, size_t n) {
	float loss = 0.0f;
	for (size_t i = 0; i < n; i++) {
		loss -= y[i] * logf(p[i] + 1e-15f);
	}
	assert(isfinite(loss));
	return loss;
}

void accumulate_grad(const float *p, const float *y, float *z_grad, size_t n,
                     float ff) {
	assert(0 < ff && ff <= 1);
	for (size_t i = 0; i < n; i++) {
		float grad = p[i] - y[i];
		z_grad[i] += ff * (grad - z_grad[i]);
	}
}

void test_softmax(void) {
	const size_t n = 3;
	const float input[n] = {1.0f, 2.0f, 3.0f};
	float output[n];

	softmax(input, output, n);

	float expected[n] = {0.09003057f, 0.24472847f, 0.66524096f};
	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, output, n);
}

void test_cross_entropy_loss(void) {
	const size_t n = 3;
	const float p[n] = {0.1f, 0.9f, 0.0f};
	const float y[n] = {0.0f, 1.0f, 0.0f};

	float loss = cross_entropy_loss(p, y, n);
	TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.10536052f, loss);
}

void test_accumulate_grad(void) {
	const size_t n = 3;
	const float p[n] = {0.1f, 0.9f, 0.0f};
	const float y[n] = {0.0f, 1.0f, 0.0f};

	// Set initial gradients to 5.
	float z_grad[n] = {5.0f, 5.0f, 5.0f};

	// Apply accumulate_grad with ff = 0.1.
	const float ff = 0.1f;
	accumulate_grad(p, y, z_grad, n, ff);

	// Check expected gradients.
	float expected[n] = {(1 - ff) * 5.0f + ff * (0.1f - 0.0f),
	                     (1 - ff) * 5.0f + ff * (0.9f - 1.0f),
	                     (1 - ff) * 5.0f + ff * (0.0f - 0.0f)};

	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-4f, expected, z_grad, n);
}

void adapt(float *z, float *z_grad, size_t n, float lr) {
	for (size_t i = 0; i < n; i++) {
		z[i] -= lr * z_grad[i];
	}
}

void test_adapt(void) {
	const size_t n = 3;
	float z[n] = {0.0f};
	float z_grad[n] = {0.0f};

	float y[n] = {0.0f, 1.0f, 0.0f};
	float p[n];

	for (size_t i = 0; i < 100; i++) {
		softmax(z, p, n);
		accumulate_grad(p, y, z_grad, n, 0.1f);
		adapt(z, z_grad, n, 1e0f);
	}
	TEST_ASSERT_FLOAT_ARRAY_WITHIN(1e-2f, y, p, n);
	TEST_ASSERT_FLOAT_WITHIN(1e-2f, cross_entropy_loss(p, y, n), 0.0f);
}

void setUp(void) {}
void tearDown(void) {}
int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_softmax);
	RUN_TEST(test_cross_entropy_loss);
	RUN_TEST(test_accumulate_grad);
	RUN_TEST(test_adapt);
	return UNITY_END();
}
