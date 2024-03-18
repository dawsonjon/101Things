Transmitter Examples
====================

Microphone Example
------------------

*transmitter_microphone_example.cpp*

This is a very minimal usage example, the transmit mode and frequency are hard
coded. Audio input is taken from a microphone using the builtin ADC.


Serial Example
--------------

*transmitter_serial_example.cpp*

This is a more complete usage example, the transmit mode and frequency are 
controlled through the USB serial interface. Audio data is streamed from a host PC
via the USB serial connection.

The example may be controlled using the transmit.py utility.

Hardware Variations
-------------------

The original transmitter used an FST3253 analog switch as a balanced mixer. In
this design, two PWM outputs provide complimentary "balanced" inputs to the
mixer. One non inverted output swings between 0 and VCC/2, the inverted output
swings between vcc/2 and vcc.

The polar modulated amplifier behaves as an unbalanced mixer. It only requires
a single PWM output that swings between 0 and vcc. However the second
(inverted) pwm output has been retained as this allows ether inverting or
non-inverting gate drivers to be used.

To allow both hardware variants to be used, two build targets have been defined
for each of the transmitter examples.

transmitter_microphone_example_mixer.uf2 and
transmitter_serial_example_mixer.uf2 support the FST3253 mixer design.

transmitter_microphone_example_polar_amplifier.uf2 and
transmitter_serial_example_polar_amplifier.uf2 support the polar amplifier design.


Transmitter Serial Example Usage
================================

A simple transmitter application is included in *transmitter_serial_example.cpp*. This allows
the frequency and mode to be selected, and also to allow transmit data to be
streamed through the USB Serial Interface.

The transmitter expects audio data in 8-bit signed format. The expected sample rate
depends on the transmit mode.

==== ================
Mode Sample Rate kS/s
==== ================
FM        15
AM       12.5
LSB       10
USB       10
==== ================

The sox utility can be used to convert from most common audio formats. 

.. code:: bash

  #generate a file to send in SSB mode
  sox FileToConvert.mp3 -t s8 -r 10k myfile.wav

Using the Python Transmit Utility
=================================

A simple python utility can stream audio files over USB to the transmitter.

To send audio using FM modulation at 10MHz.

.. code:: bash

  $python transmit.py test_file_15k.wav 
  Available Ports
  0 /dev/ttyS4: n/a [n/a]
  1 /dev/ttyACM0: Pico - Board CDC [USB VID:PID=2E8A:000A SER=E6609CB2D37B9828 LOCATION=1-1:1.0]
  Select COM port >
  1
  Enter Frequency Hz>
  10000000
  Enter mode AM/FM/LSB/USB >
  FM

To send audio using USB modulation at 14.175MHz.

.. code:: bash

  $python transmit.py test_file_10k.wav 
  Available Ports
  0 /dev/ttyS4: n/a [n/a]
  1 /dev/ttyACM0: Pico - Board CDC [USB VID:PID=2E8A:000A SER=E6609CB2D37B9828 LOCATION=1-1:1.0]
  Select COM port >
  1
  Enter Frequency Hz>
  14.175e6
  Enter mode AM/FM/LSB/USB >
  USB
