# Copyright Pololu Corporation.  For more information, see https://www.pololu.com/
# This example tests the speed of reads and writes to the slave device.

from a_star import AStar
import time

a_star = AStar()

import timeit

n = 500
total_time = timeit.timeit(a_star.test_write8, number=n)
write_kbits_per_second = 8 * n * 8 / total_time / 1000

print "Writes of 8 bytes: "+'%.1f'%write_kbits_per_second+" kilobits/second"

n = 500
total_time = timeit.timeit(a_star.test_read8, number=n)
read_kbits_per_second = 8 * n * 8 / total_time / 1000

print "Reads of 8 bytes: "+'%.1f'%read_kbits_per_second+" kilobits/second"
