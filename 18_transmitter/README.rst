Transmitter Example Usage
=========================

A simple transmitter application is included in transmitter.cpp. This allows
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
