# Epsilon

Epsilon is a C library of small, decoupled, and well-tested functions for machine learning and statistics. It is designed for microcontrollers and resource-constrained environments.

[![CI Meson](https://github.com/breuderink/epsilon/actions/workflows/ci_meson.yml/badge.svg)](https://github.com/breuderink/epsilon/actions/workflows/ci_meson.yml)

## Motivation
Most machine learning focuses on training large models on powerful hardware.
After training, researchers freeze the models and apply them to new data.
These models are too big to run on microcontrollers. One can compress these
models to make them fit. The compressed model can make predictions for new
data. But the model itself remains static, even if the environment changes.

An alternative approach is to optimize the model on the microcontroller
itself. In this case, the model can adapt to new data. This requires
particular memory-efficient algorithms. Further, the optimization process
should be reliable. Epsilon provides methods to train and apply machine
learning methods on microcontrollers.

These algorithms should run on microcontrollers, such as the
[Raspberry Pi Pico](https://www.raspberrypi.org/products/raspberry-pi-pico/),
the [BBC micro:bit](https://www.microbit.org), the 
[Arduino Nano 33 BLE Sense](https://store.arduino.cc/arduino-nano-33-ble-sense) 
or even the [ATtiny85](https://www.microchip.com/wwwproducts/en/attiny85)
8-bit microcontroller.

To allow machine learning to run on microcontrollers, the implementations:

- do not use dynamic memory allocation if possible,
- favour online over batch operation,
- work with fixed-point math when realistic, and
- are easy to tune.

directory, and configure the project. In the repository root, configure and
build the project, then run the unit tests and examples:

# Folder Structure

```
/           # Project root
├── LICENSE
├── README.md
├── meson.build
├── docs/           # Documentation and references
├── src/            # Source code (.c/.h)
├── tests/          # Unit tests
├── examples/       # Example programs
├── subprojects/    # External dependencies (e.g., Unity)
```

# Building & Testing
Epsilon uses Meson for building. In the repository root:

```bash
meson setup builddir
meson compile -C builddir
meson test -C builddir
```


# Algorithms

## Pseudo-random number generation
- [Xorshift](docs/marsaglia2003xrn.pdf): Fast, simple PRNG with good statistical properties. See `examples/example_rng.c`.

## Hashing
- [FNV hash](https://tools.ietf.org/html/draft-eastlake-fnv-17): Fast hash for feature hashing. See `examples/example_hash.c`.

## Statistics
- Welford's method: Online mean and variance in one pass. See `examples/example_stats.c`.

## Transformations
- Fast Walsh-Hadamard Transform (FWHT): O(n log n) transform, similar to FFT. See `examples/example_transform.c`.

## Passive-aggressive learning
- [Online passive-aggressive (PA)](docs/crammer2006opa.pdf): Regression with updates only on mistakes.


# Other solutions for Tiny ML or Edge AI
- [TensorFlow Lite](https://www.tensorflow.org/lite/)
- [Edge Impulse](https://www.edgeimpulse.com)
- [Micro ML](https://github.com/eloquentarduino/EloquentMicroML)
- [Q-behave](https://github.com/smigielski/q-behave)
