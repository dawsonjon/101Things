from matplotlib import pyplot as plt
import numpy as np

clock_frequency = 125e6
output_frequency = 3.5e6
waveform_length = 256
normalised_frequency = output_frequency/clock_frequency
period_clocks = clock_frequency/output_frequency
index_increment = waveform_length % period_clocks
num_full_periods = np.floor(waveform_length/period_clocks)
wrap = period_clocks * num_full_periods

print(period_clocks, waveform_length/period_clocks, index_increment)

waveform = np.sin(np.arange(0, waveform_length*2) * normalised_frequency * 2.0 * np.pi)
advanced_waveforms = []
for i in range(256):
  advanced_waveforms.append(waveform[i:256+i])

for w in advanced_waveforms:
  plt.plot(w)
plt.show()

index = 0
output_waveform = []
last_idx = 0
for i in range(100):
  idx = int(np.floor(index)) & 0xff
  print(idx, idx - last_idx)
  last_idx = idx
  output_waveform.extend(advanced_waveforms[idx])
  index += index_increment
  if index > wrap:
    index -= wrap

plt.plot(output_waveform)
plt.show()  

