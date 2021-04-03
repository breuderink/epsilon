#include <stdint.h>
#include <stddef.h>

uint32_t xorshift32(uint32_t y);

void fx_sorf(float *x, uint8_t nbits);
void fx_fwht(float *x, uint8_t nbits);
void fx_repeat(float *x1, size_t n1, float *x2, size_t n2);
