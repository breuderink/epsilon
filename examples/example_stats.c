#include "epsilon/stats.h"
#include <stdio.h>

int main(void) {
	online_stats_t stats = {0};

	for (int i = 0; i < 10; ++i) {
		float observation = i % 2;
		observe(&stats, observation);

		float mean_o = mean(&stats);
		float pvar_o = pvariance(&stats);
		float var_o = variance(&stats);
		printf("Observed %.2f -> mean = %.2f, pop. var = %.2f, var = %.2f.\n",
		       observation, mean_o, pvar_o, var_o);
	}
	return 0;
}
