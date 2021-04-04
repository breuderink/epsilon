# Epsilon
Epsilon is a library with small functions for machine learning and statistics
written in plain C. The functions are well tested and loosely coupled. Algorithms that can run on embedded systems are preferred:

- No dynamic memory allocation if possible.
- Favour online over batch operation.
- Work with fixed-point math when realistic.

# Algorithms

## Pseudo-random number generation
- [Xorshift](docs/marsaglia2003xrn.pdf) by George Marsaglia.

## Feature extraction
- SORF. 
