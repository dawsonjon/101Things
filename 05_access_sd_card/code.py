#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: code.py
# description:
# License: MIT
#

import board
import busio
import sdcardio
import storage

# Use the board's primary SPI bus
#spi = board.SPI()
# Or, use an SPI bus on specific pins:
print(dir(board))
spi = busio.SPI(board.GP6, MOSI=board.GP7, MISO=board.GP4)

# For breakout boards, you can choose any GPIO pin that's convenient:
cs = board.GP5
# Boards with built in SPI SD card slots will generally have a
# pin called SD_CS:
#cs = board.SD_CS

sdcard = sdcardio.SDCard(spi, cs)
vfs = storage.VfsFat(sdcard)

storage.mount(vfs, "/sd")

with open("/sd/test.txt", "w") as f:
    f.write("Hello world!\r\n")

with open("/sd/test.txt", "r") as f:
    print("Read line from file:")
    print(f.readline())
