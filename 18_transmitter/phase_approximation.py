from math import sin, cos, pi, atan2
from matplotlib import pyplot as plt

def phase(i, q):
   absi = abs(i)
   if (q>=0):
      r = ((q - absi)<<5) / (q + absi);
      angle = 32 - r;
   else:
      r = ((q + absi)<<5) / (absi - q);
      angle = 96 - r;

   print(r)

   if (i < 0):
     return -angle
   else:
     return angle


stimulus = [(int(round(sin(2*pi*i/100)*127)), int(round(cos(2*pi*i/100)*127))) for i in range(100)]
response = [phase(i, q) for i, q in stimulus]
print(response)

plt.plot(response)
plt.plot([128 * atan2(i, q)/pi for i, q in stimulus])
plt.show()
