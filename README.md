# Epsilon
Epsilon is a library with small functions for machine learning and statistics
written in plain C. The functions are well tested and loosely coupled.

## Motivation
Mainstream machine learning focuses on large models on powerful hardware, and
requires large amounts of data. After training, models are frozen and applied
to new data. The strict separation between model training and application
make this approach more a different way to design software than an
intelligent system. In addition, the power and data inefficiency of deep
learning methods make it hard to apply these models locally.

The epsilon library contains building blocks for both training and using
machine learning underpowered devices, such as microcontrollers. 

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

# Other solutions for Tiny ML or Edge AI
- [TensorFlow Lite](https://www.tensorflow.org/lite/)
- [Edge Impulse](https://www.edgeimpulse.com)
- [Micro ML](https://github.com/eloquentarduino/EloquentMicroML)
- [Q-behave](https://github.com/smigielski/q-behave)