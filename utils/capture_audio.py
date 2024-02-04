#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: capture_audio.py
# description:
# License: MIT
#

import socket
import time
import os
import sys
import subprocess

SERVER_ADDR = '192.168.1.101'
SERVER_PORT = 4242

while 1:

    process = subprocess.Popen(["play", "-t", "u8", "-r", "10000", "-"], stdin=subprocess.PIPE)
    

    #play clip
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_ADDR, SERVER_PORT))
    print("connected to", SERVER_ADDR, SERVER_PORT, file=sys.stderr)

    while 1:
      samples=s.recv(1024)
      process.stdin.write(samples)
      

    s.close()

