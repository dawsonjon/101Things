"""Capture Audio from Pi PICO using serial interface"""

import os
import sys
import subprocess
import numpy as np
from matplotlib import pyplot as plt
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

#filename = sys.argv[1]
#with open(filename, 'wb') as output_file:
with serial.Serial(port, 12000000, rtscts=1) as ser:
  sample_num = 0
  samples = 63 * np.sin(2.0 * np.pi * np.arange(sample_num, sample_num+1024) * 0.7e3 / sample_rate);
  #samples += 63 * np.sin(2.0 * np.pi * np.arange(sample_num, sample_num+1024) * 1.9e3 / sample_rate);
  samples = bytes(samples.astype("int8"))
  sample_num+=1024
  while 1:
    #plt.plot(samples)
    #plt.show()
    ser.write(samples)
    samples = 63 * np.sin(2.0 * np.pi * np.arange(sample_num, sample_num+1024) * 0.7e3 / sample_rate);
    samples += 63 * np.sin(2.0 * np.pi * np.arange(sample_num, sample_num+1024) * 1.9e3 / sample_rate);
    samples = bytes(samples.astype("int8"))
    sample_num+=1024
