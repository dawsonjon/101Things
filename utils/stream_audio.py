#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: stream_audio.py
# description:
# License: MIT
#

import socket
import time
import os
import sys
import subprocess

SERVER_ADDR = '192.168.1.122'
SERVER_PORT = 4242

while 1:

    process = subprocess.Popen(["rec", "--buffer", "17", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdout=subprocess.PIPE)
    

    #play clip
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_ADDR, SERVER_PORT))
    print("connected to", SERVER_ADDR, SERVER_PORT, file=sys.stderr)

    while 1:
      #sys.stdout.buffer.write(samples)
      samples = process.stdout.read(1024)
      s.sendall(samples)
      

    s.close()
