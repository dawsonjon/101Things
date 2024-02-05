#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: image2fbuf.py
# description:
#
# Converts an image file into a binary file that can be read directly into a
# micropython frameBuf object.
#
# License: MIT
#

import imageio
import sys

input_file = sys.argv[1]
output_file = sys.argv[2]

im = imageio.imread(input_file)
h, w, c = im.shape
      
bytevals = []
for y in range(h):
  for i in range(w//8):
    byteval = 0
    for j in range(8):
      x = (i*8)+j   
      byteval <<= 1
      if im[y][x][0] > 0:
        byteval |= 1
    bytevals.append(byteval) 

with open(output_file, "wb") as outf:
  outf.write(bytes(bytevals))
