from diagram import *

#delay diagram
################################################################################

d = Diagram(38, 18, "Delay")
inp = IO(d, 5, 5)
node = Join(d, 10, 5)
adder = Adder(d, 25, 5)
adder.arrow(adder.bottom())
outp = IO(d, 30, 5)
delay = TextBlock(d, 15, 10, "D")
gain = GainBlock(d, 20, 10)
Text(d, gain.origin(), "mix")
Text(d, delay.origin(), "delay")
d.connect(inp.right(), node.centre())
d.connect(node.centre(), adder.left())
d.connect(adder.right(), outp.left())
d.connect(node.centre(), delay.left())
d.connect(delay.right(), gain.left())
d.connect(gain.right(), adder.bottom())
d.render("images/delay.svg")

#echo diagram
################################################################################

d = Diagram(48, 23, "Echo")
#row 1
inp = IO(d, 5, 5)
node = Join(d, 10, 5)
adder = Adder(d, 35, 5)
adder.arrow(adder.bottom())
outp = IO(d, 40, 5)
#row 2
adder2 = Adder(d, 15, 10)
adder2.arrow(adder2.bottom())
delay = TextBlock(d, 20, 10, "D")
node2 = Join(d, 25, 10)
gain = GainBlock(d, 30, 10)
#row 3
gain2 = RGainBlock(d, 20, 15)

Text(d, gain.origin(), "mix")
Text(d, delay.origin(), "delay")
Text(d, gain2.origin(), "feedback")

d.connect(inp.right(), node.centre())
d.connect(node.centre(), adder.left())
d.connect(adder.right(), outp.left())
d.connect(node.centre(), adder2.left())
d.connect(adder2.right(), delay.left())
d.connect(delay.right(), node2.centre())
d.connect(node2.centre(), gain.left())
d.connect(gain.right(), adder.bottom())
d.connect(node2.centre(), gain2.right())
d.connect(gain2.left(), adder2.bottom())
d.render("images/echo.svg")

#reverb diagram
################################################################################

d = Diagram(53, 23, "Reverb")
#row 1
inp = IO(d, 5, 5)
node = Join(d, 10, 5)
adder = Adder(d, 40, 5)
adder.arrow(adder.bottom())
outp = IO(d, 45, 5)
#row 2
adder2 = Adder(d, 15, 10)
adder2.arrow(adder2.bottom())
delay = TextBlock(d, 25, 10, "D")
node2 = Join(d, 30, 10)
gain = GainBlock(d, 35, 10)
#row 3
gain2 = RGainBlock(d, 20, 15)
matrix = TextBlock(d, 25, 15, "M")

Text(d, gain.origin(), "mix")
Text(d, delay.origin(), "delay")
Text(d, gain2.origin(), "feedback")
Text(d, matrix.origin(), "mix matrix")

d.connect(inp.right(), node.centre())
d.connect(node.centre(), adder.left())
d.connect(adder.right(), outp.left())
d.connect(node.centre(), adder2.left())
d.connect(adder2.right(), delay.left(), 3)
d.connect(delay.right(), node2.centre(), 3)
d.connect(node2.centre(), gain.left())
d.connect(gain.right(), adder.bottom())
d.connect(node2.centre(), matrix.right(), 3)
d.connect(gain2.right(), matrix.left(), 3)
d.connect(gain2.left(), adder2.bottom(), 3)
d.render("images/reverb.svg")

#tremolo diagram
################################################################################

d = Diagram(23, 18, "Tremolo")

osc = Oscillator(d, 10, 5)

inp = IO(d, 5, 10)
mixer = Mixer(d, 10, 10)
outp = IO(d, 15, 10)

Text(d, osc.origin(), "rate")

d.connect(inp.right(), mixer.left())
d.connect(mixer.right(), outp.left())
d.connect(osc.bottom(), mixer.top())
d.render("images/tremolo.svg")

#vibrato diagram
################################################################################

d = Diagram(23, 18, "Vibrato")

osc = Oscillator(d, 10, 5)

inp = IO(d, 5, 10)
delay = TextBlock(d, 10, 10, "D")
delay.arrow(delay.top())
outp = IO(d, 15, 10)

Text(d, osc.origin(), "rate")

d.connect(inp.right(), delay.left())
d.connect(delay.right(), outp.left())
d.connect(osc.bottom(), delay.top())
d.render("images/vibrato.svg")

#flanger diagram
################################################################################

d = Diagram(38, 23, "Flanger")

inp = IO(d, 5, 5)
node = Join(d, 10, 5)
adder = Adder(d, 25, 5)
adder.arrow(adder.bottom())
outp = IO(d, 30, 5)

delay = TextBlock(d, 15, 10, "D")
gain = GainBlock(d, 20, 10)

osc = Oscillator(d, 15, 15)

Text(d, gain.origin(), "mix")
Text(d, delay.origin(), "delay")
d.connect(inp.right(), node.centre())
d.connect(node.centre(), adder.left())
d.connect(adder.right(), outp.left())
d.connect(node.centre(), delay.left())
d.connect(delay.right(), gain.left())
d.connect(gain.right(), adder.bottom())
d.connect(osc.top(), delay.bottom())
d.render("images/flanger.svg")

#chorus diagram
################################################################################

d = Diagram(38, 43, "Chorus")

inp = IO(d, 5, 5)
node = Join(d, 10, 5)
adder = Adder(d, 25, 5)
adder.arrow(adder.bottom())
adder.arrow(adder.left())
outp = IO(d, 30, 5)

