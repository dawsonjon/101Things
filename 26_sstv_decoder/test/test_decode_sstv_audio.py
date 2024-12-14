from subprocess import run, Popen, PIPE
import numpy as np
from scipy.signal import hilbert
from matplotlib import pyplot as plt

Fs = 15000

run(["g++", "-DSIMULATION=true", "../decode_sstv.cpp", "../half_band_filter2.cpp", "../cordic.cpp", "test_decode_sstv_audio.cpp", "-o", "test_sstv_audio"])
uut = Popen("./test_sstv_audio", stdin=PIPE, stdout=PIPE)

def test_audio(filename, display=True):
  with open(filename, "rb") as inf:
    buffer = inf.read()

  array = np.frombuffer(buffer, "int16")
  array = np.array(array)

  image = np.zeros([266, 330, 3], dtype="int")
  plot_data = []
  for data in array:
      data = "%i \n"%(data)
      #print("python data:",  data)
      uut.stdin.write(bytes(data, "utf8"))
      uut.stdin.flush()
      data = uut.stdout.readline()
      pixel_complete, line, col, colour, pixel, phase = [int(i) for i in data.split()]
      plot_data.append(phase)

      if len(plot_data) == 50000:
        plt.plot(plot_data)
        plt.show()

      if pixel_complete:
        image[line, col, colour] = pixel
   
  if display:
    plt.imshow(image) 
    plt.show() 

#test_audio("../test_files/SSTV_sunset_audio.wav")
#test_audio("../test_files/test_07.wav")
#test_audio("../test_files/test.wav")
test_audio("../test_files/test_45.wav")
#test_audio("../test_files/test_09.wav")
#test_audio("../test_files/test_s2_25.wav")
#test_audio("../test_files/test_44.wav")

