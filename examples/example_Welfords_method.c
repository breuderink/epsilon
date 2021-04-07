#include <epsilon.h>
#include <stdio.h>

int main() {
	online_stats_t stats = {0};

	for (int i = 0; i < 30; ++i) {
		float observation = i % 2;
		observe(&stats, observation);

        float mean_est = mean(&stats);
        float var_est = var(&stats);
		printf("Observed %.2f -> updated mean to %.2f and variance to %.2f.\n",
		       observation, mean_est, var_est);
	}
	return 0;
}
