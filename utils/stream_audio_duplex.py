import socket
import time
import os
import sys
import subprocess

SERVER_ADDR = '192.168.1.121'
SERVER_PORT = 4242

while 1:

    #Use sox utility to stream access sound card
    record_process = subprocess.Popen(["rec", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdout=subprocess.PIPE)
    play_process = subprocess.Popen(["play", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdin=subprocess.PIPE)
    
    #connect via TCP socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_ADDR, SERVER_PORT))
    s.setblocking(0)
    print("connected to", SERVER_ADDR, SERVER_PORT, file=sys.stderr)

    #transfer sampels between wifi and soundcard
    while 1:
      samples = record_process.stdout.read(1024)
      s.sendall(samples)
      try:
        samples = s.recv(1024)
        play_process.stdin.write(samples)
      except BlockingIOError:
        pass
      

    s.close()
