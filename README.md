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

- Do not use dynamic memory allocation if possible.
- Favour online over batch operation.
- Work with fixed-point math when realistic.
- Are easy to tune.

# Algorithms

## Pseudo-random number generation
- [Xorshift](docs/marsaglia2003xrn.pdf) is a very simple and vast
pseudo-random number generator by George Marsaglia that has good statistical
properties.

## Feature extraction
- [Structured random orthogonal features](docs/yu2016orf.pdf) (SORF). Note that SORF is patented.

## Regression
- Linear passive-aggressive regression (TODO)
- Kernelized passive-aggressive regression (TODO)