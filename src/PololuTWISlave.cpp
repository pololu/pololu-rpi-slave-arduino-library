// Copyright Pololu Corporation.  For more information, see https://www.pololu.com/

#include "Arduino.h"
#include "PololuTWISlave.h"
#include <util/twi.h>

static PololuTWISlave *slave;

void PololuTWISlave::init(uint8_t address, PololuTWISlave &my_slave)
{
  slave = &my_slave;
  TWAR = address << 1;
  digitalWrite(SDA, 1);
  digitalWrite(SCL, 1);
  ack();
}

ISR(TWI_vect)
{
  // Handle the TWI event.  This function also uses the TWDR register
  // to send/receive data, and the TWCR register to handle a bus
  // error.
  PololuTWISlave::handleEvent(TWSR);

  // I don't think there is a reason to ever NACK unless the master is
  // doing something invalid, which is a situation that we don't need
  // to support.  So, just ACK:
  PololuTWISlave::ack();
}

void PololuTWISlave::ack()
{
  TWCR =
    (1<<TWEN)    // enable TWI
    | (1<<TWIE)  // enable TWI interrupt
    | (1<<TWINT) // clear interrupt flag
    | (1<<TWEA); // ACK
}

void PololuTWISlave::nack()
{
  TWCR =
    (1<<TWEN)     // enable TWI
    | (1<<TWIE)   // enable TWI interrupt
    | (1<<TWINT); // clear interrupt flag
}

void PololuTWISlave::clearBusError()
{
  TWCR = (1<<TWEN) | (1<<TWSTO) | (1<<TWINT) | (1<<TWEA);
  // the hardware will now clear TWSTO automatically
}

uint8_t PololuTWISlave::handleEvent(uint8_t event)
{
  // See the ATmega32U4 datasheet for a list of I2C states and responses.
  // We are ignoring any master-related states and general calls.
  switch(event)
  {
    /*** Slave receiver mode ***/
    case TW_SR_SLA_ACK: // addressed and ACKed
      slave->start();
      break;
    case TW_SR_DATA_ACK: // received a data byte and ACKed
      slave->receive(TWDR);
      break;
    case TW_SR_STOP: // A STOP or repeated START
      slave->stop();
      break;
    case TW_SR_DATA_NACK: // received data, NACKed
      break;

    /*** Slave transmitter mode ***/
    case TW_ST_SLA_ACK: // addressed and ACKed (get ready to transmit the first byte)
    case TW_ST_DATA_ACK: // transmitted a byte and got ACK (get ready to transmit the next byte)
      TWDR = slave->transmit();
      break;
    case TW_ST_DATA_NACK: // transmitted a byte and got NACK -> done sending
    case TW_ST_LAST_DATA: // transmitted a byte with TWEA=0 - shouldn't happen
      break;

    /*** Misc states ***/
    case TW_NO_INFO: // not sure how this can happen - TWINT=0 according to datasheet
      // ideally we would not change TWCR after this event, but our code will ACK
      break;
    case TW_BUS_ERROR: // error on the bus - set TWSTO and TWINT and ACK
      PololuTWISlave::clearBusError();
      break;
  }
}
