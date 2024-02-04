#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: half_band_filter_test.py
# description:
# License: MIT
#

import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "half_band_filter_test.cpp", "half_band_filter.cpp", "-o",  "half_band_filter"], check=True)
process = subprocess.run(["./half_band_filter"], check=True, capture_output=True)
audio_list = []
for line in process.stdout.splitlines():
  audio_list.append(int(line.strip()))

plt.plot(audio_list)
plt.show()

complex_data = np.array(audio_list)

spectrum = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(np.hanning(len(complex_data)) * complex_data))))
plt.plot(spectrum)
plt.show()

#plt.plot(audio_list)
#plt.plot(magnitude_list)
#plt.plot(phase_list)
  
