# Raspberry Pi slave library for Arduino

Version: 1.0.0<br>
Release date: 2016 February 16<br>
[www.pololu.com](https://www.pololu.com/)

Summary
-------

This is an Arduino library that helps establish I<sup>2</sup>C communication with
a Raspberry Pi, with the Arduino acting as the I<sup>2</sup>C slave.  It should
work with most Arduino-compatible boards, but we designed it for use
with the
[A-Star 32U4 Robot Controller](https://www.pololu.com/product/3119),
which mounts conveniently to the Pi's GPIO header.  The idea is that
the Raspberry Pi can take care of high-level tasks like video
processing or network communication, while the A-Star takes care of
actuator control, sensor inputs, and other low-level tasks that the Pi
is incapable of.

There are a few reasons we made a library for this instead of
just recommending the standard Arduino I<sup>2</sup>C library, Wire.h:

* Wire.h just gives you access to raw I<sup>2</sup>C packets; you have to decide
  how they should relate to your data, and that's not trivial. This
  library implements a specific protocol allowing bidirectional access
  to a buffer, with atomic reads and writes.
* The processor on the Raspberry Pi has
  [a major bug](http://www.advamation.com/knowhow/raspberrypi/rpi-i2c-bug.html)
  that corrupts data except at very low speeds.  The standard Arduino
  I<sup>2</sup>C library, Wire.h, is not flexible enough to allow us to follow
  the suggested workarounds (delays inserted at key points).

We have included example Arduino code for the A-Star and Python code
for the Raspberry Pi.  Together, the examples set up a web server on
the Raspberry Pi that will let you remotely control and monitor a
robot from your smartphone or computer.

Getting started
---------------

A complete tutorial will be available soon.

Benchmarking
------------

The included script `benchmark.py` times reads and writes of 8 bytes,
to give you an idea of how quickly data can be transferred between the
devices.  Because of a limitation in the AVR's I<sup>2</sup>C module,
we have slowed down reads significantly.

| Bus speed | Reads     | Writes     |
| --------- | --------- | ---------- |
| 100 kHz   | 21 kbit/s | 53 kbit/s  |
| 400 kHz   | 43 kbit/s | 140 kbit/s |

Version history
---------------

* 1.0.0 (2016 Feb 16): Original release.
