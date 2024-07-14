#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: modulator_test.py
# description:
# License: MIT
#

import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "modulator_test.cpp",  "modulator.cpp",  "half_band_filter.cpp", "half_band_filter2.cpp",  "cordic.cpp", "-o",  "modulator"], check=True)
process = subprocess.run(["./modulator"], check=True, capture_output=True)
lines = process.stdout.splitlines()

def plot_results(lines):
  audio_list = []
  magnitude_list = []
  phase_list = []
  i_list = []
  q_list = []
  for line in lines:
    audio, i, q, magnitude, phase = line.split()
    audio_list.append(int(audio.strip()))
    magnitude_list.append(int(magnitude.strip()))
    phase_list.append(int(phase.strip()))
    i_list.append(int(i.strip()))
    q_list.append(int(q.strip()))

  plt.title("Magnitude/Phase output")
  plt.plot(magnitude_list, label = "Magnitude")
  plt.plot(phase_list, label = "Phase")
  plt.legend()
  plt.show()

  complex_data = np.array(i_list) + 1.0j * np.array(q_list)
  recovered_complex_data = 0.5 * np.array(magnitude_list) * np.exp(-1.0j * np.pi * np.array(phase_list) / 32768)

  plt.title("I/Q output")
  plt.plot(complex_data.real, label="real")
  plt.plot(complex_data.imag, label="imaginary")
  plt.show()

  spectrum = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(np.hanning(len(complex_data)) * complex_data))))
  recovered_spectrum = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(np.hanning(len(recovered_complex_data)) * recovered_complex_data))))


  plt.figure()
  plt.grid(True)
  plt.title("Modulator Frequency Response")
  plt.xlabel("Frequency (Hz)")
  plt.ylabel("Magnitude (dB)")
  plt.xlim([-6000, 6000])
  frequency_scale = np.linspace(-6000, 6000, len(spectrum))
  plt.plot(frequency_scale, spectrum, label="Modulator IQ output")
  plt.plot(frequency_scale, recovered_spectrum, label="Modulator Phase/Magnitude output")
  plt.legend()
  plt.show()

while lines:
  plot_results(lines[:8192])
  lines = lines[8192:]
