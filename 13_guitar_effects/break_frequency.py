from math import tan, pi

frequencies = [100, 200, 400, 800, 200, 400, 800, 1600]
sample_frequency = 20000
for frequency in frequencies:
  break_frequency = frequency/sample_frequency;
  k = (tan(pi*break_frequency)-1)/(tan(pi*break_frequency)+1)
  print("frequency", frequency, k)
