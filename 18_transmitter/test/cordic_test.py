#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: cordic_test.py
# description:
# License: MIT
#

import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "cordic_test.cpp", "../pico_transmitter/cordic.cpp",  "-o",  "cordic"], check=True)
process = subprocess.run(["./cordic"], check=True, capture_output=True)
i_list = []
q_list = []
r_list = []
t_list = []
for line in process.stdout.splitlines():
  i, q, r, t = line.split()
  i_list.append(int(i.strip()))
  q_list.append(int(q.strip()))
  r_list.append(int(r.strip()))
  t_list.append(int(t.strip()))

plt.grid(True)
plt.title("CORDIC Test Response")
plt.xlabel("Time (samples)")
plt.ylabel("Amplitude/Phase")
plt.plot(i_list, label="Rectangular (I)")
plt.plot(q_list, label="Rectangular (Q)")
plt.plot(r_list, label="Magnitude")
plt.plot(t_list, label="Phase")
plt.show()
