import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "modulator_test.cpp",  "modulator.cpp",  "half_band_filter2.cpp",  "cordic.cpp", "-o",  "modulator"], check=True)
process = subprocess.run(["./modulator"], check=True, capture_output=True)
audio_list = []
magnitude_list = []
phase_list = []
for line in process.stdout.splitlines():
  audio, magnitude, phase = line.split()
  audio_list.append(int(audio.strip()))
  magnitude_list.append(int(magnitude.strip()))
  phase_list.append(int(phase.strip()))

plt.plot(magnitude_list)
plt.plot(phase_list)
plt.show()

complex_data = np.array(magnitude_list) * np.exp(1.0j * np.pi * np.array(phase_list) / 128)
#complex_data = np.array(magnitude_list) + 1.0j * np.array(phase_list)

plt.plot(complex_data.real)
plt.plot(complex_data.imag)
plt.show()

spectrum = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(np.hanning(len(complex_data)) * complex_data))))
plt.plot(spectrum)
plt.show()

#plt.plot(audio_list)
#plt.plot(magnitude_list)
#plt.plot(phase_list)
  
