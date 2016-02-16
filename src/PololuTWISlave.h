// Copyright Pololu Corporation.  For more information, see https://www.pololu.com/

#pragma once
#include <stdint.h>

/* PololuTWISlave is a basic AVR I2C slave library that is lightweight
 * and fast.  Unlike the standard Arduino library Wire.h, it does not
 * enforce a particular style of buffering the data - you get to
 * handle the bytes and events one at a time.
 *
 * To use this library, inherit from PololuTWISlave and implement the
 * four virtual functions that specify how to receive and transmit
 * bytes and how to handle the start and stop signals.
 *
 * The library does not support master mode, general calls, error
 * states, and possibly other features of I2C - it only does the
 * minimum required to establish communication with a master that we
 * control.
 */

class PololuTWISlave
{
public:
  /* Methods for a slave to declare.  These methods will be called
   * from the ISR, with clock stretching used to delay further bus
   * activity until they return. */
  virtual void receive(uint8_t b) = 0;
  virtual uint8_t transmit() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;

  /* Initialize slave on a specific address; do not respond to general calls. */
  static void init(uint8_t address, PololuTWISlave &slave);

  /* Low-level static methods not meant to be called by users. */
  static uint8_t handleEvent(uint8_t event);
  static void ack();
  static void nack();
  static void clearBusError();
};
