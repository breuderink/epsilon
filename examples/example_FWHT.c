#include <fwht.h>
#include <stdio.h>

#define LOG2_DIMS 3
#define DIMS (1 << LOG2_DIMS)

void display(float x[DIMS]) {
	for (int i = 0; i < DIMS; ++i) {
		printf("%5.1f ", x[i]);
	}
	printf("\n");
}

int main(void) {
	// Define and display input vector.
	float x[DIMS] = {-3, -3, -3, -3, 3, 3, 3, 3};
	printf("x =\n");
	display(x);

	// Transform input, and display. Note that it is sparse now.
	FWHT(x, LOG2_DIMS);
	printf("\nFWHT(x) =\n");
	display(x);

	// Transform again. Note that it is the (scaled) input again.
	FWHT(x, LOG2_DIMS);
	printf("\nFWHT(FWHT(x)) =\n");
	display(x);

	// Print Hadamard matrix.
	printf("\nHadamard matrix:\n");
	for (int d = 0; d < DIMS; ++d) {
		// Create unit vector.
		float row[DIMS] = {0};
		row[d] = 1.0f;

		// Perform fast Walsh-Hadamard transform.
		FWHT(row, LOG2_DIMS);

		// Print result that forms a row of the Hadamard matrix. Note the
		// pattern in the sign changes. For more information see
		// https://en.wikipedia.org/wiki/Hadamard_transform.
		display(row);
	}
	return 0;
}
