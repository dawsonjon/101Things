import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "waveform_test.cpp",  "-o", "waveform"], check=True)
process = subprocess.run(["./waveform"], check=True, capture_output=True)
idx_list = []
sample_list = []
for line in process.stdout.splitlines():
  print(line)
  idx, sample = line.split()[:2]
  idx_list.append(int(idx.strip()))
  sample_list.append(float(sample.strip()))


bit = iter(sample_list)

periods = []
sample_num = 0
try:
  while 1:
    ones = 0
    while next(bit) == 1:
      ones+=1
      sample_num += 1
    print(sample_num, ones)
    zeros = 0
    while next(bit) == 0:
      zeros+=1
      sample_num += 1
    print(sample_num, zeros)
    periods.append(ones+zeros)
except:
  print(125e6/1e6)
  print(np.mean(periods))


#plt.plot(periods)
plt.plot(sample_list)
plt.show()

