"""Capture Audio from Pi PICO using serial interface"""

import os
import sys
import subprocess
import numpy as np
import serial
import serial.tools.list_ports
from matplotlib import pyplot as plt

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
        plt.plot(samples)
        plt.show()
        dc = np.mean(samples)
        samples = samples - dc
        spectrum = np.abs(np.fft.fftshift(np.fft.fft(samples)))
        plt.plot(spectrum)
        plt.show()
