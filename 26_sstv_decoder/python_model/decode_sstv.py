import numpy as np
from scipy.signal import hilbert
from matplotlib import pyplot as plt

dt = np.dtype("int16")
dt = dt.newbyteorder('<')
Fs = 15000

last = 0
#with open("SSTV_sunset_audio.wav" , "rb") as inf:
#with open("test.wav" , "rb") as inf:
#with open("test_07.wav" , "rb") as inf:
with open("../test_files/test_s2_25.wav" , "rb") as inf:
#with open("test_45.wav" , "rb") as inf:
#with open("test_1_35.wav" , "rb") as inf:

  buffer = inf.read(1<<22)
  array = np.frombuffer(buffer, dtype=dt)

array = np.array(array)

array = hilbert(array)
phases = np.angle(array)
frequencies = (phases[1:]-phases[:-1]) % (np.pi)
frequencies *= Fs/(2*np.pi)

smoothed_data = 0
for n in range(len(frequencies)):
  smoothed_data = smoothed_data*0.93 + frequencies[n]*0.07
  frequencies[n] = smoothed_data


#plt.plot(array.real)
#plt.plot(array.imag)
#plt.show()

#plt.plot(np.arange(len(frequencies))/Fs, frequencies)
#plt.ylim(1100, 2300)
#plt.show()

def decode_vis(number):

  if (number & 0x30) == 0x20:
    mode = "martin"
    hsync_pulse_ms = 4.862
    colour_gap_ms = 0.572
  elif (number & 0x30) == 0x30:
    mode = "scottie"
    hsync_pulse_ms = 9
    colour_gap_ms = 1.5
  else:
    #default to martin
    mode = "martin"
    hsync_pulse_ms = 4.862
    colour_gap_ms = 0.572

  if (number & 0x8):
    height = 256
  else:
    height = 128

  if (number & 0x4):
    width = 320
    if mode == "scottie":
      colour_time_ms = 138.240
    else:
      colour_time_ms = 146.342
  else:
    width = 160
    if mode == "scottie":
      colour_time_ms = 88.064
    else:
      colour_time_ms = 73.216

  print(number, mode, width, height)
  return mode, width, height, colour_time_ms, colour_gap_ms, hsync_pulse_ms


