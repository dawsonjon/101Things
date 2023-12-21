import subprocess
import numpy as np
from matplotlib import pyplot as plt

subprocess.run(["g++",  "cordic_test.cpp", "cordic.cpp",  "-o",  "cordic"], check=True)
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

plt.plot(i_list)
plt.plot(q_list)
plt.plot(r_list)
plt.plot(t_list)
plt.show()
