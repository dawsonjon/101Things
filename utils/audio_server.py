import time
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

#Use sox utility to stream access sound card
#play_process = subprocess.Popen(["play", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdin=subprocess.PIPE)

with serial.Serial(port, 12000000, rtscts=1) as ser:
  while 1:

    samples = ser.readline()
    print(samples)
    #play_process.stdin.write(samples)

