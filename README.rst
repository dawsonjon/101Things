.. image:: https://readthedocs.org/projects/chips-20/badge/?version=latest
    :target: https://readthedocs.org/projects/chips-20/badge/?version=latest

101 Things
==========

Welcome to *101 Things*, a collection of projects that celebrate creativity and
hands-on ingenuity.  If you share a passion for making things, you've come to
the right place. Each project uses simple designs, and basic tools, to push the
creative boundaries of a home tinkerer.  Join me on this exploration of
crafting, coding, and constructing, as we unlock the magic of making, one
project at a time.

Documentation
-------------

`ReadTheDocs <https://101-things.readthedocs.io/en/latest/>`_.

Projects so far
---------------

Pi Pico Rx - A crystal radio for the digital age?
"""""""""""""""""""""""""""""""""""""""""""""""""

.. image:: docs/images/thumbnail5.png
  :target: https://github.com/dawsonjon/PicoRX/assets/717535/4846c78d-21c3-42e8-bcdf-278cc2bdadcb

DIY Multi-Effect Guitar Unit
""""""""""""""""""""""""""""

.. image:: docs/images/headphone_amp.jpg
  :align: center

Raspberry Pi Pico Ham Radio Transmitter
"""""""""""""""""""""""""""""""""""""""

.. image::  docs/images/HamTransmitterThumbnail.png
  :target: https://youtu.be/PbhmQfPkNL0

Raspberry Pi Power and SWR Meter
"""""""""""""""""""""""""""""""""""""""

.. image::  docs/images/power_meter_thumbnail.png
  :target: https://youtu.be/_b22atCT8KU

Raspberry Pi Pico FM Radio
"""""""""""""""""""""""""""

.. image:: docs/images/fm_radio_thumbnail.png
  :target: https://youtu.be/bj8MgL6k2tU

8-inch Dobsonian Telescope
""""""""""""""""""""""""""

.. image::  docs/images/telescope_thumbnail.png
  :target: https://youtu.be/KxbAh8ndkbY

Breadboard Software Defined Radio
"""""""""""""""""""""""""""""""""

.. image::  docs/images/breadboard_radio_thumbnail.png
  :target: https://youtu.be/lS1ZRMIYLjA

Getting the Code
----------------

.. code::

  sudo apt install git
  git clone https://github.com/dawsonjon/101Things.git


Install Pi Pico SDK
-------------------

Follow the `Getting started with the Raspberry Pi Pico <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf>`_ quick start guide to install the C/C++ SDK.

.. code::

  sudo apt install wget #if wget not installed
  wget https://raw.githubusercontent.com/raspberrypi/pico-setup/master/pico_setup.sh
  chmod +x pico_setup.sh
  ./pico_setup.sh
  

Build Projects
--------------

Some projects may target either the pico or pico_w variant, although those that use the wireless functionality can only be built on the pi-pico.

To build for pico_w:
""""""""""""""""""""

.. code::

  mkdir build
  cd build
  cmake -DPICO_BOARD=pico_w -DPICO_SDK_PATH=~/pico/pico-sdk ..
  make

To build for pico:
""""""""""""""""""

.. code::

  mkdir build
  cd build
  cmake -DPICO_BOARD=pico -DPICO_SDK_PATH=~/pico/pico-sdk ..
  make
