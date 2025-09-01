

# Epsilon
Epsilon is a minimal C library for machine learning and statistics. No dynamic
memory allocation, online algorithms, easy to use and extend.

Planned: 
- CSV reading
- Feature hashing
- Passive-aggressive regression/classification
- More metrics

## Getting started
Build with Meson:

```bash
meson setup builddir
meson test -C builddir
```

See `examples/` for usage.


## Algorithms
- [Xorshift](docs/marsaglia2003xrn.pdf): PRNG
- [FNV hash](https://tools.ietf.org/html/draft-eastlake-fnv-17): Feature hashing
- Welford's method: Online mean/variance
- Fast Walsh-Hadamard Transform: O(n log n) transform
- Passive-aggressive learning (planned)
- Online learning utilities (planned)
- More metrics (planned)


## License
BSD 3-Clause License. See `LICENSE`.



[![CI Meson](https://github.com/breuderink/epsilon/actions/workflows/ci_meson.yml/badge.svg)](https://github.com/breuderink/epsilon/actions/workflows/ci_meson.yml)