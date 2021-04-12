#include <epsilon.h>
#include <stdio.h>

#define LOG2_DIMS 3
#define DIMS (1 << LOG2_DIMS)

int main() {
	for (int d = 0; d < DIMS; ++d) {
		// Create unit vector.
		float x[DIMS] = {0};
		x[d] = 1.0f;

		// Perform fast Walsh-Hadamard transform.
		FWHT(x, LOG2_DIMS);

		// Print result that forms a row of the Hadamard matrix. Note the
		// pattern in the sign changes. For more information see
		// https://en.wikipedia.org/wiki/Hadamard_transform.
		for (int i = 0; i < DIMS; ++i) {
			printf("%5.1f ", x[i]);
		}
		printf("\n");
	}
	return 0;
}
