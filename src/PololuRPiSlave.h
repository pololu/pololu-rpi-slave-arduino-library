// Copyright Pololu Corporation.  For more information, see https://www.pololu.com/

#pragma once
#include "PololuTWISlave.h"

/* PololuRPiSlave is an extension of PololuTWISlave that slows down
 * communication where necessary to work around the RPi I2C clock
 * stretching bug described here:
 *
 * http://www.advamation.com/knowhow/raspberrypi/rpi-i2c-bug.html
 *
 * The second template parameter, pi_delay_us, specifies the delay.
 * We recommend a value of 10 for an I2C speed of 100 kHz or a value
 * of 0 for 400 kHz.
 *
 * Additionally, it implements a system of buffers allowing user code
 * and the I2C system to read and write asynchronously from the same
 * data, without dictating any particular protocol.
 *
 * The data size is determined by the template parameter BufferType.
 * As described below, we allocate four copies of the buffer.  We
 * recommend keeping the buffer under 64 bytes.
 *
 * I2C writes are limited in the code to 16 bytes.
 *
 * You probably don't have to worry about the details below, since the
 * point of this buffering is to make it simple: all you need to do is
 * call updateBuffer() before using the buffer, do your writes and
 * reads, then call finalizeWrites() when you are done.  The I2C
 * master can read and write to the same data at any time, and you
 * should never encounter inconsistent data unless both sides attempt
 * to write to the same region simultaneously.
 *
 * Buffering details:
 *
 * The point is that reads and writes involving I2C and user code are
 * asynchronous and slow, but we want these slow operations to be
 * effectively atomic, so the two sides have to avoid reading and
 * writing from the same buffer at the same time.
 *
 * There is a central buffer (staging_buffer) that is synchronized
 * with three other buffers (buffer, buffer_old, i2c_read_buffer) when
 * appropriate; I2C reads are done directly from i2c_read_buffer, and
 * user code can read and write to "buffer" as desired.
 *
 * There is also a 16-byte buffer i2c_write_buffer, which stores
 * incoming I2C writes until they can be applied.
 */


template <class BufferType, unsigned int pi_delay_us>
  class PololuRPiSlave: public PololuTWISlave
{
private:
  uint8_t index;
  bool index_set = 0;
  uint8_t i2c_write_length = 0;
  uint8_t i2c_write_buffer[16];

  BufferType i2c_read_buffer;
  BufferType staging_buffer;
  BufferType buffer_old;

  void piDelay()
  {
    delayMicroseconds(pi_delay_us);
  }

  void updateI2CBuffer()
  {
    memcpy(&i2c_read_buffer, &staging_buffer, sizeof(BufferType));
  }

  void finalizeI2CWrites()
  {
    if(i2c_write_length == 0) return;

    for(uint8_t i=0; i < i2c_write_length; i++)
    {
      ((uint8_t *)&staging_buffer)[i+index] = i2c_write_buffer[i];
    }
    i2c_write_length = 0;
  }

public:

  BufferType buffer;

  void updateBuffer()
  {
    cli();
    memcpy(&buffer, &staging_buffer, sizeof(BufferType));
    sei();
    memcpy(&buffer_old, &buffer, sizeof(BufferType));
  }

  void finalizeWrites()
  {
    uint8_t i;
    cli();
    for(i=0; i < sizeof(BufferType); i++)
    {
      if(((uint8_t *)&buffer_old)[i] != ((uint8_t *)&buffer)[i])
        ((uint8_t *)&staging_buffer)[i] = ((uint8_t *)&buffer)[i];
    }
    sei();
  }

  virtual void receive(uint8_t b)
  {
    piDelay();
    if(!index_set)
    {
      updateI2CBuffer();
      index = b;
      index_set = true;
    }
    else
    {
      // Wrap writes at the end of the buffer
      if(i2c_write_length > sizeof(i2c_write_buffer))
        i2c_write_length = 0;

      // Write the data to the buffer
      i2c_write_buffer[i2c_write_length] = b;
      i2c_write_length ++;
    }
  }

  virtual uint8_t transmit()
  {
    piDelay();
    return ((uint8_t *)&i2c_read_buffer)[index++];
  }

  virtual void start()
  {
    piDelay();
    index_set = false;
  }

  virtual void stop()
  {
    finalizeI2CWrites();
  }

  /* Initialize the slave on a given address. */
  void init(uint8_t address)
  {
    PololuTWISlave::init(address, *this);
  }
};
