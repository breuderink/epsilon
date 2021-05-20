# Epsilon
Epsilon is a library with small functions for machine learning and statistics
written in plain C. The functions are decoupled and well tested.

[![tests and examples](https://github.com/breuderink/epsilon/actions/workflows/tests.yml/badge.svg)](https://github.com/breuderink/epsilon/actions/workflows/tests.yml)

## Motivation
Most machine learning focuses on training large models on powerful hardware.
After training, researchers freeze the models and apply them to new data.
These models are too big to run on microcontrollers. One can compress these
models to make them fit. The compressed model can make predictions for new
data. But the model itself remains static, even if the environment changes.

An alternative approach is to optimise the model on the microcontroller
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

# Building
Epsilon uses CMake for building. Create a build directory, and configure the
project. In the repository root, configure and build the project. Then run
the unit tests and the examples with CTest:

```
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
$ ctest
```

# Algorithms
## Pseudo-random number generation
- [Xorshift](docs/marsaglia2003xrn.pdf) is a fast and simple
pseudo-random number generator by George Marsaglia that has good statistical
properties. See the [xorshift example](examples/example_xorshift.c).

## Hashing
- The [FNV hash](https://tools.ietf.org/html/draft-eastlake-fnv-17) is fast
hash function that maps variable length input to a fixed output
([example](examples/example_FNV_hash.c)). It can be used for [feature
hashing](https://en.wikipedia.org/wiki/Feature_hashing).

## Online statistics
- Welfords method computes mean and variance in a single pass. See the
[example of Welford's method](examples/example_Welfords_method.c).

## Feature extraction
- Fast Walsh-Hadamard transform (FWHT) implements the Walsh-Hardamard
transform in O(n log n) time. The FWHT similar to the fast Fourier transform
and the Haar transform. See the [FWHT example](examples/example_FWHT.c).
- [Structured random orthogonal features](docs/yu2016orf.pdf) (SORF). An O(d
log d) transformation that can be used for a feature map that approximates a
specific kernel. Here _d_ is the number of input dimensions. Note that SORF
is [patented](https://patents.google.com/patent/US20180114145A1), and that
compilation of SORF is disabled by default. Instead one can use a budgeted
kernel classification or regression.

## Regression
- [Online passive-aggressive (PA)](docs/crammer2006opa.pdf) regression solves a
regression problem by only updating the model on prediction mistakes. When the
target depends non-linearly on the inputs one can use a kernel that projects the
input onto a set of support vectors. [Kernel
methods](https://en.wikipedia.org/wiki/Kernel_method) such as the support vector
machine (SVM) work efficiently in high-dimensional feature spaces, but don't
easily scale to large datasets. To scale to large datasets one can [maintain a
budget](docs/wang2010opa.pdf) of support vectors.  The [example of budgeted kernel
passive aggressive (BKPA) regression ](examples/example_BPKA_regression.c) 
demonstrates how online, non-linear regression can be performed with a limited
memory budget.

## Classification
- Kernelized passive-aggressive classification (TODO).


# Other solutions for Tiny ML or Edge AI
- [TensorFlow Lite](https://www.tensorflow.org/lite/)
- [Edge Impulse](https://www.edgeimpulse.com)
- [Micro ML](https://github.com/eloquentarduino/EloquentMicroML)
- [Q-behave](https://github.com/smigielski/q-behave)
