#!/usr/bin/env python3

# Copyright Pololu Corporation.  For more information, see https://www.pololu.com/

import urllib.request
import time

while True:
  try:
    time.sleep(1)
    urllib.request.urlopen("http://localhost:5000/heartbeat/1").read()
    time.sleep(0.01)
    urllib.request.urlopen("http://localhost:5000/heartbeat/0").read()
  except urllib.request.URLError:
    print("error")
