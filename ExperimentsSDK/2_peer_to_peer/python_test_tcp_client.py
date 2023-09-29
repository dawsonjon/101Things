import socket
import time
import numpy as np
from matplotlib import pyplot as plt

# Set the server address here like 1.2.3.4
SERVER_ADDR = '192.168.1.120'

# These constants should match the server
BUF_SIZE = 2048
SERVER_PORT = 4242
TEST_ITERATIONS = 10

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((SERVER_ADDR, SERVER_PORT))
print("connected")


plot = []
phase = 0
for i in range(100):
  print(phase/10000)
  samples = np.round(127.0 * (np.sin(2 * np.pi * 200/10000 * np.arange(phase, phase + 2048)) + 1))
  plot.extend(samples)
  samples = bytes([int(i) for i in samples])
  s.sendall(samples);
  phase += 2048

#plt.plot(plot)
#plt.show()

s.close()
print("test completed")
