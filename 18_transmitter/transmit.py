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

#prompt user for transmit frequency
while 1:
  print("Enter Frequency Hz>")
  frequency = input()
  try:
    frequency = float(frequency)
    frequency_ok = True
  except TypeError:
    frequency_ok = False
    pass

  if frequency < 500e3:
    print("frequency too low")
    frequency_ok = False

  if frequency > 30e6:
    print("frequency too high")
    frequency_ok = False

  if frequency_ok:
    break

#prompt user for transmit mode
while 1:
  print("Enter mode AM/FM/LSB/USB >")
  mode = input()

  if mode.upper() in ["AM", "FM", "LSB", "USB"]:
    break
mode = {"AM":"a", "FM":"f", "LSB":"l", "USB":"u"}[mode]


filename = sys.argv[1]
with serial.Serial(port, 12000000, rtscts=1) as ser:

    #clear any data in buffer
    while ser.in_waiting:
      ser.read(ser.in_waiting)

    #repeatedly play back input file
    while 1:
      with open(filename, 'rb') as input_file:

          ser.write(bytes("f%u m%s s"%(frequency, mode), "utf8"))
          while ser.in_waiting:
            ser.read(ser.in_waiting)

          samples = input_file.read(1024)
          while 1:
            ser.write(samples)
            samples = input_file.read(1024)
            if len(samples) < 1024:
              break
