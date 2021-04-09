# Epsilon
Epsilon is a library with small functions for machine learning and statistics
written in plain C. The functions are well tested and decoupled.

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
properties. See the xorshift [example](examples/example_xorshift.c).

## Feature extraction
- [Structured random orthogonal features](docs/yu2016orf.pdf) (SORF). An `O(d
log d)` transformation that can be used for a feature map that approximates a
specific kernel. Here `d` is the number of input dimensions. Note that SORF
is [patented](https://patents.google.com/patent/US20180114145A1), and that
compilation is disabled by default.

## Regression
- Linear passive-aggressive regression (TODO)
- Kernelized passive-aggressive regression (TODO)
- Kernelized passive-aggressive classification (TODO)


# Other solutions for Tiny ML or Edge AI
- [TensorFlow Lite](https://www.tensorflow.org/lite/)
- [Edge Impulse](https://www.edgeimpulse.com)
- [Micro ML](https://github.com/eloquentarduino/EloquentMicroML)
- [Q-behave](https://github.com/smigielski/q-behave)
