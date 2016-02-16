#!/usr/bin/python

# Copyright Pololu Corporation.  For more information, see https://www.pololu.com/

import urllib2
import time

while True:
  try:
    time.sleep(1)
    urllib2.urlopen("http://localhost:5000/heartbeat/1").read()
    time.sleep(0.01)
    urllib2.urlopen("http://localhost:5000/heartbeat/0").read()
  except urllib2.URLError:
    print("error")
