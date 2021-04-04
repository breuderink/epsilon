#include <stdint.h>
#include <stddef.h>

uint32_t xorshift32(uint32_t y);

// Perform in-place Fast Walsh-Hadamard transform.
void FWHT(float *x, uint8_t nbits);

// SORF is a pseudorandom matrix transformation based on fast Walsh-Hadamard
// transform and sign flipping. Based on [1, 2].
void fx_SORF(float *x, uint8_t nbits);

// Repeat elements in x1 to form a vector x2 to increase input dimensions for
// SORF.
void fx_repeat(float *x1, size_t n1, float *x2, size_t n2);

/*
# References
[1] Felix, X. Yu, et al. "Orthogonal random features." Advances in Neural
    Information Processing Systems. 2016.

[2] Choromanski, Krzysztof, and Vikas Sindhwani. "Recycling randomness
    with structure for sublinear time kernel expansions." International
    Conference on Machine Learning. 2016.
*/