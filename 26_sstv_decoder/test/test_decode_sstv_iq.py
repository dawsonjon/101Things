from subprocess import run, Popen, PIPE
import numpy as np
from scipy.signal import hilbert
from matplotlib import pyplot as plt

Fs = 15000

run(["g++", "-DSIMULATION=true", "../decode_sstv.cpp", "../cordic.cpp", "test_decode_sstv_iq.cpp", "-o", "test_sstv_iq"])
uut = Popen("./test_sstv_iq", stdin=PIPE, stdout=PIPE)

def test_audio(filename, display=True):
  with open(filename, "rb") as inf:
    buffer = inf.read()

  array = np.frombuffer(buffer, "int16")
  array = np.array(array)
  array = hilbert(array)
  #phases = np.angle(array)
  #frequencies = (phases[1:]-phases[:-1]) % (np.pi)
  #frequencies *= Fs/(2*np.pi)
   
  #smoothed_data = 0 
  #for n in range(len(frequencies)): 
    #smoothed_data = smoothed_data*0.93 + frequencies[n]*0.07
    #frequencies[n] = smoothed_data 

  image = np.zeros([266, 330, 3], dtype="int")
  plot_data = []
  for data in array:
      data = "%i %i\n"%(data.real, data.imag)
      #print("python data:",  data)
      uut.stdin.write(bytes(data, "utf8"))
      uut.stdin.flush()
      data = uut.stdout.readline()
      pixel_complete, line, col, colour, pixel, phase = [int(i) for i in data.split()]
      plot_data.append(phase)

      if len(plot_data) == 20000:
        plt.plot(plot_data)
        plt.show()

      if pixel_complete:
        image[line, col, colour] = pixel
   
  if display:
    plt.imshow(image) 
    plt.show() 

#test_audio("../test_files/SSTV_sunset_audio.wav")
test_audio("../test_files/test_07.wav")
test_audio("../test_files/test.wav")
#test_audio("../test_files/test_45.wav")
#test_audio("../test_files/test_09.wav")
#test_audio("../test_files/test_s2_25.wav")
#test_audio("../test_files/test_44.wav")

