.. image:: https://readthedocs.org/projects/chips-20/badge/?version=latest
    :target: https://readthedocs.org/projects/chips-20/badge/?version=latest

101 Things
==========

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

.. code::

  mkdir build
  cd build
  cmake -DPICO_BOARD=pico_w -DPICO_SDK_PATH=~/pico/pico-sdk ..
  make

Documentation
-------------

`ReadTheDocs <https://101-things.readthedocs.io/en/latest/>`_.
