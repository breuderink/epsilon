#include <epsilon.h>
#include <stdio.h>

int main() {
	online_stats_t stats = {0};

	for (int i = 0; i < 30; ++i) {
		float observation = i % 2;
		observe(&stats, observation);

        float mean_o = mean(&stats);
        float pvar_o = pvariance(&stats);
		printf("Observed %.2f -> mean = %.2f, pop. variance = %.2f.\n",
		       observation, mean_o, pvar_o);
	}
	return 0;
}
