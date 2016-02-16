# Copyright Pololu Corporation.  For more information, see https://www.pololu.com/
from a_star import AStar
import time

a_star = AStar()

while 1:
  a_star.leds(0,0,0)
  time.sleep(0.5)
  a_star.leds(1,1,1)
  time.sleep(0.5)
