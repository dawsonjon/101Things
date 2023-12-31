"""Capture Audio from Pi PICO using serial interface"""

import os
import sys
import subprocess
import numpy as np
import serial
import serial.tools.list_ports

#get a list of available serial ports
print("Available Ports")
portinfo = dict(enumerate(serial.tools.list_ports.comports()))
for idx, info in portinfo.items():
    port, desc, hwid = info
    print(idx, "{}: {} [{}]".format(port, desc, hwid))

#prompt user for port to connect to
while 1:
    print("Select COM port >")
    idx = input()
    port = portinfo.get(int(idx), None)
    if port is not None:
        port = port[0]
        break

sample_rate = 10172

filename = sys.argv[1]
with serial.Serial(port, 12000000, rtscts=1) as ser:
  while 1:
    with open(filename, 'rb') as input_file:
      samples = input_file.read(1024)
      while 1:
        ser.write(samples)
        samples = input_file.read(1024)
        if len(samples) < 1024:
          break
