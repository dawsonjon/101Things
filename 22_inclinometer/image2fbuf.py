#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: image2c.py
# description:
#
# Converts an image file into a C array
# License: MIT
#

import imageio
import sys

input_file = sys.argv[1]
name = input_file.split(".")[0]
output_file = name+".h"

im = imageio.imread(input_file)
h, w, c = im.shape

pixels = []
for y in range(h):
  for x in range(w):
    r, g, b = im[y][x][:3]
    color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | b >> 3;
    color = (color >> 8 | color << 8) & 0xffff;
    pixels.append("0x%04x"%color)
pixels = ",\n".join(pixels)
contents = "static const uint16_t __in_flash() %s[] = {%s};"%(name, pixels)

with open(output_file, "w") as outf:
  outf.write(contents);
     
