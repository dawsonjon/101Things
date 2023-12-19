import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "filter_test.cpp",  "half_band_filter2.cpp",  "-o",  "filter"], check=True)
process = subprocess.run(["./filter"], check=True, capture_output=True)
i_list = []
q_list = []
for line in process.stdout.splitlines():
  i, q = line.split()
  i_list.append(int(i.strip()))
  q_list.append(int(q.strip()))

complex_data = np.array(i_list) +  1.0j * np.array(q_list)

plt.plot(complex_data.real)
plt.plot(complex_data.imag)
plt.show()

spectrum = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(np.hanning(len(complex_data)) * complex_data))))
plt.plot(spectrum)
plt.show()

#plt.plot(audio_list)
#plt.plot(magnitude_list)
#plt.plot(phase_list)
  
