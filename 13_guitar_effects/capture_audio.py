#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: capture_audio.py
# description:
# License: MIT
#

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

if len(sys.argv) > 1:
  filename = sys.argv[1]
  with open(filename, 'wb') as output_file:
    with serial.Serial(port, 12000000, rtscts=1) as ser:
        header = ser.read(8)
        buffer = ser.read(2048+8)
        offset = buffer.find(b"AAAACCCC")
        ser.read(offset)
        while 1: 
          header = ser.read(8)
          if header != b"AAAACCCC":
            print(header)
            print("sync lost")
          samples = ser.read(2048)
          samples = np.frombuffer(samples, dtype="<u2") * 16
          output_file.write(samples.tobytes())
else:
  process = subprocess.Popen(["play", "-t", "u16", "-L", "-r", "20000", "-"], stdin=subprocess.PIPE)
  with serial.Serial(port, 12000000, rtscts=1) as ser:
      header = ser.read(8)
      buffer = ser.read(2048+8)
      offset = buffer.find(b"AAAACCCC")
      ser.read(offset)
      while 1: 
        header = ser.read(8)
        if header != b"AAAACCCC":
          print(header)
          print("sync lost")
        samples = ser.read(2048)
        samples = np.frombuffer(samples, dtype="<u2") * 16
        process.stdin.write(samples.tobytes())