node1 = Join(d, 10, 10)
delay1 = TextBlock(d, 15, 10, "D")
delay1.arrow(delay1.bottom())
gain1 = GainBlock(d, 20, 10)
adder1 = Adder(d, 25, 10)
adder1.arrow(adder1.bottom())
adder1.arrow(adder1.left())
osc1 = Oscillator(d, 15, 15)

node2 = Join(d, 10, 20)
delay2 = TextBlock(d, 15, 20, "D")
delay2.arrow(delay2.bottom())
gain2 = GainBlock(d, 20, 20)
adder2 = Adder(d, 25, 20)
adder2.arrow(adder2.bottom())
adder2.arrow(adder2.left())
osc2 = Oscillator(d, 15, 25)

delay3 = TextBlock(d, 15, 30, "D")
delay3.arrow(delay3.bottom())
gain3 = GainBlock(d, 20, 30)
osc3 = Oscillator(d, 15, 35)

Text(d, gain1.origin(), "mix 1")
Text(d, delay1.origin(), "delay 1")
Text(d, osc1.right(), "rate 1")
Text(d, gain2.origin(), "mix 2")
Text(d, delay2.origin(), "delay 2")
Text(d, osc2.right(), "rate 2")
Text(d, gain3.origin(), "mix 3")
Text(d, delay3.origin(), "delay 3")
Text(d, osc3.right(), "rate 3")

d.connect(inp.right(), node.centre())
d.connect(node.centre(), adder.left())
d.connect(adder.right(), outp.left())
d.connect(node.centre(), node1.centre())
d.connect(node1.centre(), delay1.left())
d.connect(delay1.right(), gain1.left())
d.connect(gain1.right(), adder1.left())
d.connect(node1.centre(), node2.centre())
d.connect(osc1.top(), delay1.bottom())
d.connect(adder1.top(), adder.bottom())
d.connect(node2.centre(), delay2.left())
d.connect(delay2.right(), gain2.left())
d.connect(gain2.right(), adder2.left())
d.connect(node2.centre(), delay3.left())
d.connect(osc2.top(), delay2.bottom())
d.connect(adder2.top(), adder1.bottom())
d.connect(delay3.right(), gain3.left())
d.connect(gain3.right(), adder2.bottom())
d.connect(osc3.top(), delay3.bottom())

d.render("images/chorus.svg")

#distortion diagram
################################################################################
from matplotlib import pyplot as plt

def cubic(x):
  return 1.5*(x - (x*x*x)/3)

def quadratic(x):
  if x > 2.0/3.0:
    return 1.0
  elif x < -2.0/3.0:
    return -1.0
  elif x > 1.0/3.0:
    return -(3 * x * x) + (4 * x) - (1.0/3.0)
  elif x < -1.0/3.0:
    return (3 * x * x) + (4 * x) + (1.0/3.0)
  else:
    return 2*x

def full_wave(x):
  return abs(x)

def half_wave(x):
  return x.clip(min=0)

def foldback(x):
  if x > 0:
    return -(4 * x * x) + (4 * x)
  else:
    return +(4 * x * x) + (4 * x)

def fuzz(x):
  return np.clip(3*x, -1.0, 1.0)

plt.figure()
plt.grid(True)
plt.title("Waveshaping Functions")
plt.xlabel("Input Signal")
plt.ylabel("Output Signal")
inp = np.linspace(-1.0, 1.0, 1000)
plt.plot(inp, cubic(inp), label = "cubic")
plt.plot(inp, [quadratic(i) for i in inp], label = "quadratic")
plt.plot(inp, fuzz(inp), label = "fuzz")
plt.legend()
plt.savefig("images/waveshapers.svg")

plt.figure()
plt.grid(True)
plt.title("Unusual Waveshaping Functions")
plt.xlabel("Input Signal")
plt.ylabel("Output Signal")
plt.plot(inp, full_wave(inp), label="full_wave")
plt.plot(inp, half_wave(inp), label="half_wave")
plt.plot(inp, [foldback(i) for i in inp], label="foldback")
plt.legend()
plt.savefig("images/unusual_waveshapers.svg")


signal = np.sin(np.linspace(0, 5*2*np.pi, 1000))
cubic = cubic(signal)
quadratic = [quadratic(i) for i in signal]
fuzz = fuzz(signal)
half_wave = half_wave(signal)
full_wave = full_wave(signal)
foldback = [foldback(i) for i in signal]

plt.figure()
plt.grid(True)
plt.title("Distortion")
plt.xlabel("Time (samples)")
plt.ylabel("Amplitude")
plt.plot(signal, label="clean")
plt.plot(cubic, label="cubic")
plt.plot(quadratic, label="quadratic")
plt.plot(fuzz, label="fuzz")
plt.legend()
plt.savefig("images/distortion.svg")

figure, axis = plt.subplots(3, 1)

figure.suptitle("Crazy Distortion")
plt.xlabel("Time (samples)")
axis[0].plot(signal, "b", label="clean")
axis[0].plot(half_wave, "r", label="half_wave")
axis[0].set(ylabel = "Amplitude")
axis[0].grid(True)
axis[0].legend()

axis[1].plot(signal, 'b', label="clean")
axis[1].plot(full_wave, 'g', label="full_wave")
axis[1].set(ylabel = "Amplitude")
axis[1].grid(True)
axis[1].legend()

axis[2].plot(signal, 'b', label="clean")
axis[2].plot(foldback, 'orange', label="foldback")
axis[2].set(ylabel = "Amplitude")
axis[2].grid(True)
axis[2].legend()
plt.savefig("images/crazy_distortion.svg")