def decode(samples, Fs):
  vsync_samples = int(Fs*300/1000) 
  vsync_gap_samples = int(Fs*10/1000) 
  samples_per_bit = int(Fs*30/1000) 

  #default values get replaced when vis is decoded
  height = 256
  width = 320

  n = 0
  sync_counter = 0
  y_pixel = 0
  state = "vsync_1"
  image = np.zeros([height+10, width+10, 3], dtype="int")
  plot_data = []
  smoothed_sample = 0
  last_hsync_sample= 0
  confirmed_sync_sample = None
  while 1:

    #VSYNC/HEADER
    #1900Hz for 300ms
    if state == "vsync_1":

      if samples[n] < 2100 and samples[n] > 1700:
        sync_counter += 1
      else:
        sync_counter = max(0, sync_counter - 1)

      if sync_counter > 3*vsync_samples/4:
        sync_counter = 0
        state = "vsync_2"
        timeout = (vsync_samples/4)+vsync_gap_samples

    #1200Hz for 10ms
    elif state == "vsync_2":

      if samples[n] < 1400:
        sync_counter += 1
      else:
        sync_counter = max(0, sync_counter - 1)

      timeout -= 1
      if (sync_counter > vsync_gap_samples/4) or (timeout == 0):
        timeout = vsync_samples
        sync_counter = 0
        state = "vsync_3"

    #1900Hz for 300ms
    elif state == "vsync_3":

      if samples[n] < 2100 and samples[n] > 1700:
        sync_counter += 1
      else:
        sync_counter = max(0, sync_counter - 1)

      timeout -= 1
      if (sync_counter > 3*vsync_samples/4) or (timeout == 0):
        timeout = vsync_samples/4
        sync_counter = 0
        state = "vis_start"

    #VIS identifier
    elif state == "vis_start":
      if samples[n] < 1550:
        sync_counter += 1
      else:
        sync_counter = max(0, sync_counter - 1)

      timeout -= 1
      if (sync_counter > 2) or (timeout == 0):
        vis_sample = 3
        vis_accumulator = np.zeros(10)
        state = "vis_get_bit"

    elif state == "vis_get_bit":
      bit = int(vis_sample//samples_per_bit)
      if vis_sample >= (10*samples_per_bit):

          vis_threshold = (vis_accumulator[0]+vis_accumulator[9])/2
          vis_data = (vis_accumulator<vis_threshold)[1:8]
          vis = sum([int(vis_data[i])<<i for i in range(7)])

          mode, width, height, colour_time_ms, colour_gap_ms, hsync_pulse_ms = decode_vis(vis)
          samples_per_colour_gap = Fs*colour_gap_ms/1000.0
          samples_per_colour_line = Fs*(colour_time_ms+colour_gap_ms)/1000.0
          if mode == "martin":
            samples_per_line = Fs*((colour_time_ms*3)+(colour_gap_ms*4) + hsync_pulse_ms)/1000.0
          else:
            samples_per_line = Fs*((colour_time_ms*3)+(colour_gap_ms*3) + hsync_pulse_ms)/1000.0
          samples_per_pixel = samples_per_colour_line/width
          samples_per_hsync = Fs*hsync_pulse_ms/1000.0
          mean_samples_per_line = samples_per_line

          timeout = samples_per_colour_gap
          sync_counter = 0
          state = "wait_rising_edge"

      else:
        vis_accumulator[bit] += samples[n]
        vis_sample += 1

    elif state == "wait_rising_edge":

      if samples[n] > 1400:
        sync_counter += 1
      else:
        sync_counter = max(0, sync_counter - 1)

      timeout -= 1
      if (sync_counter > samples_per_colour_gap/2) or (timeout == 0):
          state = "decode_line"
          pixel_accumulator = 0
          pixel_n = 0
          last_x = 0
          image_sample = 0

    #1200Hz for n ms
    elif state == "decode_line":

      x, y, colour = sample_to_pixel(image_sample, mode, mean_samples_per_line, samples_per_colour_line, samples_per_pixel, samples_per_hsync)

      if x == 0 and colour == 1:
        pass
        #print(n, y)
        #plt.plot(samples[:n+1])
        #plt.show()

      if x != last_x:
        if colour < 3:
          image[y][x][colour] = int(pixel_accumulator//pixel_n)
          pixel_accumulator = 0
          pixel_n = 0
          last_x = x

      #end of image
      if y == 256:
        break
      if n==len(samples):
        break

      #slant detection
      if samples[n] < 1300 and samples[n-1] > 1300:

        #confirm sync if close to expected time
        print(n - last_hsync_sample, samples_per_line, samples_per_line * 0.99, samples_per_line * 1.01)
        if (n - last_hsync_sample) > samples_per_line * 0.99 and (n - last_hsync_sample) < samples_per_line * 1.01:
          print("blah")
          if confirmed_sync_sample is None:
            confirmed_sync_sample = n
          else:
            num_lines = round((n-confirmed_sync_sample)/samples_per_line)
            mean_samples_per_line = mean_samples_per_line * 0.7 + ((n-confirmed_sync_sample)/num_lines) * 0.3
            print(num_lines, n, last_hsync_sample, n-last_hsync_sample, samples_per_line, mean_samples_per_line)
        last_hsync_sample = n

      #colour pixels
      brightness = min(max(samples[n], 1500), 2300)
      brightness = 256*((brightness-1500)/(2300-1500))
      pixel_accumulator += brightness
      pixel_n += 1
      image_sample += 1


    n += 1
  plt.imshow(image)
  plt.show()

def sample_to_pixel(sample, mode, samples_per_line, samples_per_colour_line, samples_per_pixel, samples_per_hsync):

  """Use synchronous detection, from the sample number we can work out what the colour and x/y coordinates should be"""

  if mode == "martin":

    y = int(sample//samples_per_line)
    sample -= y*samples_per_line
    colour = int(sample//samples_per_colour_line)
    sample -= colour*samples_per_colour_line
    colour = [1, 2, 0, 3][colour] #martin colour order is g-b-r, map to r-g-b
    x = int(sample//samples_per_pixel)

  elif mode == "scottie":

    y = int(sample//samples_per_line)
    sample -= y*samples_per_line
    colour = int(sample//samples_per_colour_line)
    sample -= colour*samples_per_colour_line

    #hsync is between blue and red component (not at end of line)
    #for red component, subtract the length of the scan-line
    if colour == 2:
        sample -= samples_per_hsync
    if sample < 0:
        #return colour 3 for non-displayable pixels (e.g. during hsync)
        return 0, 0, 3

    colour = [1, 2, 0, 3][colour] #scottie colour order is g-b-r, map to r-g-b
    x = int(sample//samples_per_pixel)

  return x, y, colour
    


decode(frequencies, Fs)

  

