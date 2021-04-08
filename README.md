# Epsilon
Epsilon is a library with small functions for machine learning and statistics
written in plain C. The functions are well tested and loosely coupled.

## Motivation
Most machine learning focuses on training large models on powerful
hardware. After training, models are frozen and applied to new data. These
trained models can be compressed to allow making predictions on constrained
hardware such as microcontrollers. This compressed model can be applied to
new data, but the model itself is static.

An alternative approach is to optimize the model on the target hardware. This
requires particular efficient and robust algorithms, since there is no
researcher to guide the process.

The epsilon library contains building blocks for both training and applying
machine learning models on such underpowered devices, such as
microcontrollers.

These algorithms should function on microcontrollers, such as the
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

# Algorithms

## Pseudo-random number generation
- [Xorshift](docs/marsaglia2003xrn.pdf) is a fast and simple
pseudo-random number generator by George Marsaglia that has good statistical
properties. See the xorshift [example](examples/example_xorshift.c).

## Feature extraction
- [Structured random orthogonal features](docs/yu2016orf.pdf) (SORF). An `O(d
log d)` transformation that can be used for a feature map that approximates a
specific kernel. Here `d` is the number of input dimensions. Note that SORF
is [patented](https://patents.google.com/patent/US20180114145A1).

## Regression
- Linear passive-aggressive regression (TODO)
- Kernelized passive-aggressive regression (TODO)

# Building
Epsilon uses CMake for building. Create a build directory, and configure the
project. In the repository root, configure and build the project:

```
$ mkdir -p build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build .
```

Next, run the unit test using CTest:

```
$ ctest -C Release
```

# Other solutions for Tiny ML or Edge AI
- [TensorFlow Lite](https://www.tensorflow.org/lite/)
- [Edge Impulse](https://www.edgeimpulse.com)
- [Micro ML](https://github.com/eloquentarduino/EloquentMicroML)
- [Q-behave](https://github.com/smigielski/q-behave)