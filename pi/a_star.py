# Copyright Pololu Corporation.  For more information, see https://www.pololu.com/
import smbus
import struct
import time
import threading

SLAVE_ADDRESS = 20

class AStar:
  # This lock allows multiple threads in a program to use their own AStar
  # instances without interfering with each other's I2C communications.
  lock = threading.Lock()

  def __init__(self):
    self.bus = smbus.SMBus(1)

  def read_unpack(self, address, size, format):
    # Ideally we could do this:
    #    byte_list = self.bus.read_i2c_block_data(SLAVE_ADDRESS, address, size)
    # But the AVR's TWI module can't handle a quick write->read transition,
    # since the STOP interrupt will occasionally happen after the START
    # condition, and the TWI module is disabled until the interrupt can
    # be processed.
    #
    # A delay of 0.0001 (100 us) after each write is enough to account
    # for the worst-case situation in our example code.
    AStar.lock.acquire()
    self.bus.write_byte(SLAVE_ADDRESS, address)
    time.sleep(0.0001)
    byte_list = [self.bus.read_byte(SLAVE_ADDRESS) for _ in range(size)]
    AStar.lock.release()
    return struct.unpack(format, bytes(byte_list))

  def write_pack(self, address, format, *data):
    data_array = list(struct.pack(format, *data))
    AStar.lock.acquire()
    self.bus.write_i2c_block_data(SLAVE_ADDRESS, address, data_array)
    time.sleep(0.0001)
    AStar.lock.release()

  def leds(self, red, yellow, green):
    self.write_pack(0, 'BBB', red, yellow, green)

  def play_notes(self, notes):
    self.write_pack(24, 'B15s', 1, notes.encode("ascii"))

  def motors(self, left, right):
    self.write_pack(6, 'hh', left, right)

  def read_buttons(self):
    return self.read_unpack(3, 3, "???")

  def read_battery_millivolts(self):
    return self.read_unpack(10, 2, "H")

  def read_analog(self):
    return self.read_unpack(12, 12, "HHHHHH")

  def read_encoders(self):
    return self.read_unpack(39, 4, 'hh')

  def test_read8(self):
    self.read_unpack(0, 8, 'cccccccc')

  def test_write8(self):
    AStar.lock.acquire()
    self.bus.write_i2c_block_data(SLAVE_ADDRESS, 0, [0,0,0,0,0,0,0,0])
    time.sleep(0.0001)
    AStar.lock.release()
